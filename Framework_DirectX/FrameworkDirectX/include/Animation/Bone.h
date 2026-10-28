#pragma once
#include <string>
#include <SimpleMath.h>
#include "ModelNode.h"

struct BoneBlob {
	char name[64];
	char nodeName[64];
	//int32_t targetNodeIndex;
	int32_t index;
	int32_t nodeIndex;
	DirectX::SimpleMath::Matrix worldMat;
	DirectX::SimpleMath::Matrix invMat;
};

class Bone
{
	friend JsonImporter;
public:
	Bone() = default;
	Bone(const BoneBlob& blob)
		: m_Name(blob.name)
		, m_TargetNodeName(blob.nodeName)
		, m_WorldMatrix(blob.worldMat)
		, m_InverseMatrix(blob.invMat)
		, m_Index(blob.index)
		, m_NodeIndex(blob.nodeIndex)
	{
	}

	// clone
	Bone(const Bone& other)
		: m_Name(other.m_Name)
		, m_TargetNodeName(other.m_TargetNodeName)
		, m_WorldMatrix(other.m_WorldMatrix)
		, m_InverseMatrix(other.m_InverseMatrix)
		, m_Index(other.m_Index)
		, m_NodeIndex(other.m_NodeIndex)
	{
	}
	LogPtr<Bone> Clone() const {
		return MAKE_UNIQUE_LOG<Bone>(*this);
	}

	void setNodeName(const std::string& name) { m_TargetNodeName = name; }
	std::string& nodeName() { return m_TargetNodeName; }
	void setName(const std::string& name) { m_Name = name; };
	std::string& name() { return m_Name; };

	void setIndex(int index) { m_Index = index; };
	int index() { return m_Index; };
	void setNodeIndex(int index) { m_NodeIndex = index; };
	int nodeIndex() { return m_NodeIndex; };

	void setWorldMatrix(const DirectX::SimpleMath::Matrix& matrix) 
	{ m_WorldMatrix = matrix;}
	DirectX::SimpleMath::Matrix& worldMatrix() 
	{ return m_WorldMatrix; }
	void setInverseMatrix(const DirectX::SimpleMath::Matrix& matrix) 
	{ m_InverseMatrix = matrix;}
	DirectX::SimpleMath::Matrix& inverseMatrix()
	{ return m_InverseMatrix; }


private:
	std::string m_Name;
	std::string m_TargetNodeName;
	DirectX::SimpleMath::Matrix m_WorldMatrix;
	DirectX::SimpleMath::Matrix m_InverseMatrix;
	int m_Index = -1;
	int m_NodeIndex = -1;

public:
#ifdef _DEBUG
	static constexpr const char* m_ClassName = "Bone";
#endif
};

