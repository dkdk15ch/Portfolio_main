#include "Component/Mover/BatMovementComp.h"
#include "GameObject/State/BatState.h"
#include "GameObject/Enemy/Bat.h"
#include "GameObject/Param/BatParam.h"
#include "GameObject/ObjectParam.h"
#include "MyRand.h"

BatMovementComp::BatMovementComp(GameBase* owner)
	: MovementComp(owner)
	, m_StateMachine(MAKE_UNIQUE_LOG<BatStateMachine>(owner, this))
{
	setLimitResponse(MAKE_UNIQUE_LOG<LimitHitResponse::Reflection>(this));
	m_Param = owner->param();

}

void BatMovementComp::start()
{
	m_StateMachine->init();
}

void BatMovementComp::init()
{

}

void BatMovementComp::preUpdate(uint32_t frameIndex, double deltaTime)
{
}

void BatMovementComp::update(uint32_t frameIndex, double deltaTime)
{
	if (m_StateMachine->currentType().empty()) {
		m_StateMachine->changeState(BatStateMachine::BORN);
	}
	m_StateMachine->update(frameIndex, deltaTime);
}

void BatMovementComp::postUpdate(uint32_t frameIndex, double deltaTime)
{
	MovementComp::postUpdate(frameIndex, deltaTime);
}


void BatMovementComp::setRandVelocity()
{
	auto rand = MyRand::range(0, 3);
	const auto batParam = dynamic_cast<const BatParam*>(m_Param);
	if (!batParam) { return; }
	const float speed = batParam->moveSpeed();
	switch (rand)
	{
	case 0: {
		setVeloX(speed);
		break;
	}
	case 1: {
		setVeloX(-speed);
		break;
	}
	case 2: {
		setVeloY(speed);
		break;
	}
	case 3: {
		setVeloY(-speed);
		break;
	}
	} 
}

