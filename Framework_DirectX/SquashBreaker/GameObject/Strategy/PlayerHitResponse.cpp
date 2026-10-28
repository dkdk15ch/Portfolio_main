#include "GameObject/Strategy/PlayerHitResponse.h"
#include "GameObject/GameBase.h"
#include "Component/Mover/PlayerMovementComp.h"
#include "Component/Collider/ColliderCompBase.h"
#include "GameObject/State/PlayerState.h"
#include "Manager/GameManager.h"

namespace PlayerHitResponse {
	IHitResponse* Factory::create(CollisionTag otherLayer) const {
		switch (otherLayer) {
		case CollisionTag::Static: {
			return new Wall();
		}
		case CollisionTag::Block: {
			return new Block();
		}
		case CollisionTag::EnemyBall:
		{
			return new Ball();
		}
		}
		return nullptr;
	}


	void Block::onHit(GameBase& owner, const HitInfo& info) {
		//	押し出し
		auto mover = static_cast<MovementComp*>(owner.parent()->getComponent(CompType::MovementComp));
		mover->addPos(info.normal * info.overlapLength);
	}

	void Wall::onHit(GameBase& owner, const HitInfo& info) {
		//	押し出し
		auto mover = static_cast<MovementComp*>(owner.parent()->getComponent(CompType::MovementComp));
		mover->addPos(info.normal * info.overlapLength);
	}

	void Ball::onHit(GameBase& owner, const HitInfo& info) {
		auto mover = dynamic_cast<PlayerMovementComp*>(owner.parent()->getComponent(CompType::MovementComp));
		if (mover) {
			auto& SM = mover->m_StateMachine;
			if (!SM->isCurrent(PlayerStateMachine::DAMAGE)) {
				SM->changeState(PlayerStateMachine::DAMAGE);
			}
		}
	}

}//	namespace EnemyHitResponse
