#pragma once
#include "Component/MovementComp.h"
#include "Component/ComponentBase.h"
#include "Delegate.h"

class BallParam;

class BallMovementComp : public MovementComp
{
public:
	BallMovementComp(GameBase* owner);
	virtual ~BallMovementComp();

	void init() override;
	void update(uint32_t frameIndex, double deltaTime) override final;

	Delegate::Singlecast<void(void)> OnLostBall;
	Delegate::Singlecast<void(void)> OnPinch;
	Delegate::Singlecast<void(void)> OnObstacleHit;


	/**
	* @brief		targetの場所へ 初速V0で飛ばす。
	* @param target	目標座標(※Dirではない）
	* @param V0		初速
	*/
	void launchToTarget(DirectX::SimpleMath::Vector3 target, float force);
	void hitBlock(int face, DirectX::SimpleMath::Vector3 hitPos);

	bool isBounded()const { return m_BIsBounded; }
	void bound(){ m_BIsBounded = true; 	}

	DirectX::SimpleMath::Vector3 calcFuturePos(float targetPosZ, double deltaTime);
private:
	bool m_BIsBounded;
	bool m_BIsLaunching;
	double m_ReseInterval;

	void updatePhysics(double deltatime);
	void hitWall(int face);
	void resolveWallCollision();
	void updateVisualScale();
};

