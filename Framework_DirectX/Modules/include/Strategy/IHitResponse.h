#pragma once
#include "Tag.h"

class GameBase;
struct HitInfo;
class IHitResponse;

class IHitResponseFactory {
public:
    IHitResponseFactory() = default;
    ~IHitResponseFactory() = default;
    virtual IHitResponse* create(CollisionTag otherLayer) const = 0;
};

class IHitResponse{
public:
    IHitResponse() = default;
    virtual ~IHitResponse() = default;
    virtual void onHit(GameBase& owner, const HitInfo& info) = 0;
};


