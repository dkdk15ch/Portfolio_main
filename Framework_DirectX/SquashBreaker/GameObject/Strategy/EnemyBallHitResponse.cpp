#include "GameObject/Strategy/EnemyBallHitResponse.h"
#include "GameObject/GameBase.h"
#include "GameObject/Ball.h"
#include "GameObject/Param/CollisionLayerParam.h"
#include "Component/Collider/ColliderCompBase.h"
#include "Component/Mover/BallMovementComp.h"
#include "Component/Collider/SphereColliderComp.h"


namespace EnemyBallHitResponse {
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
		case CollisionTag::Racket: {
			return new Racket();
		}
		case CollisionTag::Enemy: {
			return new Enemy();
		}									
		}
		return nullptr;
	}

	void Block::onHit(GameBase& owner, const HitInfo& info) {
		
		owner.disableNextFrame();
	}

	void Wall::onHit(GameBase& owner, const HitInfo& info) {
		auto p = static_cast<const BallParam*>(owner.param());
		auto& ball = *static_cast<Ball*>(&owner);
		auto& mover = *ball.mover();


		float cor = p->corWall();
		const auto& velo = mover.velocity();
		if (velo.Length() < p->speedMin()) {
			cor = 1.0f;
		}

		using namespace CollisionMath;
		switch (info.face)
		{
		case Face::RIGHT:
		case Face::LEFT:
		{
			mover.setVeloX(velo.x * -cor);
			return;
		}
		case Face::TOP: {
			mover.setVeloY(velo.y * -cor);
			return;
		}
		case Face::BOTTOM: 
		case Face::FRONT: 
		case Face::BACK: {
			ball.disable();
			return;
		}
		default:
			break;
		}

	}

	void Player::onHit(GameBase& owner, const HitInfo& info) {
		owner.disableNextFrame();
	}

	void Racket::onHit(GameBase& owner, const HitInfo& info) {
		//	ブロックにもヒットするようにターゲットレイヤー変更
		auto collider = dynamic_cast<SphereColliderComp*>(owner.getComponent(CompType::ColliderComp));
		if (!collider) { return; }
		auto layerParam = CollisionLayerParam::layerParam();
		collider->targetLayer().setTag(layerParam->ballTarget());
		collider->setLayer(CollisionTag::Ball);
		auto ball = dynamic_cast<Ball*>(&owner);
		ball->checkRayToBlock(); 
	}

	void Enemy::onHit(GameBase& owner, const HitInfo& info)
	{
		owner.disableNextFrame();
	}

}//	namespace BallHitResponse
