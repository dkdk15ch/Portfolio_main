#pragma once
#include <string>
#include <vector>
#include "Vertex.h"
#include "ResMesh.h"
#include "Mesh.h"

class ModelMesh : public Mesh
{
public:
	virtual ~ModelMesh() = default;

	void setNodeName(const std::string& name) { m_TargetNodeName = name; }
	std::string& getNodeName() { return m_TargetNodeName; }
	void setName(const std::string& name) { m_Name = name; }
	std::string& name() { return m_Name; }
	VertexType type()const { return m_Type; }

	void setIndex(uint32_t index) { m_Index = index; }
	int index()const { return static_cast<int>(m_Index); }

	void setWorldMatrix(const DirectX::SimpleMath::Matrix& matrix) {
		m_WorldMatrix = matrix;
	}
	DirectX::SimpleMath::Matrix& worldMatrix()
	{
		return m_WorldMatrix;
	}

private:
	std::string m_Name;
	std::string m_TargetNodeName;
	uint32_t m_Index;
	VertexType m_Type = VertexType::Vertex_Skinned;
	DirectX::SimpleMath::Matrix m_WorldMatrix;

public:
#ifdef _DEBUG
	static constexpr const char* m_ClassName = "ModelMesh";
#endif
};

