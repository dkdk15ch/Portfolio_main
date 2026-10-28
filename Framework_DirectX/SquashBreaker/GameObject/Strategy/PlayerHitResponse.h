#pragma once
#include "Strategy/IHitResponse.h"
#include "Tag.h"

class GameBase;
struct HitInfo;

namespace PlayerHitResponse {
	class Factory : public IHitResponseFactory {
		friend class Ball;
	public:
		Factory() = default;
		~Factory() = default;
		virtual IHitResponse* create(CollisionTag otherLayer) const override;
	};

	class Block : public IHitResponse {
	protected:
		using IHitResponse::IHitResponse;
		virtual void onHit(GameBase& owner, const HitInfo& info) override;
	};
	class Wall : public IHitResponse {
	protected:
		using IHitResponse::IHitResponse;
		virtual void onHit(GameBase& owner, const HitInfo& info) override;
	};

	class Ball : public IHitResponse {
	protected:
		using IHitResponse::IHitResponse;
		virtual void onHit(GameBase& owner, const HitInfo& info) override;
	};
}