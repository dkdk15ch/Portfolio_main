#pragma once
#include "Level/Level.h"
#include <SimpleMath.h>

class ColliderManager;
class GameBase;

class TestScene : public Level
{
public:
	TestScene(std::string jsonKey);
	virtual ~TestScene();

	void init()override final;

	void update(const uint32_t frameIndex, double deltaTime) override;
	virtual void term()override;

private:
	ColliderManager* m_ColliderMgr;
	GameBase* m_Player;
};

