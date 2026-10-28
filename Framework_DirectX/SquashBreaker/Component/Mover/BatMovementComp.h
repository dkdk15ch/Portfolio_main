#pragma once
#include "Component/MovementComp.h"
#include "Component/ComponentBase.h"

class GameBase;
class Animator;
class BatStateMachine;

class BatMovementComp : public MovementComp
{
	friend class IBatState;
public:
	BatMovementComp(GameBase* owner);
	~BatMovementComp() = default;
	
	
	void start()override final;
	void init()override final;
	
	void preUpdate(uint32_t frameIndex, double deltaTime) override final;
	void update(uint32_t frameIndex, double deltaTime) override final;
	void postUpdate(uint32_t frameIndex, double deltaTime) override final;

private:
	void setRandVelocity();

	LogPtr<BatStateMachine> m_StateMachine;

};
