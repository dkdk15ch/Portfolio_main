#pragma once
#include "GameObject/ObjectParam.h"

class BallMovementComp;
class Ball;

class BallParam : public ObjectParam
{
	friend BallMovementComp;
	friend Ball;

public:
	//	physic
	double defaultTimeScale() const { return DEFAULT_TIME_SCALE; }
	float speedMin() const { return SPEED_MIN; }
	float gravity() const { return GRAVITY; }
	float ignoreGravityHeight() const { return IGNORE_GRAVITY_HEIGHT; }
	float corWall() const { return COR_WALL; }
	float corBlock() const { return COR_BLOCK; }
	float frontHitVerticalDamping() const { return FRONT_HIT_VERTICAL_DAMPING; }
	//	sound
	const std::string& sE_Bound()const { return SE_BOUND; }
	float volumeBound()const { return VOL_BOUND; }

private:
	//	override IActorData
	void moveFromJson(const std::string& actorKey, const std::string& sceneKey) override final;

	//	physic
	double DEFAULT_TIME_SCALE;
	float SPEED_MIN;
	float GRAVITY;
	float IGNORE_GRAVITY_HEIGHT;
	float COR_WALL;
	float COR_BLOCK;
	float FRONT_HIT_VERTICAL_DAMPING;
	//	sound
	std::string SE_BOUND;
	float VOL_BOUND;

};

