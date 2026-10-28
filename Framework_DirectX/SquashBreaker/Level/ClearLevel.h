#pragma once
#include "Level/Level.h"

class ClearLevel : public Level {
public:
	ClearLevel();
	~ClearLevel();

	void init() override final;
	void update(const uint32_t frameIndex, double deltaTime) override;
	virtual void term()override;

private:


};