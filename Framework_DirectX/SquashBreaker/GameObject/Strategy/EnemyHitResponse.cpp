#include "GameObject/Strategy/EnemyHitResponse.h"
#include "GameObject/GameBase.h"
#include "GameObject/Enemy/Enemy.h"
#include "Component/MovementComp.h"
#include "Component/Collider/ColliderCompBase.h"

namespace EnemyHitResponse {
	IHitResponse* Factory::create(CollisionTag otherLayer) const {
		switch (otherLayer) {
		case CollisionTag::Static: {
			return new Wall();
		}
		case CollisionTag::Player: {
			return new Player();
		}
		case CollisionTag::Enemy: {
			return new Enemy();
		}
		case CollisionTag::Block: {
			return new Block();
		}
		case CollisionTag::EnemyBall:
		case CollisionTag::Ball: 
		{
			return new Ball();
		}
		}
		return nullptr;
	}

	void Block::onHit(GameBase& owner, const HitInfo& info) {
		auto mover = dynamic_cast<MovementComp*>(owner.getComponent(CompType::MovementComp));
		mover->addPos(info.normal * info.overlapLength);
	}

	void Wall::onHit(GameBase& owner, const HitInfo& info) {

	}

	void Player::onHit(GameBase& owner, const HitInfo& info) {

	}

	void Enemy::onHit(GameBase& owner, const HitInfo& info) {
		auto mover = dynamic_cast<MovementComp*>(owner.getComponent(CompType::MovementComp));
		mover->addPos(info.normal * info.overlapLength);
		mover->setReverseVelo();
	}

	void Ball::onHit(GameBase& owner, const HitInfo& info) {
		auto e = static_cast<::Enemy*>(&owner);
		e->subHp(1);
	}

}//	namespace EnemyHitResponse
