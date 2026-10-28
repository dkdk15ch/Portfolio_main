#pragma once
#include "GameObject/ObjectParam.h"

class CollisionLayerParam : public ObjectParam
{
public:
	static const CollisionLayerParam* layerParam();

	uint32_t racketTarget() const { return RACKET_TARGET; }
	uint32_t playerTarget() const { return PLAYER_TARGET; }
	uint32_t ballTarget() const { return BALL_TARGET; }
	uint32_t enemyBallTarget() const { return ENEMY_BALL_TARGET; }
	uint32_t blockTarget() const { return BLOCK_TARGET; }
	uint32_t enemyTarget() const { return ENEMY_TARGET; }
	uint32_t itemTarget() const { return ITEM_TARGET; }
	uint32_t targetUIRayTarget() const { return TARGET_UI_RAY_TARGET; }
	uint32_t staticTarget() const { return STATIC_TARGET; }

protected:
	virtual void moveFromJson(const std::string& actorKey, const std::string& sceneKey = "") override final;

private:
	uint32_t RACKET_TARGET;
	uint32_t PLAYER_TARGET;
	uint32_t BALL_TARGET;
	uint32_t ENEMY_BALL_TARGET;
	uint32_t BLOCK_TARGET;
	uint32_t ENEMY_TARGET;
	uint32_t ITEM_TARGET;
	uint32_t TARGET_UI_RAY_TARGET;
	uint32_t STATIC_TARGET;
};