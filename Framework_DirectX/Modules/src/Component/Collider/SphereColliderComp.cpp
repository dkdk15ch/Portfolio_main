#include "Component/Collider/SphereColliderComp.h"
#include "Component/Collider/BoxColliderComp.h"
#include "Component/Render/RenderCompBase.h"
#include "GameObject/GameBase.h"
#include "ConstantBuffer.h"
#include "Manager/ColliderManager.h"
#include <DirectXColors.h>

#ifdef _DEBUG
#include "Manager/LevelManager.h"
#endif // _DEBUG

SphereColliderComp::SphereColliderComp(GameBase* owner)
	: ColliderCompBase(owner)
{
}

bool SphereColliderComp::detectAligned(
	ColliderCompBase& other, 
	uint32_t frameIndex, 
	HitInfo& outputInfo,
	int InstanceIndex)
{
	using namespace CollisionMath;
	using type = ColliderCompBase::Shape;

	const auto& shape = other.shape();
	const auto& data = other.owner().instanceDataList()[InstanceIndex];
	const auto& t2 = data.m_Transform;
	const auto& world = t2->worldMatrix();
	auto pos = DirectX::SimpleMath::Vector3();
	pos.x = world._41;
	pos.y = world._42;
	pos.z = world._43;
	constexpr float slop = 0.1f;

	switch (shape)
	{
	case type::AABB:
	{
		auto& t = *owner().transform();
		auto sphereWorld = offset() * t.worldMatrix();
		const float radius = t.worldScaleX();
		auto BoxCollider = static_cast<BoxColliderComp*>(&other);
		const auto& BB = BoxCollider->BB(InstanceIndex);

		if (IntersectAABBSphere(BB, radius, sphereWorld)) {
			outputInfo.instanceIndex = InstanceIndex;
			auto sPos = sphereWorld.Translation();
			outputInfo.hitPos = CalcClosestPointOnAABB(BB, sPos);
			outputInfo.normal = calcHitNormalPointOnAABB(BB, outputInfo.hitPos);
			float len = (sPos - outputInfo.hitPos).Length();
			outputInfo.overlapLength = radius - len + slop;
			return true;
		}
		return false;
		break;
	}
	case type::OBB:
	{
		break;
	}
	case type::Sphere:
	{
		const auto& t1 = *owner().transform();
		auto mat1 = m_Offset * t1.worldMatrix();
		const auto& pos1 = mat1.Translation();
		const float r1 = DirectX::SimpleMath::Vector3(mat1._11, mat1._12, mat1._13).Length();

		auto mat2 = other.offset() * t2->worldMatrix();
		const auto& pos2 = mat2.Translation();
		const float r2 = DirectX::SimpleMath::Vector3(mat2._11, mat2._12, mat2._13).Length();

		if (IntersectSpheres(mat1, mat2)) {
			outputInfo.other = &other;
			outputInfo.hitPos = (pos1 + pos2)  * 0.5f;	
			outputInfo.instanceIndex = InstanceIndex;
			outputInfo.normal = NormalSphere(pos1, pos2);

			float len = (pos1 - pos2).Length();
			outputInfo.overlapLength = r1 + r2 - len + slop;
			return true;
		}
		return false;
	}
	}

	return false;
}

bool SphereColliderComp::intersectRay(const RayDesc& ray, HitInfo& info, uint32_t instIdx, uint32_t frameIndex)
{
	return false;
}

void SphereColliderComp::show(DirectX::XMVECTOR color)
{
#ifdef _DEBUG
	using namespace DirectX::SimpleMath;

	auto finalColor = color;
	if (DirectX::XMVector4Equal(finalColor, DirectX::Colors::Magenta)) {
		finalColor = DirectX::Colors::Blue;
	}
	auto obj = MAKE_UNIQUE_LOG<GameBase>(owner().name() + "_SphereCollider");
	obj->setParent(&owner());
	obj->addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(obj.get(), GPSOkey::WIRE));
	auto debugger = static_cast<RenderCompBase*>(obj->getComponent(CompType::RenderComp));
	debugger->setSphereMesh();
	obj->setColor(finalColor);
	Vector3 pos, scale;
	Quaternion rot;
	m_Offset.Decompose(scale, rot, pos);
	obj->initStaticTransform(
		pos,
		DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f),
		scale
	);


	auto mgr = LevelManager::m_Instance;
	mgr->requestAddObject(std::move(obj));
#endif // _DEBUG
}

