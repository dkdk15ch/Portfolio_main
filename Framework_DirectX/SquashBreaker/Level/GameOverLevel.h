#pragma once
#include "Level/Level.h"

class GameOverLevel : public Level {
public:
	GameOverLevel();
	~GameOverLevel();

	void init() override final;
	void update(const uint32_t frameIndex, double deltaTime) override;
	virtual void term()override;

private:


};