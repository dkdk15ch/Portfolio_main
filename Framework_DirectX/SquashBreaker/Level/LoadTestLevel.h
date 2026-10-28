#pragma once
#include "Level/Level.h"

class LoadTestLevel : public Level
{
public:
	LoadTestLevel(std::string jsonKey);
	virtual ~LoadTestLevel();

	void update(const uint32_t frameIndex, double deltaTime) override;
	virtual void term()override;

private:
};

