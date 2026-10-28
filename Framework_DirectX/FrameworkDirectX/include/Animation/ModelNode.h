#pragma once
#include <vector>
#include <memory>
#include <string>
#include <SimpleMath.h>
#include "PointerHelper.h"

class Model;
struct aiNode;
class Bone;

struct ModelNodeBlob {
	char name[64];
	DirectX::SimpleMath::Vector3 localPosition;
	DirectX::SimpleMath::Quaternion localRotation;
	DirectX::SimpleMath::Vector3 localScale;
	DirectX::SimpleMath::Matrix localMatrix;
	DirectX::SimpleMath::Matrix globalInvertTPose;
	int32_t parent;
	uint32_t childrenIndexOffset;
	uint32_t childrenIndexCount;
	uint32_t meshIndexOffset;
	uint32_t meshIndexCount;
	uint32_t boneIndexOffset;
	uint32_t boneIndexCount;

	size_t childrenSize;
	size_t meshSize;
	size_t boneSize;
};

struct FlipMode {
	bool flipX = false;
};

class ModelNode
{
	friend class JsonImporter;
public:
	ModelNode() = default;

	ModelNode(const ModelNode& other)
		: m_Name(other.m_Name)
		, m_OriginalName(other.m_OriginalName)
		, m_BIsDynamic(other.m_BIsDynamic)
		, m_LocalMatrix(other.m_LocalMatrix)
		, m_LocalPosition(other.m_LocalPosition)
		, m_LocalRotation(other.m_LocalRotation)
		, m_LocalScale(other.m_LocalScale)
		, m_BoneIndexList(other.m_BoneIndexList) 
		, m_MeshIndexList(other.m_MeshIndexList) 
		, m_Model(nullptr)
		, m_Parent(nullptr)
	{
	}

	LogPtr<ModelNode> Clone() const {
		return MAKE_UNIQUE_LOG<ModelNode>(*this);
	}

	void init(Model* model, aiNode*);	
	void Destroy();

	void setName(const std::string& name);
	std::string& name() { return m_Name; }
	std::string& originalName() { return m_OriginalName; }
	void setIndexList(Model* model);

	void updateMatrix();
	/**
	 * @brief			子を再帰して、ローカル行列更新
	 * @param parent	親（
	 * @param boneList 
	 * @param mode 
	 */
	void updateChildMatrix(DirectX::SimpleMath::Matrix parent,
		const std::vector<Bone*>& boneList, const FlipMode* mode = nullptr);

	void setLocalMatrix(const DirectX::SimpleMath::Matrix&);
	DirectX::SimpleMath::Matrix& localMatrix() {
		return m_LocalMatrix;}
	DirectX::SimpleMath::Matrix calcWorldMatrix()const;
	const DirectX::SimpleMath::Matrix& worldMatrix()const { 
		return m_WorldMatrix; }

	void setLocalRotation(DirectX::SimpleMath::Quaternion quat) {
		m_LocalRotation = quat;}
	const DirectX::SimpleMath::Quaternion& localRotation() const {
		return m_LocalRotation;}	

	void setLocalPosition(DirectX::SimpleMath::Vector3 pos) {
		m_LocalPosition = pos;	}
	const DirectX::SimpleMath::Vector3& localPosition()const {
		return m_LocalPosition;	}

	void setLocalScale(DirectX::SimpleMath::Vector3 scale) {
		m_LocalScale = scale;	}
	const DirectX::SimpleMath::Vector3& localScale()const {
		return m_LocalScale;	}

	void addBoneIndex(int index) { m_BoneIndexList.push_back(index); }
	bool isBone() const { return m_BoneIndexList.size() != 0; }

	bool isMesh()const { return m_MeshIndexList.size() != 0; }

	void setDynamic(bool isDynamic) { m_BIsDynamic = isDynamic; }
	bool isDynamic() const { return m_BIsDynamic; }

	void setModel(Model* model) { m_Model = model; }

	void setParent(ModelNode* obj);
	ModelNode* parent()const;
	void addChild(ModelNode* obj);
	std::vector<ModelNode*>& children();

	void calcTPoseNode(DirectX::SimpleMath::Matrix parent);

	DirectX::SimpleMath::Matrix globlaInvertTPose()const {
		return m_GlobalInvertTPose;}

private:
	std::string m_Name;
	DirectX::SimpleMath::Vector3 m_LocalPosition;
	DirectX::SimpleMath::Quaternion m_LocalRotation;
	DirectX::SimpleMath::Vector3 m_LocalScale;
	ModelNode* m_Parent;
	std::vector<ModelNode*> m_Children;
	std::vector<int> m_MeshIndexList;
	std::vector<int> m_BoneIndexList;

	DirectX::SimpleMath::Matrix m_LocalMatrix;
	DirectX::SimpleMath::Matrix m_WorldMatrix;
	DirectX::SimpleMath::Matrix m_GlobalInvertTPose;

	std::string m_OriginalName;
	bool m_BIsDynamic = true;
	Model* m_Model;
public:
#ifdef _DEBUG
	static constexpr const char* m_ClassName = "ModelNode";
#endif
};

