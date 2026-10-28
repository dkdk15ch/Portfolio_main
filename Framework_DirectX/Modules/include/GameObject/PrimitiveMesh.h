#pragma once
#include <memory>
#include "Mesh.h"

struct ID3D12Device;
class DescriptorPool;
namespace CB { enum class WireColor; }


class PrimitiveMesh
{
public:
	static PrimitiveMesh* m_Instance;

	enum class Type {
		CUBE,
		SPHERE,
		PLANE,
		CAPSULE,
	};

	PrimitiveMesh(ID3D12Device* device, DescriptorPool* pool);
	~PrimitiveMesh();
	void term();

	//	プリミティブ形状
	std::unique_ptr<Mesh> m_PCube;
	std::unique_ptr<Mesh> m_PSphere;
	std::unique_ptr<Mesh> m_PPlane;
	std::unique_ptr<Mesh> m_PCapsule;
	static Mesh* cubeMesh();
	static Mesh* sphereMesh();
	static Mesh* planeMesh();
	static Mesh* capsuleMesh();
	static Mesh* getMesh(Type type);

	//	立方体の最小・最大座標
	static DirectX::SimpleMath::Vector3 cubeMin();
	static DirectX::SimpleMath::Vector3 cubeMax();
	static float sphereRadius();	//	球半径

private:
	DirectX::SimpleMath::Vector3 m_CubeMin;
	DirectX::SimpleMath::Vector3 m_CubeMax;
	float m_SphereRadius;
};

