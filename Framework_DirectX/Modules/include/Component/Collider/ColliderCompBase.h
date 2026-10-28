#pragma once
#include "Component/ComponentBase.h"
#include "PointerHelper.h"
#include <SimpleMath.h>
#include <Tag.h>
#include "Util/CollisionMath.h"
#include <DirectXColors.h>

class GameBase;
class TransformComp;
class MovementComp;

namespace CB {
	struct InstanceParamBuffer;
}
struct RayDesc;


class ColliderCompBase;
struct HitInfo {
	HitInfo() = default;
	HitInfo(ColliderCompBase* checker,
		ColliderCompBase* hitCollider)
		: self(checker)
		, other(hitCollider)
		, hitPos(DirectX::SimpleMath::Vector3::Zero)
		, normal(DirectX::SimpleMath::Vector3::Zero)
		, face(CollisionMath::Face::None)
		, overlapLength( NAN)
		, distance{ NAN }
		, instanceIndex(0)
	{
	}

	ColliderCompBase* self;
	ColliderCompBase* other;
	DirectX::SimpleMath::Vector3 hitPos;
	DirectX::SimpleMath::Vector3 normal;
	CollisionMath::Face face;
	float overlapLength;
	float distance;
	uint32_t instanceIndex;

};

class ColliderCompBase : public ComponentBase
{
public:

	ColliderCompBase(GameBase* owner);
	virtual ~ColliderCompBase() = default;
	
	CompType compType() override final { return CompType::ColliderComp; }

	void enable() override final {
		m_SelfLayer = m_InitSelfLayer;
	};
	void disable() override final {
		m_SelfLayer = CollisionTag::disable;
	}

	//	検知（呼び出し用）
	bool detect(
		ColliderCompBase& other,
		uint32_t frameIndex,
		HitInfo& outputInfo,
		int InstanceIndex = 0);
	bool detectRay(
		const RayDesc& ray,
		HitInfo& info,
		uint32_t instIdx,
		uint32_t frameIndex
	);

	//	親オブジェクトの応答呼び出し
	void callOnHit(const HitInfo& other);

	enum class Shape {
		AABB,
		OBB,
		Sphere,
		Ray,
	};
	virtual Shape shape() const = 0;

	void setLayer(CollisionTag newTag) {
		m_SelfLayer = newTag;
		if (m_InitSelfLayer == CollisionTag::disable) {
			m_InitSelfLayer = newTag;
		}
	}
	void setInitLayer(CollisionTag newTag) {
		m_InitSelfLayer = newTag;
	}

	CollisionTag layer()const { return m_SelfLayer; }
	Tag<CollisionTag>& targetLayer() { return m_TargetLayers; }
	const Tag<CollisionTag>& targetLayer() const { return m_TargetLayers; }
	void setTargetLayer(uint32_t target) { m_TargetLayers.setTag(target); }

	const DirectX::SimpleMath::Matrix& offset()const {
		return m_Offset;
	}
	void setOffset(DirectX::SimpleMath::Vector3 pos,
		DirectX::SimpleMath::Vector3 rot,
		DirectX::SimpleMath::Vector3 scale);

protected:
	//	軸平行な当たり判定
	virtual bool detectAligned(
		ColliderCompBase& other,
		uint32_t frameIndex,
		HitInfo& outputInfo,
		int InstanceIndex = 0) = 0;

	//	軸と傾いた当たり判定
	//virtual bool detectOriented(const ColliderCompBase& other,
	//	uint32_t frameIndex, int InstanceIndex = 0) = 0;

	virtual bool intersectRay(
		const RayDesc& ray,
		HitInfo& info,
		uint32_t instIdx,
		uint32_t frameIndex) = 0;

	DirectX::SimpleMath::Matrix m_Offset;

private:
	CollisionTag m_SelfLayer;
	CollisionTag m_InitSelfLayer = CollisionTag::disable;
	Tag<CollisionTag> m_TargetLayers;

public:
	virtual void show(DirectX::XMVECTOR color = DirectX::Colors::Magenta) = 0;
};

