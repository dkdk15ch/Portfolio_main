#define NOMINMAX
#include "Util/CollisionMath.h"
#include "ConstantBuffer.h"
#include "Logger.h"
#include "Component/TransformComp.h"
#include "Component/Collider/BoxColliderComp.h"
#include "Component/Collider/ColliderCompBase.h"
#include <algorithm>

namespace CollisionMath {
	using namespace DirectX::SimpleMath;

MinMax MinMax::toMinMax(const::BoundingBox& bb)
{
	return MinMax{
		bb.min.x, bb.max.x, bb.min.y, bb.max.y, bb.min.z, bb.max.z };
}


Face checkHitFace(
	DirectX::SimpleMath::Vector3& pos,
	const ::BoundingBox& BB,
	uint32_t frameIndex, 
	int InstanceIndex)
{
	int face = 0;
	auto point = Point(pos);
	float maxDistance = -FLT_MAX;

	auto box = MinMax::toMinMax(BB);
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
	auto result = static_cast<Face>(face);
	if (result == Face::BACK) {
		printf("");
	}

	return result;

	return static_cast<Face>(face);
}

bool IntersectRayAABB(
	const DirectX::SimpleMath::Vector3& rayStart, 
	const DirectX::SimpleMath::Vector3& rayDir,
	const ::BoundingBox& BB,
	float& outHitTime, 
	DirectX::SimpleMath::Vector3* outColPos, 
	float sphereRadius)
{
	auto box = MinMax::toMinMax(BB);
	outHitTime = 0.0f;
	float t_max = FLT_MAX;
	auto dir = Point(rayDir);
	auto pos = Point(rayStart);

	for (int i = 0; i < 3; ++i) {
		const float p = pos.get(i);
		const float d = dir.get(i);
		const float min = box.getMin(i) - sphereRadius;
		const float max = box.getMax(i) + sphereRadius;

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

			if (t1 > outHitTime) {
				outHitTime = t1;
			}
			if (t2 < t_max) {
				t_max = t2;
			}
			//	スラブ交差点チェック
			if (outHitTime >= t_max) {
				return false;
			}
		}
	}
	if (outHitTime < 0.0f || outHitTime > 1.0f) return false;
	if (outColPos) {
		*outColPos = rayStart + outHitTime * rayDir;
	}
	return true;
}

float SqDistPointAABB(
	const ::BoundingBox& BB,
	const DirectX::SimpleMath::Vector3& p)
{
	float sqLen = 0.0f;
	Point pt(p);
	MinMax box = MinMax::toMinMax(BB); 

	for (int i = 0; i < 3; ++i) {
		const float p = pt.get(i);
		const float minVal = box.getMin(i);
		const float maxVal = box.getMax(i);

		if (p < minVal) {
			sqLen += (p - minVal) * (p - minVal);
		}
		if (p > maxVal) {
			sqLen += (p - maxVal) * (p - maxVal);
		}
	}
	return sqLen;
}

float SqDistance(const DirectX::SimpleMath::Vector3& p1, const DirectX::SimpleMath::Vector3& p2)
{
	// 距離の2乗を取得
	return Vector3::DistanceSquared(p1, p2);
}

DirectX::SimpleMath::Vector3 NormalSphere(const DirectX::SimpleMath::Vector3& p1, const DirectX::SimpleMath::Vector3& p2)
{
	auto result = (p2 - p1);
	result.Normalize();
	return result;
}


bool IntersectSpheres(DirectX::SimpleMath::Matrix m1, DirectX::SimpleMath::Matrix m2)
{
	Vector3	s1;
	Quaternion r1;
	Vector3	t1;
	m1.Decompose(s1, r1, t1);
	Vector3	s2;
	Quaternion r2;
	Vector3	t2;
	m2.Decompose(s2, r2, t2);
	float sqRadius = s1.x + s2.x;
	sqRadius *= sqRadius;
	float dis = SqDistance(t1, t2);

	bool result = dis <= sqRadius;
	if (result) {
		printf("Hit\n");
	}

	return dis <= sqRadius;
}



bool IntersectAABBSphere(const ::BoundingBox& BB, TransformComp& SphereT)
{
	const auto radius = SphereT.worldScaleX();		
	float dis = SqDistPointAABB(BB, SphereT.worldPos());
		

	if (dis <= radius * radius) {
		return true;
	}
	return false;
}

bool IntersectAABBSphere(const ::BoundingBox& BB, float radius, const DirectX::SimpleMath::Matrix& sphereMat)
{
	auto sPos = sphereMat.Translation();
	float dis = SqDistPointAABB(BB, sPos);

	if (dis <= radius * radius) {
		return true;
	}
	return false;
}

DirectX::SimpleMath::Vector3 CalcClosestPointOnAABB(const ::BoundingBox& BB, DirectX::SimpleMath::Vector3 spherePos)
{
	// 1. AABBの最小値・最大値を算出
	MinMax data = MinMax::toMinMax(BB);
	// 2. 球の中心（sPos）をAABBの範囲内にクランプする
	Vector3 closest;
	closest.x = std::clamp(spherePos.x, data.minX, data.maxX);
	closest.y = std::clamp(spherePos.y, data.minY, data.maxY);
	closest.z = std::clamp(spherePos.z, data.minZ, data.maxZ);

	return closest;
}

DirectX::SimpleMath::Vector3 calcHitNormalPointOnAABB(const ::BoundingBox& BB, 
	const DirectX::SimpleMath::Vector3& closest)
{
	MinMax data = MinMax::toMinMax(BB);
	DirectX::SimpleMath::Vector3 normal(0.0f, 0.0f, 0.0f);
	const float EPS = 1e-4f; 

	if (std::abs(closest.x - data.maxX) < EPS) {normal.x = 1.0f;}
	else if (std::abs(closest.x - data.minX) < EPS) {normal.x = -1.0f;}
	
	if (std::abs(closest.y - data.maxY) < EPS) {normal.y = 1.0f;}
	else if (std::abs(closest.y - data.minY) < EPS) {normal.y = -1.0f;}

	if (std::abs(closest.z - data.maxZ) < EPS) {normal.z = 1.0f;}
	else if (std::abs(closest.z - data.minZ) < EPS) {normal.z = -1.0f;}

	normal.Normalize();
	return normal;
}

DirectX::SimpleMath::Vector3 calcHitNormalPointOnAABB(const::BoundingBox& BB, const DirectX::SimpleMath::Vector3& center, const DirectX::SimpleMath::Vector3& closest)
{
	DirectX::SimpleMath::Vector3 v = center - closest;
	float lenSq = v.LengthSquared();

	if (lenSq < 1e-6f)
	{
		MinMax data = MinMax::toMinMax(BB);
		float dmin	= center.x - data.minX;
		float dmaxX = data.maxX - center.x;
		float dminY = center.y - data.minY;
		float dmaxY = data.maxY - center.y;
		float dminZ = center.z - data.minZ;
		float dmaxZ = data.maxZ - center.z;
		if (dmin > dmaxX) { dmin = dmaxX; }
		if (dmin > dminY) { dmin = dmaxX; }
		if (dmin > dmaxY) { dmin = dmaxX; }
		if (dmin > dminZ) { dmin = dmaxX; }
		if (dmin > dmaxZ) { dmin = dmaxX; }

		return DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	}

	v.Normalize();
	return v;
}

bool IntersectAABBs(const ::BoundingBox& BB1, const ::BoundingBox& BB2, HitInfo& info)
{
	auto b1 = MinMax::toMinMax(BB1);
	auto b2 = MinMax::toMinMax(BB2);
	float minOverlap = FLT_MAX;
	uint32_t minAxis = 4;
	float sign = 0.f;

	for (int i = 0; i < 3; ++i) {
		float min1 = b1.getMin(i), max1 = b1.getMax(i);
		float min2 = b2.getMin(i), max2 = b2.getMax(i);

		if (min1 > max2 || max1 < min2) {
			return false;
		}

		float overlap = std::min(max1, max2) - std::max(min1, min2);
		if (overlap < minOverlap) {
			minOverlap = overlap;
			minAxis = i;

			float c1 = (min1 + max1) * 0.5f;
			float c2 = (min2 + max2) * 0.5f;
			sign = (c1 > c2) ? 1.0f : -1.0f;
		}
	}

	info.overlapLength = minOverlap;

	if (minAxis == 0) 
	{
		info.normal = Vector3(sign, 0.0f, 0.0f);
	}
	else if (minAxis == 1) 
	{
		info.normal = Vector3(0.0f, sign, 0.0f);
	}
	else if (minAxis == 2) 
	{
		info.normal = Vector3(0.0f, 0.0f, sign);
	}

	return true;
}

DirectX::SimpleMath::Vector3 CalcClosestPointOnAABBvsAABB(const BoundingBox& BB1, const BoundingBox& BB2)
{
	return DirectX::SimpleMath::Vector3();
}

}// namespace CollisionMath
