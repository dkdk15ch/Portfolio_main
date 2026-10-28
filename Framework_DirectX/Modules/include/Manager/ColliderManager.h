#pragma once
#include <unordered_map>
#include <string>
#include <SimpleMath.h>
#include "Tag.h"
#include "Util/MappedVector.h"

class ColliderCompBase;
struct HitInfo;

struct RayDesc {
	RayDesc()=default;

	RayDesc(
	float maxDist,
	Tag<CollisionTag> target,
	float r = 0)
		: start{}
		, direction{}
		, maxDistance{maxDist}
		, radius(r)
		, targetTag(target)
	{}
	
	DirectX::SimpleMath::Vector3 start;
	DirectX::SimpleMath::Vector3 direction;
	float maxDistance;
	float radius; //	球状の厚さを考慮する場合(球を飛ばす時など)
	Tag<CollisionTag> targetTag;
};

class ColliderManager
{
public:
	static ColliderManager* m_Instance;

	ColliderManager();
	void term();

	void update(uint32_t frameIndex);
	void add(const std::string& key, ColliderCompBase* adder);
	void remove(const std::string& key);
	void clear();

	/**
	 * @brief		rayTagに含まれるコライダー全てにレイが当たってるか判定
	 * @param ray				レイ（呼び出しで保持or作成)
	 * @param [output] info			ヒット情報(最も近い対象）
	 * @param frameIndex		
	 * @param rayTarget　　		対象にするもの全てタグ追加して渡す
	 * @return					ヒット(true)
	 */
	bool raycast(
		const RayDesc& ray,
		HitInfo& info,
		uint32_t frameIndex,
		bool bIsSkippedOnHit = false
	);

private:
	void allCheck(uint32_t frameIndex);

	MappedVector<ColliderCompBase> m_ColliderList;
};

