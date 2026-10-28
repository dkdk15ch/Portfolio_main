#pragma once
#include "Component/Collider/ColliderCompBase.h"
#include <utility>

namespace CollisionMath { struct BoundingBox; }
struct RayDesc;

struct BoundingBox {
	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;

	void setMinMax(
		//const CB::InstanceParamBuffer& instancePtr,
		GameBase* owner,
		DirectX::SimpleMath::Vector3 scale,
		const DirectX::SimpleMath::Matrix& offset,
		int InstanceIndex);

};

class BoxColliderComp : public ColliderCompBase
{
public:
	BoxColliderComp(GameBase* owner, ColliderCompBase::Shape shape);


	ColliderCompBase::Shape shape() const override final {
		return m_Shape;
	}

	void swapBB(uint32_t index1, uint32_t index2);
	const BoundingBox& BB(uint32_t index) { return m_BBList[index]; }

protected:
	bool detectAligned(
		ColliderCompBase& other,
		uint32_t frameIndex, 
		HitInfo& outputInfo,
		int InstanceIndex = 0) override final;

	bool intersectRay(
		const RayDesc& ray,
		HitInfo& info,
		uint32_t instIdx,
		uint32_t frameIndex) override final;

private:
	ColliderCompBase::Shape m_Shape; // コンストラクタでのみ変更
	std::vector<BoundingBox> m_BBList;
	
	

	CollisionMath::Face hitFace(const DirectX::SimpleMath::Vector3& pos, uint32_t frameIndex, int InstanceIndex);

public:
	void show(DirectX::XMVECTOR color = DirectX::Colors::Red) override final;
};

