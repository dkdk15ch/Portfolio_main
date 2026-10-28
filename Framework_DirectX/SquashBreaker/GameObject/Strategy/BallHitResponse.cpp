#include "GameObject/Strategy/BallHitResponse.h"
#include "GameObject/GameBase.h"
#include "GameObject/Ball.h"
#include "GameObject/Param/BallParam.h"
#include "Component/Collider/ColliderCompBase.h"
#include "Component/Mover/BallMovementComp.h"
#include "InOutput/Audio.h"


namespace BallHitResponse {
	IHitResponse* Factory::create(CollisionTag otherLayer) const {
		switch (otherLayer) {
		case CollisionTag::Static: {
			return new Wall();
		}
		case CollisionTag::Player: {
			return new Player();
		}
		case CollisionTag::Block: {
			return new Block();
		}

		}
		return nullptr;
	}

	void Block::onHit(GameBase& owner, const HitInfo& info) {
		const auto& moveComp = dynamic_cast<BallMovementComp*>(owner.getComponent(CompType::MovementComp));
		moveComp->hitBlock(
			static_cast<int>(info.face)
			, info.hitPos
		);
		auto& ball = *static_cast<Ball*>(&owner);
		auto& mover = *ball.mover();

		auto p = static_cast<const BallParam*>(owner.param());
		Audio::playSE(p->sE_Bound(), p->volumeBound());

		mover.OnObstacleHit.execute();
	}

	void Wall::onHit(GameBase& owner, const HitInfo& info) {
		auto p = static_cast<const BallParam*>(owner.param());
		auto mover = static_cast<Ball*>(&owner)->mover();
		mover->OnObstacleHit.execute();

		float cor = p->corWall();
		const auto& velo = mover->velocity();
		if (velo.Length() < p->speedMin()) {
			cor = 1.0f;
		}

		Audio::playSE(p->sE_Bound(), p->volumeBound());

		using namespace CollisionMath;
		switch (info.face)
		{
		case Face::RIGHT:
		case Face::LEFT:
		{
			mover->setVeloX(velo.x * -cor);
			return;
		}
		case Face::TOP: {
			mover->setVeloY(velo.y * -cor);
			return;
		}
		case Face::BOTTOM: {
			mover->setVeloY(velo.y * -cor);
			if (mover->isBounded()) {
				mover->init();
			}
			else {
				mover->bound();
			}
			return;
		}
		case Face::FRONT: {
			mover->setVeloZ(velo.z * -cor);
			mover->setVeloY(velo.y * p->frontHitVerticalDamping());
			return;
		}
		case Face::BACK: {
			mover->init();
			return;
		}
		default:
			break;
		}

	}

	void Player::onHit(GameBase& owner, const HitInfo& info) {

	}

}//	namespace BallHitResponse
