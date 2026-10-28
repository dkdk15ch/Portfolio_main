#include "Component/Animator.h"
#include <algorithm>
#include "Animation/Model.h"
#include "Animation/ModelMesh.h"
#include "Animation/Bone.h"
#include "Animation/Animation.h"
#include "Animation/AnimationKeyFrame.h"
#include "Animation/AnimationNode.h"
#include "ConstantBuffer.h"
#include "App.h"
#include "GraphicContext.h"
#include "Logger.h"

Animator::Animator(GameBase* owner)
	: ComponentBase(owner)
	, m_BoneMatrixBuffer{}
	, m_TargetModel(nullptr)
	, ROOT_BONE_INDEX(0)
	, m_CurrentAnimation(nullptr)
	, m_NextAnimation(nullptr)
	, m_AnimationTime(0.0)
	, m_Duration(0.25)
	, m_TimeScale(1.0)
	, m_Mode(MAKE_UNIQUE_LOG<FlipMode>())
{
	auto graphicContext = GraphicContext::m_Instance;
	auto name = std::wstring(L"Animator");
	auto FrameCount = App::m_FrameCount;
	m_BoneMatrixBuffer.reserve(FrameCount);

	// 変換行列用の定数バッファの生成.
	for (auto i = 0u; i < FrameCount; ++i)
	{
		auto boneBuffer = MAKE_UNIQUE_LOG<ConstantBuffer>();
		if (!boneBuffer) {
			ELOG("Error : Out of memory.");
			return;
		}
		// 定数バッファ初期化.
		if (!boneBuffer->init(
			graphicContext->device(),
			graphicContext->poolRes(),
			sizeof(CB::BoneParameter), name))
		{
			ELOG("Error : ConstantBuffer::init() Failed.");
			return;
		}
		auto pBuffer = boneBuffer->getPtr<CB::BoneParameter>();
		for (int i = 0; i < 256; i++) {
			pBuffer->bone[i] =
				DirectX::SimpleMath::Matrix::Identity;
		}
		m_BoneMatrixBuffer.push_back(std::move(boneBuffer));
	}
}

void Animator::setModel(const std::string& targetKey)
{
	m_TargetModel = m_PAssetManager->getModel(targetKey);
	assert(m_TargetModel);
	if (m_TargetModel) {
		// 中身をコピー
		m_cloneBoneList = m_TargetModel->cloneBoneList();
		m_RefcloneBoneList = GetPointers(m_cloneBoneList);
		m_CloneNode = m_TargetModel->cloneModelNodeList();
		ROOT_BONE_INDEX = m_TargetModel->ROOT_NODE_INDEX;		
	}
}

void Animator::setNextAnimation(const std::string& key) {
	auto nextAni = m_TargetModel->findAnimation(key);
	if (!nextAni) {
		std::string k = m_TargetModel->name() + "|" + key;
		nextAni = m_TargetModel->findAnimation(k);
	}
	setNextAnimation(nextAni);
}


void Animator::preUpdate(uint32_t frameIndex, double deltaTime)
{
	if (m_NextAnimation) {
		m_CurrentAnimation = m_NextAnimation;
		m_NextAnimation = nullptr;
		m_AnimationTime = 0.0;
		m_CurrentFrame = 0.0;
	}
}

void Animator::update(uint32_t , double deltaTime)
{
	updateAnimation(deltaTime);
}

void Animator::postUpdate(uint32_t frameIndex, double deltaTime)
{
	//	ワールド行列 = ボーン行列 x ワールド行列(parent)
	updateMatrices();
}

void Animator::preRender(const uint32_t frameIndex)
{
	auto ptr = m_BoneMatrixBuffer[frameIndex]->getPtr<CB::BoneParameter>();
	auto boneSize = uint32_t(m_cloneBoneList.size());
	for (uint32_t i = 0; i < boneSize; ++i) {
		// スキニング行列 = バインド逆(ボーンオフセット)行列 x ボーン行列
		auto& invM = m_cloneBoneList[i]->inverseMatrix();
		auto& boneM = m_cloneBoneList[i]->worldMatrix();
		DirectX::XMMATRIX transposedMatrix =
			DirectX::XMMatrixTranspose(invM * boneM);
		DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&ptr->bone[i]), transposedMatrix);
	}
	//	hlslで 頂点 x スキニング行列
}

void Animator::term()
{
	m_BoneMatrixBuffer.clear();
	m_cloneBoneList.clear();
}

const std::string& Animator::meshName()
{
	return m_TargetModel->m_ModelMeshList[0]->name();
}

Mesh* Animator::mesh()
{
	return m_TargetModel->m_ModelMeshList[0] == nullptr ?
		nullptr : m_TargetModel->m_ModelMeshList[0].get();
}

double Animator::duration()const {
	if (!m_CurrentAnimation) { return 1000.0; }

	return m_CurrentAnimation->m_Duration;
}

void Animator::setFlipX(bool bIsEnabel)
{
	m_Mode->flipX = bIsEnabel;
}

void Animator::updateAnimation(double deltaTime)
{
	using namespace DirectX;
	if (m_TimeScale == 0.0) { return; }

	if (!m_CurrentAnimation) { return; }
	double currentTick = m_AnimationTime * m_CurrentAnimation->m_TicksPerSecond;

	auto boneCount = m_cloneBoneList.size();
	auto& AnimaMap = m_CurrentAnimation->m_NodeMap;
	for (uint32_t i = 0; i < boneCount; ++i) {
		auto bone = m_cloneBoneList[i].get();
		//	アニメーションノード
		auto it = AnimaMap.find(bone->name());
		if (it == AnimaMap.end()) {
			ELOG("Error : Animator::UpdateAnima - not found bone");
			continue;
		}
		//	一番近いノードを探す
		auto segment = (*it).second->findNearestKeys(m_AnimationTime);
		const auto& start = std::get<0>(segment);
		const auto& last = std::get<1>(segment);
		//	線形補間
		auto range = float(last.time() - start.time());
		XMVECTOR pos = DirectX::XMLoadFloat3(&start.position());
		XMVECTOR rot = DirectX::XMLoadFloat4(&start.rotation());
		XMVECTOR scale = DirectX::XMLoadFloat3(&start.scale());
		if (range > 0) {
			auto rate = float(m_AnimationTime - start.time()) / range;
			pos += (XMLoadFloat3(&last.position()) - XMLoadFloat3(&start.position())) * rate;
			rot = XMQuaternionNormalize(XMQuaternionSlerp(
				XMLoadFloat4(&start.rotation()),
				XMLoadFloat4(&last.rotation()),
				rate));
			scale += (XMLoadFloat3(&last.scale()) - XMLoadFloat3(&start.scale())) * rate;
		} 
		//	ローカル更新
		auto nodeIdx = bone->nodeIndex();
		if (nodeIdx < 0 || static_cast<size_t>(nodeIdx) >= m_CloneNode.size()) {
			continue;
		}

		if (bone->name().find("Thumb") != std::string::npos ||
			bone->name().find("Index") != std::string::npos ||
			bone->name().find("Middle") != std::string::npos ||
			bone->name().find("Ring") != std::string::npos ||
			bone->name().find("Pinky") != std::string::npos 
			) {
		//std::cout << bone->GetName() << std::endl;
		//std::cout << "Start :";
		//std::cout << " /Tick ="		<< start.tick() << std::endl;
		//std::cout << " /Time ="		<< start.time() << std::endl;
		//std::cout << " /posX ="		<< start.position().x;
		//std::cout << " /posY ="		<< start.position().y;
		//std::cout << " /posZ ="		<< start.position().z << std::endl;
		//std::cout << " /rotX ="		<< start.rotation().x;
		//std::cout << " /rotY ="		<< start.rotation().y;
		//std::cout << " /rotZ =" << start.rotation().z;
		//std::cout << " /rotZ ="		<< start.rotation().w << std::endl;
		//std::cout << " /scaleX ="	<< start.scale().x;
		//std::cout << " /scaleY ="	<< start.scale().x;
		//std::cout << " /scaleZ ="	<< start.scale().x << std::endl;

		//std::cout << "Last :";
		//std::cout << " /Tick ="   << last.tick() << std::endl;
		//std::cout << " /Time ="   << last.time() << std::endl;
		//std::cout << " /posX ="   << last.position().x;
		//std::cout << " /posY ="   << last.position().y;
		//std::cout << " /posZ ="   << last.position().z << std::endl;
		//std::cout << " /rotX ="   << last.rotation().x;
		//std::cout << " /rotY ="   << last.rotation().y;
		//std::cout << " /rotZ ="   << last.rotation().z << std::endl;
		//std::cout << " /scaleX =" << last.scale().x;
		//std::cout << " /scaleY =" << last.scale().x;
		//std::cout << " /scaleZ =" << last.scale().x << std::endl;


		
		//continue;
	}
		m_CloneNode[nodeIdx]->setLocalPosition(pos);
		m_CloneNode[nodeIdx]->setLocalRotation(rot);
		m_CloneNode[nodeIdx]->setLocalScale(scale);
	}
	//	タイム加算、終了時処理
	m_AnimationTime += deltaTime * m_TimeScale * m_CurrentAnimation->timeScale();
	m_CurrentFrame = static_cast<uint32_t>(m_AnimationTime * m_CurrentAnimation->m_TicksPerSecond);
	if (m_CurrentAnimation->m_TotalTime < m_AnimationTime) {
		m_AnimationTime = 0.0;
		m_CurrentFrame = 0;
		if (m_NextAnimation) {
			m_CurrentAnimation = m_NextAnimation;
			m_NextAnimation = nullptr;
		}
		if (!m_CurrentAnimation->isLoop()) {
			m_CurrentAnimation = nullptr;
		}
	}
}

ModelNode* Animator::findCloneNode(const std::string& name) {
	const auto node = m_TargetModel->findNode(name);
	if (!node) { return nullptr; }

	auto nodeIdx = m_TargetModel->findNodeIndex(node);
	return nodeIdx != UINT_MAX ? m_CloneNode[nodeIdx].get() : nullptr;
}

bool Animator::isPlaying() const
{
	if (!m_CurrentAnimation) { return false; }
	return m_CurrentAnimation->m_TotalTime > m_AnimationTime;
}

void Animator::updateMatrices()
{
	m_CloneNode[0]->updateChildMatrix(DirectX::SimpleMath::Matrix::Identity, m_RefcloneBoneList, m_Mode.get());
}

