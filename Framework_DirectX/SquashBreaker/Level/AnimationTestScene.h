#pragma once
#include "Level/Level.h"
#include <SimpleMath.h>
#include "GameObject/TestAnimation.h"


class ColliderManager;

class AnimationTestScene : public Level
{
public:
	AnimationTestScene(std::string jsonKey);
	virtual ~AnimationTestScene();

	void update(const uint32_t frameIndex, double deltaTime) override;
	virtual void term()override;

private:
	ColliderManager* m_ColliderMgr;
};

