#pragma once
#include "Component/MovementComp.h"
#include "Component/ComponentBase.h"

class DebugMoveComp : public MovementComp {
public:
	DebugMoveComp(GameBase* owner) :MovementComp(owner) {};
	~DebugMoveComp() = default;

	void update(uint32_t frameIndex, double deltaTime) override;
private:
};