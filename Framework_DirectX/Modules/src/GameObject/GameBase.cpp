#include "GameObject/GameBase.h"
#include "Component/ComponentBase.h"
#include "Component/Render/RenderCompBase.h"
#include "Component/Collider/ColliderCompBase.h"
#include "Component/MovementComp.h"
#include "ConstantBuffer.h"
#include <DirectXColors.h>
#include <vector>
#include "Manager/ObjectParamManager.h"
#include "Manager/LevelManager.h"
#include "GameObject/ObjectParam.h"
#include "GameObject/Spawner.h"

PrimitiveObjDesc::PrimitiveObjDesc()
	: paramKey("")
	, instanceNum(1)
	, psoKey(GPSOkey::SIMPLE)
	, self(CollisionTag::disable)
	, targetLayer(0)
	, ColliderOffsetT(DirectX::SimpleMath::Vector3::Zero)
	, ColliderOffsetR(DirectX::SimpleMath::Vector3::Zero)
	, ColliderOffsetS(DirectX::SimpleMath::Vector3::One)
	, hasCollider(false)
	, hasRender(false)
	, hasMover(false)
{
}



GameBase::GameBase(
	std::string name, 
	uint32_t instanceNum, 
	const std::string jsonActorKey)
	: m_Name(std::move(name))
	, m_TimeScale(1.0)
	, m_InstanceNum(instanceNum)
	, m_InstanceData(instanceNum)
	, m_Spawner(nullptr)
{
	addComponent(MAKE_UNIQUE_LOG<TransformComp>(this));
	if (!m_InstanceData.empty())
	{
		m_InstanceData[0].m_Transform = static_cast<TransformComp*>(getComponent(CompType::TransformComp));
		m_InstanceData[0].m_Color = DirectX::Colors::White;
	}

	if (!jsonActorKey.empty()) {
		std::string key = jsonActorKey;
		auto mgr = ObjectParamManager::m_Instance;
		auto param = mgr->getParam(key);
		if (!param) {
			mgr->addParam(key, key, MAKE_UNIQUE_LOG<ObjectParam>());
			param = mgr->getParam(key);
		}
		setParam(param);
		if (!param->bIsInitSceneBoundingData()) {
			mgr->updateSceneBindingData(key);
		}
	}
}


GameBase::~GameBase()
{
}

void GameBase::term() {
	m_Components.clear();
}

void GameBase::restart()
{

}

void GameBase::preUpdate(uint32_t frameIndex, double deltaTime)
{

	if (!m_BIsEnable) { return; }

	const double dt = deltaTime * m_TimeScale;
	if (dt <= 0.0) return;

	for (auto& comp : m_Components) {
		comp->preUpdate(frameIndex, dt);
	}
	implPreupdate(frameIndex, dt);
	for (auto& child : m_Children) {
		if (child) {
			child->preUpdate(frameIndex, dt);
		}
	}
}

void GameBase::start() {
	if (!m_BIsEnable) { return; }
	if (bIsStarted) return;

	for (auto& comp : m_Components) {
		comp->start();
	}
	implStart();
	for (auto& child : m_Children) {
		if (child) {
			child->start();
		}
	}
	bIsStarted = true;
}

void GameBase::update(uint32_t frameIndex, double deltaTime) {
	if (!m_BIsEnable) { return; }
	const double dt = deltaTime * m_TimeScale;
	if (dt <= 0.0) return;

	for (auto& comp : m_Components) {
		comp->update(frameIndex, dt);
	}
	implupdate(frameIndex, dt);
	for (auto& child : m_Children) {
		if (child) {
			child->update(frameIndex, dt);
		}
	}
}

void GameBase::postUpdate(uint32_t frameIndex, double deltaTime)
{
	if (!m_BIsEnable) { return; }
	const double dt = deltaTime * m_TimeScale;
	if (dt <= 0.0) return;

	for (auto& comp : m_Components) {
		comp->postUpdate(frameIndex, dt);
	}
	implPostupdate(frameIndex, dt);
	for (auto& child : m_Children) {
		if (child) {
			child->postUpdate(frameIndex, dt);
		}
	}
}

void GameBase::onHit(const HitInfo& info)
{
	if (!m_BIsEnable) { return; }
	for (auto& comp : m_Components) {
		comp->collisionBegin(info);
	}
	implOnHit(info);
	for (auto& child : m_Children) {
		if (child) {
			child->onHit(info);
		}
	}
}

void GameBase::preRender(const uint32_t frameIndex)
{
	if (!m_BIsEnable) { return; }
	for (auto& comp : m_Components) {
		comp->preRender(frameIndex);
	}
	implPreRender(frameIndex);
	for (auto& child : m_Children) {
		if (child) {
			child->preRender(frameIndex);
		}
	}
}

void GameBase::postRender(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer)
{
	if (!m_BIsEnable) { return; }
	for (auto& comp : m_Components) {
		comp->postRender(pCmdList, frameIndex, lightBuffer);
	}
	implPostRender(pCmdList, frameIndex, lightBuffer);
	for (auto& child : m_Children) {
		if (child) {
			child->postRender(pCmdList, frameIndex, lightBuffer);
		}
	}
}

#pragma region Implementation

void GameBase::initStaticTransform(
	DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 rot, DirectX::SimpleMath::Vector3 scale)
{
	addComponent(MAKE_UNIQUE_LOG<MovementComp>(this));
	auto mover = static_cast<MovementComp*>(getComponent(CompType::MovementComp));
	mover->setPos(pos);
	mover->setRot(rot);
	mover->setScale(scale);
	removeComponent(CompType::MovementComp);
}

void GameBase::setParent(GameBase* parent)
{
	if (m_Parent == parent) return;
	//	循環チェック
	auto p = parent;
	while (p) {
		if (p == this) { return; }
		p = p->parent();
	}
	//	元親から自身を削除
	if (m_Parent) {
		auto& parentChildren = m_Parent->m_Children;
		parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), this), parentChildren.end());
	}
	m_Parent = parent;
	if (m_Parent) {
		m_Parent->m_Children.push_back(this);
	}
}


void GameBase::enable()
{
	m_BIsEnable = true;
	if (auto render = getComponent(CompType::RenderComp)) {
		static_cast<RenderCompBase*>(render)->setVisible(true);
	}
	if (auto collider = getComponent(CompType::ColliderComp)) {
		static_cast<ColliderCompBase*>(collider)->enable();
	}
	for (auto& c : m_Children) {
		c->enable();
	}
}

void GameBase::disable()
{
	m_BIsEnable = false;
	if (auto render = getComponent(CompType::RenderComp)) {
		static_cast<RenderCompBase*>(render)->setVisible(false);
	}
	if (auto collider = getComponent(CompType::ColliderComp)) {
		static_cast<ColliderCompBase*>(collider)->disable();
	}
	if (m_Spawner) {
		m_Spawner->despawn(this);
	}
	for (auto& c : m_Children) {
		c->disable();
	}
}

void GameBase::disableNextFrame()
{
	if (!m_BIsEnable) { return; }

	LevelManager::m_Instance->requestDisableObject(m_Name);
}

void GameBase::enableNextFrame()
{
	if (m_BIsEnable) { return; }
	LevelManager::m_Instance->requestEnableObject(m_Name);
}

void GameBase::implPreupdate(uint32_t frameIndex, double deltaTime)
{

}

void GameBase::implStart()
{
}

void GameBase::implupdate(uint32_t frameIndex, double deltaTime) {
}

void GameBase::implPostupdate(uint32_t frameIndex, double deltaTime)
{
}

void GameBase::implPreRender(const uint32_t frameIndex)
{
}

void GameBase::implPostRender(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer)
{
}

#pragma endregion


#pragma region Component

void GameBase::addComponent(LogPtr<ComponentBase> component)
{
	if (!component) { return; }
	//	重複チェック
	auto type = component->compType();
	if (getComponent(type)) { return; }

	m_Components.push_back(std::move(component));
}


bool GameBase::removeComponent(CompType type)
{
	auto it = std::find_if(
		m_Components.begin(),
		m_Components.end(),
		[type](const LogPtr<ComponentBase>& comp) {
			return comp->compType() == type;
		});

	if (it != m_Components.end()) {
		m_Components.erase(it);
		return true;
	}
 
	ELOG("Error : GameBase::removeComponent");
	return false;
}


ComponentBase* GameBase::getComponent(CompType type)const
{
	auto it = std::find_if(
		m_Components.begin(),
		m_Components.end(),
		[type](const LogPtr<ComponentBase>& comp) {
			return comp->compType() == type;
		});

	if (it != m_Components.end()) {
		return it->get();
	}
	return nullptr;
}

const std::vector<InstanceData>& GameBase::instanceDataList()const
{
	assert(!m_InstanceData.empty());
	return m_InstanceData;
}

void GameBase::setParam(const ObjectParam* p)
{
	assert(p);
	this->m_Param = p; 
}


CB::InstanceParamBuffer* GameBase::getInstanceBuffer(uint32_t frameIndex) 
{
	auto render = dynamic_cast<RenderCompBase*>(
		getComponent(CompType::RenderComp));
	if (!render) { return nullptr; }

	return render->getInstanceParam(frameIndex);
}

#pragma endregion

