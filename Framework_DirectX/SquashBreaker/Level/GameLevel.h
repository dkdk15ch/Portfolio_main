#pragma once
#include "Level/Level.h"
#include <SimpleMath.h>

class ColliderManager;
class GameBase;

class GameLevel : public Level
{
public:
	GameLevel(std::string jsonKey);
	 ~GameLevel();

	 void init()override final;
	 void awake() override final;
	 void update(const uint32_t frameIndex, double deltaTime) override;
	 virtual void term()override;

private:
	void stopBall();

	ColliderManager* m_ColliderMgr;
	GameBase* m_Player;
};