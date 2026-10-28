#pragma once
#include <SimpleMath.h>
#include <unordered_map>

class ConstantBuffer;
class TransformComp;
struct HitInfo;
struct BoundingBox;

namespace CB {
	struct InstanceParamBuffer;
}

namespace CollisionMath {

enum Face {
	LEFT = 0,
	RIGHT = 1,
	BOTTOM = 2,
	TOP = 3,
	FRONT = 4,
	BACK = 5,
	None = 6,
};

inline std::unordered_map<Face, DirectX::SimpleMath::Vector3> faceToNormalMap = {
	{ Face::LEFT,   { -1.0f,  0.0f,  0.0f } }, // 左 (-X)
	{ Face::RIGHT,  {  1.0f,  0.0f,  0.0f } }, // 右 (+X)
	{ Face::BOTTOM, {  0.0f, -1.0f,  0.0f } }, // 下 (-Y)
	{ Face::TOP,    {  0.0f,  1.0f,  0.0f } }, // 上 (+Y)
	{ Face::BACK,   {  0.0f,  0.0f,  1.0f } }, // 奥 (-Z)
	{ Face::FRONT,  {  0.0f,  0.0f, -1.0f } }  // 手前 (+Z)
};

inline DirectX::SimpleMath::Vector3 faceToNormal(Face face) {
	auto it = faceToNormalMap.find(face);
	if (it == faceToNormalMap.end()) {
		return DirectX::SimpleMath::Vector3::Zero;
	}
	return it->second;
}

struct MinMax {
	float minX, maxX, minY, maxY, minZ, maxZ;
	float getMin(int i) const {
		switch (i)
		{
		case 0: { return minX; }
		case 1: { return minY; }
		case 2: { return minZ; }
		}
		assert(false);
		return 0.0f;
	}
	float getMax(int i) const {
		switch (i)
		{
		case 0: { return maxX; }
		case 1: { return maxY; }
		case 2: { return maxZ; }
		}
		assert(false);
		return 0.0f;
	}

	static MinMax toMinMax(const ::BoundingBox& bb);
};

struct Point {
	Point(DirectX::SimpleMath::Vector3 p) :x(p.x), y(p.y), z(p.z) {}

	float x, y, z;
	float get(int i) const {
		switch (i)
		{
		case 0: { return x; }
		case 1: { return y; }
		case 2: { return z; }
		}
		assert(false);
		return 0.0f;
	}
};


#pragma region HelperFunction

Face checkHitFace(
	DirectX::SimpleMath::Vector3& pos, 
	uint32_t frameIndex, 
	int InstanceIndex);


bool IntersectRayAABB(
	const DirectX::SimpleMath::Vector3& rayStart,
	const DirectX::SimpleMath::Vector3& rayDir,
	const BoundingBox& BB,
	float& outHitTime,
	DirectX::SimpleMath::Vector3* outColPos,
	float sphereRadius = 0.0f);

// 点とAABBの最短距離（二乗）
float SqDistPointAABB(
	const BoundingBox& BB,
    const DirectX::SimpleMath::Vector3& point);

float SqDistance(
	const DirectX::SimpleMath::Vector3& p1,
	const DirectX::SimpleMath::Vector3& p2
	);

DirectX::SimpleMath::Vector3 NormalSphere(const DirectX::SimpleMath::Vector3& p1,
	const DirectX::SimpleMath::Vector3& p2);

bool IntersectSpheres(DirectX::SimpleMath::Matrix m1, DirectX::SimpleMath::Matrix m2 );

#pragma endregion

bool IntersectAABBSphere(const BoundingBox& BB, TransformComp& SphereT);
bool IntersectAABBSphere(const BoundingBox& BB, float sphereScale, const DirectX::SimpleMath::Matrix& sphereMat);
DirectX::SimpleMath::Vector3 CalcClosestPointOnAABB(const BoundingBox& BB, DirectX::SimpleMath::Vector3 spherePos);
DirectX::SimpleMath::Vector3 calcHitNormalPointOnAABB(const ::BoundingBox& BB, const DirectX::SimpleMath::Vector3& closest);
DirectX::SimpleMath::Vector3 calcHitNormalPointOnAABB(
	const ::BoundingBox& BB,
	const DirectX::SimpleMath::Vector3& center, 
	const DirectX::SimpleMath::Vector3& closest);


bool IntersectAABBs(const BoundingBox& BB1, const BoundingBox& BB2, HitInfo& info);
DirectX::SimpleMath::Vector3 CalcClosestPointOnAABBvsAABB(const BoundingBox& BB1, const BoundingBox& BB2);


};// namespace CollisionMath