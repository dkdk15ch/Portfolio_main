#pragma once
#include "Strategy/IHitResponse.h"
#include "GameObject/Strategy/BallHitResponse.h"
#include "Tag.h"

class GameBase;
struct HitInfo;

namespace EnemyBallHitResponse {
	class Factory : public BallHitResponse::Factory {
	public:
		Factory() = default;
		~Factory() = default;
		virtual IHitResponse* create(CollisionTag otherLayer) const override;
	};

	class Block : public BallHitResponse::Block {
	protected:
		using BallHitResponse::Block::Block;
		virtual void onHit(GameBase& owner, const HitInfo& info) override final;
	};
	class Wall : public BallHitResponse::Wall {
	protected:
		using BallHitResponse::Wall::Wall;
		virtual void onHit(GameBase& owner, const HitInfo& info) override final;
	};
	
	class Player : public BallHitResponse::Player {
	protected:
		using BallHitResponse::Player::Player;
		virtual void onHit(GameBase& owner, const HitInfo& info) override final;
	};
	class Racket : public IHitResponse {
	protected:
		using IHitResponse::IHitResponse;
		virtual void onHit(GameBase& owner, const HitInfo& info) override;
	};
	class Enemy: public IHitResponse {
	protected:
		using IHitResponse::IHitResponse;
		virtual void onHit(GameBase& owner, const HitInfo& info) override;
	};

}