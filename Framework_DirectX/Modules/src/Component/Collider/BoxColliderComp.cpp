#include "Component/Collider/BoxColliderComp.h"
#include "GameObject/GameBase.h"
#include "Component/Render/RenderCompBase.h"
#include "ConstantBuffer.h"
#include "Util/CollisionMath.h"
#include "Manager/ColliderManager.h"
#include "GameObject/PrimitiveMesh.h"
#include <DirectXColors.h>
#include "Component/MovementComp.h"
#include <iostream>

#ifdef _DEBUG
#include "Manager/LevelManager.h"
#endif // _DEBUG


void BoundingBox::setMinMax(
	GameBase* owner,
	DirectX::SimpleMath::Vector3 scale,
	const DirectX::SimpleMath::Matrix& offset,
	int InstanceIndex)
{
	//const auto& data = owner->instanceDataList()[InstanceIndex];
	//const auto& world = offset * data.m_Transform->worldMatrix();

	////	中心座標を取得
	//DirectX::SimpleMath::Vector3 center(world._41, world._42, world._43);
	//center.x = world._41;
	//center.y = world._42;
	//center.z = world._43;
	//auto Min = PrimitiveMesh::cubeMin();
	//min.x = center.x + (Min.x * scale.x);
	//min.y = center.y + (Min.y * scale.y);
	//min.z = center.z + (Min.z * scale.z);
	//auto Max = PrimitiveMesh::cubeMax();
	//max.x = center.x + (Max.x * scale.x);
	//max.y = center.y + (Max.y * scale.y);
	//max.z = center.z + (Max.z * scale.z);

	using namespace DirectX::SimpleMath;
	const auto& data = owner->instanceDataList()[InstanceIndex];
	const auto& world = offset * data.m_Transform->worldMatrix();

	Vector3 localMin = PrimitiveMesh::cubeMin();
	Vector3 localMax = PrimitiveMesh::cubeMax();

	// ローカル8頂点
	Vector3 localCorners[8] = {
		Vector3(localMin.x, localMin.y, localMin.z),
		Vector3(localMax.x, localMin.y, localMin.z),
		Vector3(localMin.x, localMax.y, localMin.z),
		Vector3(localMax.x, localMax.y, localMin.z),
		Vector3(localMin.x, localMin.y, localMax.z),
		Vector3(localMax.x, localMin.y, localMax.z),
		Vector3(localMin.x, localMax.y, localMax.z),
		Vector3(localMax.x, localMax.y, localMax.z),
	};

	min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	max = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (int i = 0; i < 8; ++i) {
		Vector3 scaled = localCorners[i] * scale;
		Vector3 wc = Vector3::Transform(localCorners[i], world);
		min = Vector3::Min(min, wc);
		max = Vector3::Max(max, wc);
	}

}

BoxColliderComp::BoxColliderComp(GameBase* owner, ColliderCompBase::Shape shape)
	: ColliderCompBase(owner)
	, m_Shape(shape)
{


	if (m_Shape == ColliderCompBase::Shape::AABB) {
		owner->transform()->lock(TransformLock::RotAll);
	}

	m_BBList.resize(owner->instanceNum());
	for(int i = 0; i < m_BBList.size(); i++){
		m_BBList[i].setMinMax(owner, owner->transform()->scale(), m_Offset, i);
	}
}

void BoxColliderComp::swapBB(uint32_t index1, uint32_t index2)
{
	std::swap(m_BBList[index1], m_BBList[index2]);
}

bool BoxColliderComp::detectAligned(
	ColliderCompBase& other, 
	uint32_t frameIndex, 
	HitInfo& outputInfo,
	int instanceIndex)
{
	const auto& shape = other.shape();

	using type = ColliderCompBase::Shape;
	using namespace CollisionMath;
	auto& o = owner();
	//auto& t2 = *other.owner().transform();
	auto& t2 = *other.owner().instanceDataList()[instanceIndex].m_Transform;
	
	m_BBList[0].setMinMax(&o, o.transform()->worldScale(), m_Offset, 0);

	switch (shape)
	{
	case type::AABB:
	{
		auto boxCollider = static_cast<BoxColliderComp*>(&other);
		auto otherScale = other.owner().transform()->worldScale();
		boxCollider->m_BBList[instanceIndex].setMinMax(&other.owner(), otherScale, 
			other.offset(), instanceIndex);
		if (IntersectAABBs(m_BBList[0], boxCollider->m_BBList[instanceIndex],outputInfo)) {
			outputInfo.instanceIndex = instanceIndex; 
			return true;
		}
	}
	case type::OBB:
	{
		break;
	}
	case type::Sphere:
	{
		auto& t = *owner().transform();
		auto& sphereT = *other.owner().transform();
		auto sphereWorld = other.offset() * sphereT.worldMatrix();
		const float radius = sphereT.worldScaleX();
		if (IntersectAABBSphere(m_BBList[0], radius, sphereWorld)) {
			outputInfo.instanceIndex = instanceIndex;
			outputInfo.hitPos = CalcClosestPointOnAABB(m_BBList[0], sphereT.worldPos());
			return true;
		}
		return false;
	}
	}

	return false;
}

bool BoxColliderComp::intersectRay(const RayDesc& ray, HitInfo& info, uint32_t instanceIndex, uint32_t frameIndex)
{
	using namespace CollisionMath;
	auto& o = owner();
	m_BBList[instanceIndex].setMinMax(&o, o.transform()->scale(), m_Offset, instanceIndex);
	auto bb =  MinMax::toMinMax(m_BBList[instanceIndex]);

	const auto pos = Point(ray.start);
	float hitTime = 0;
	float t_max = FLT_MAX;
	const auto dir = Point(ray.direction);

	for (int i = 0; i < 3; ++i) {
		const float p = pos.get(i);
		const float d = dir.get(i);
		const float min = bb.getMin(i) - ray.radius;
		const float max = bb.getMax(i) + ray.radius;

		if (std::abs(d) < FLT_EPSILON) {
			if (p < min || p > max) return false;	//	交わらない場合終了
		}
		else {
			//	壁との交差時間
			const float inv_d = 1.0f / d;
			float t1 = (min - p) * inv_d;
			float t2 = (max - p) * inv_d;
			if (t1 > t2) {//　手前と奥逆なら入れ替え
				std::swap(t1, t2);
			}

			if (t1 > hitTime) {
				hitTime = t1;
			}
			if (t2 < t_max) {
				t_max = t2;
			}
			//	スラブ交差点チェック
			if (hitTime >= t_max) {
				return false;
			}
		}
	}
	if (hitTime < 0.0f || hitTime > ray.maxDistance) {
		return false;
	}

	const auto surfaceHit = ray.start + hitTime * ray.direction;
	info.face = hitFace(surfaceHit, frameIndex, instanceIndex);
	auto normal = faceToNormal(info.face);
	if (normal != DirectX::SimpleMath::Vector3::Zero) {
		info.normal = normal;
	}
	info.hitPos = surfaceHit + normal * ray.radius;
	info.other = this;
	info.instanceIndex = instanceIndex;
	info.distance = hitTime;
	return true;
}

CollisionMath::Face BoxColliderComp::hitFace(const DirectX::SimpleMath::Vector3& pos, uint32_t frameIndex, int InstanceIndex)
{
	int face = 0;
	auto point = CollisionMath::Point(pos);
	float maxDistance = -FLT_MAX;

	auto box = CollisionMath::MinMax::toMinMax(m_BBList[InstanceIndex]);
	for (int i = 0; i < 3; i++) {
		//	各軸で最小値以下or最大値以上　で差を求める
		float deltaMin = box.getMin(i) - point.get(i);
		if (deltaMin > maxDistance) {
			maxDistance = deltaMin;
			face = i * 2;
		}
		float deltaMax = point.get(i) - box.getMax(i);
		if (deltaMax > maxDistance) {
			maxDistance = deltaMax;
			face = i * 2 + 1;
		}
	}
	return static_cast<CollisionMath::Face>(face);
}

void BoxColliderComp::show(DirectX::XMVECTOR color)
{
#ifdef _DEBUG
	using namespace DirectX::SimpleMath;
	 
	auto finalColor = color;
	if (DirectX::XMVector4Equal(finalColor, DirectX::Colors::Magenta)) {
		finalColor = DirectX::Colors::Red;
	}

	auto obj = MAKE_UNIQUE_LOG<GameBase>(owner().name() + "_BoxCollider");
	obj->setParent(&owner());
	obj->addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(obj.get(), GPSOkey::WIRE));
	auto debugger = static_cast<RenderCompBase*>(obj->getComponent(CompType::RenderComp));
	debugger->setCubeMesh();
	obj->setColor(finalColor);
	
	Vector3 pos, scale;
	Quaternion rot;
	m_Offset.Decompose(scale, rot, pos);
	obj->initStaticTransform(
		pos, 
		DirectX::SimpleMath::Vector3(0.f,0.f,0.f),
		scale
	);

	auto mgr = LevelManager::m_Instance;
	mgr->requestAddObject(std::move(obj));
#endif // _DEBUG
}
