#pragma once
#include "GameObject/GameBase.h"
#include "GameObject/UniqueName.h"
#include "PointerHelper.h"
#include "GameObject/Param/BallParam.h"


struct RayDesc;
class BallMovementComp;
class RenderCompBase;
class SphereColliderComp;
class MovementComp;
class IHitResponseFactory;
class IHitResponse;


class Ball : public GameBase, public UniqueName
{
public:
	Ball(std::string name = "Ball");
	virtual ~Ball();

	void term() override;
	void enable() override final;
	void disable() override final;

	BallMovementComp* mover(){ return m_Mover; }
	const IHitResponseFactory* hitResFactory()const {
		if (!m_HitResponseFactory) { return nullptr; }
		return m_HitResponseFactory.get();
	}

	void setHitResponse(LogPtr<IHitResponseFactory> factory) 
	{ m_HitResponseFactory = std::move(factory); }

	RayDesc* rayDesc()const { return m_BlockCheckRay.get(); }
	void checkRayToBlock();
	void ignoreRayToBlock();

protected:
	virtual void implupdate(uint32_t frameIndex, double deltaTime) override;
	virtual void implOnHit(const HitInfo& info) override;

	BallMovementComp* m_Mover;
	LogPtr<IHitResponseFactory> m_HitResponseFactory;
private:

	void rayCastToBlock(
		uint32_t frameIndex, double deltaTime);
	LogPtr<RayDesc> m_BlockCheckRay;
	SphereColliderComp* m_Collider;

	GameBase* m_Shadow;
	MovementComp* m_ShadowMover;
};



