#include "GameObject/PrimitiveMesh.h"
#include "ConstantBuffer.h"
#include "Util/Loader.h"
#include "Mesh.h"
#include "Material.h"
#include "Component/Render/RootParam.h"
#include "Logger.h"

#pragma region PrimitiveMesh

PrimitiveMesh* PrimitiveMesh::m_Instance = nullptr;


PrimitiveMesh::PrimitiveMesh(ID3D12Device* device, DescriptorPool* pool) :
	m_CubeMin(0.0f, 0.0f, 0.0f),
	m_CubeMax(0.0f, 0.0f, 0.0f),
	m_SphereRadius(0.0f)
{
	if (!m_Instance) {
		m_Instance = this;

		//	プリミティブ形状モデルのロード
		m_PSphere = std::move(Loader::meshOnly(device, pool, L"res/Primitive/Sphere.obj"));
		assert(m_PSphere && "PrimitiveMesh::PrimitiveMesh() m_pSphere");
		auto bounding = Loader::boudingSize(pool, L"res/Primitive/Sphere.obj",false);
		m_SphereRadius = bounding.radius;

		m_PCube = std::move(Loader::meshOnly(device, pool, L"res/Primitive/Cube.obj"));
		assert(m_PCube && "PrimitiveMesh::PrimitiveMesh() m_pCube");
		bounding = Loader::boudingSize(pool, L"res/Primitive/Cube.obj",true);
		m_CubeMin = bounding.minVertex;
		m_CubeMax = bounding.maxVertex;

		m_PPlane = std::move( Loader::meshOnly(device, pool, L"res/Primitive/Plane.obj"));
		assert(m_PPlane && "PrimitiveMesh::PrimitiveMesh() m_pPlane");

		m_PCapsule = std::move(Loader::meshOnly(device, pool, L"res/Primitive/Capsule.obj"));
		assert(m_PCapsule && "PrimitiveMesh::PrimitiveMesh() m_pCapsule");
	}
}


PrimitiveMesh::~PrimitiveMesh() {
}


void PrimitiveMesh::term() {
	m_PCube.reset();
	m_PSphere.reset();
	m_PCapsule.reset();
	m_PPlane.reset();
	m_Instance = nullptr;
}


Mesh* PrimitiveMesh::cubeMesh()
{
	assert(m_Instance && "PrimitiveMesh::cubeMesh");
	return m_Instance->m_PCube.get();
}


Mesh* PrimitiveMesh::sphereMesh()
{
	assert(m_Instance && "PrimitiveMesh::sphereMesh");
	return m_Instance->m_PSphere.get();
}


Mesh* PrimitiveMesh::planeMesh()
{
	assert(m_Instance && "PrimitiveMesh::planeMesh");
	return m_Instance->m_PPlane.get();
}


Mesh* PrimitiveMesh::capsuleMesh()
{
	assert(m_Instance && "PrimitiveMesh::capsuleMesh");
	return m_Instance->m_PCapsule.get();
}


Mesh* PrimitiveMesh::getMesh(Type type)
{
	switch (type)
	{
	case PrimitiveMesh::Type::CUBE:
		return cubeMesh();
	case PrimitiveMesh::Type::SPHERE:
		return sphereMesh();
	case PrimitiveMesh::Type::PLANE:
		return planeMesh();
	case PrimitiveMesh::Type::CAPSULE:
		return capsuleMesh();
	default:
	{
		ELOG("Error Primitive::getMesh : type missmatch");
		return nullptr;
	}
	}
}


//	getter
DirectX::SimpleMath::Vector3 PrimitiveMesh::cubeMin()
{
	assert(m_Instance && L"Primitive::cubeMin");

	return m_Instance->m_CubeMin;
}


DirectX::SimpleMath::Vector3 PrimitiveMesh::cubeMax()
{
	assert(m_Instance && L"Primitive::cubeMax");

	return m_Instance->m_CubeMax;
}


float PrimitiveMesh::sphereRadius()
{
	assert(m_Instance && L"Primitive::sphereRadius");

	return m_Instance->m_SphereRadius;
}
#pragma endregion


