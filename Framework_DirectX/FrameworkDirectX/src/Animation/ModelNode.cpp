#include "Animation/ModelNode.h"
#include "Animation/ModelMesh.h"
#include "Animation/Model.h"
#include "Animation/Bone.h"

#include "Animation/convert_assimp.h"
#include <assimp/scene.h>
#include <string>
#include <iostream>


void ModelNode::init(Model* model, aiNode* node)
{
	m_Model = model;

	std::string name = convert_assimp::Format(node->mName);
	setName(std::move(name));
	setLocalMatrix(convert_assimp::Format(node->mTransformation));

	m_MeshIndexList.reserve(node->mNumMeshes);
	for (u_int i = 0; i < node->mNumMeshes; i++) {
		m_MeshIndexList.push_back(node->mMeshes[i]);
	}
	model->m_NodeMap[this->name()] = this;
}

void ModelNode::setName(const std::string& name)
{
	m_Name = name;
	m_OriginalName = m_Name;
}
void ModelNode::Destroy() {
	m_Model = nullptr;
	m_Parent = nullptr;
	m_Children.clear();
}

void ModelNode::setIndexList(Model* model)
{
	if (!model) return;
	if (name().empty() || name()=="") { return; }

	std::vector<ModelMesh*> meshList;
	for (auto& idx : m_MeshIndexList) {
		meshList.push_back(model->m_ModelMeshList[idx].get());
		model->m_MeshListMap[name()] = meshList;
	}
}

void ModelNode::updateMatrix()
{
	m_LocalMatrix =
		DirectX::SimpleMath::Matrix::CreateScale(m_LocalScale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_LocalRotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(m_LocalPosition);
		
	//std::cout << m_Name << std::endl;;
	//std::cout << "pos : "
	//	<< m_LocalPosition.x << " / "
	//	<< m_LocalPosition.y << " / "
	//	<< m_LocalPosition.z << " / "
	//	<< std::endl;
	//std::cout << "rot : "
	//	<< m_LocalRotation.x << " / "
	//	<< m_LocalRotation.y << " / "
	//	<< m_LocalRotation.z << " / "
	//	<< m_LocalRotation.w << std::endl;
	//std::cout << "scale : "
	//	<< m_LocalScale.x << " / "
	//	<< m_LocalScale.x << " / "
	//	<< m_LocalScale.x << " / "
	//	<< std::endl;
}


void ModelNode::updateChildMatrix(DirectX::SimpleMath::Matrix parent, const std::vector<Bone*>& boneList, const FlipMode* mode)
{
	updateMatrix();
	DirectX::SimpleMath::Matrix myLocal = m_LocalMatrix;

	if (mode && !m_Parent) {
		if (mode->flipX) {
			myLocal = DirectX::SimpleMath::Matrix::CreateScale(-1.0f, 1.0f, 1.0f) * m_LocalMatrix;
		}
	}

	m_WorldMatrix = myLocal * parent;
		
	for (auto& i : m_BoneIndexList) {
		auto bone = boneList[i];
		bone->setWorldMatrix(m_WorldMatrix);
	}
	for (auto& c : m_Children) {
		c->updateChildMatrix(m_WorldMatrix, boneList);
	}
}

void ModelNode::setLocalMatrix(const DirectX::SimpleMath::Matrix& matrix)
{
	DirectX::SimpleMath::Matrix localMat = matrix;

	m_LocalMatrix = localMat;
	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion rot;
	m_LocalMatrix.Decompose(scale, rot, pos);
	setLocalRotation(rot);
	setLocalPosition(pos);
	setLocalScale(scale);
}

DirectX::SimpleMath::Matrix ModelNode::calcWorldMatrix() const
{
	if (m_Parent)
	{
		return m_LocalMatrix * m_Parent->calcWorldMatrix();
	}

	return m_LocalMatrix;
}

void ModelNode::setParent(ModelNode* obj)
{
	if (obj) {
		m_Parent = obj;
		obj->addChild(this);
	}
}

ModelNode* ModelNode::parent() const
{
	return m_Parent;
}

void ModelNode::addChild(ModelNode* obj)
{
	if (!obj) { return; }
	for (auto& child : m_Children) {
		if (child == obj) { return; }
	}

	m_Children.push_back(obj);
}

std::vector<ModelNode*>& ModelNode::children()
{
	return m_Children;
}

void ModelNode::calcTPoseNode(DirectX::SimpleMath::Matrix parent)
{
	using namespace DirectX::SimpleMath;

	updateMatrix();

	Matrix myWorld = m_LocalMatrix * parent;
 	myWorld.Invert(m_GlobalInvertTPose);

	for (auto& i : m_BoneIndexList) {
		auto bone = m_Model->findBoneFromIndex(i);
		if (bone) {
			bone->setInverseMatrix(m_GlobalInvertTPose);
		}
	}

	for (auto& c : m_Children) {
		c->calcTPoseNode(myWorld);
	}
}



