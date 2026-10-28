#pragma once
#include "ColliderCompBase.h"

struct RayDesc;

class SphereColliderComp : public ColliderCompBase
{
public:
    SphereColliderComp(GameBase* owner);

    ColliderCompBase::Shape shape() const override final {
        return ColliderCompBase::Shape::Sphere;
    }

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

public:
    void show(DirectX::XMVECTOR color = DirectX::Colors::Blue) override final;
};

