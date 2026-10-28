#pragma once
#include "GameObject/GameBase.h"
#include "GameObject/UniqueName.h"
#include <SimpleMath.h>
#include "GameObject/Param/PlayerParam.h"

class MovementComp;
class SphereColliderComp;
class ModelRenderComp;
class Animator;
class IHitResponseFactory;
class EnemySpawner;
namespace EnemyObjects { class CollisionObject; }


class Enemy : public GameBase, public UniqueName
{
	friend EnemyObjects::CollisionObject;
public:
	Enemy(std::string UniqueName = "Enenmy");
	virtual ~Enemy();
	virtual void term() override;

	void disable()override {
		GameBase::disable();
		showLifeTime();
	}

	int hp()const { return m_HP; }
	void subHp(int hp) { m_HP -= hp; }

	SphereColliderComp* collider()const;
	void showLifeTime();

protected:
	virtual void implPreupdate(uint32_t frameIndex, double deltaTime) override;
	virtual void implupdate(uint32_t frameIndex, double deltaTime) override;
	virtual void implOnHit(const HitInfo& info) override;

	double m_LifeTime;

	MovementComp* m_Mover;
	SphereColliderComp* m_Collider;
	GameBase* m_ColliderObj;
	LogPtr<IHitResponseFactory> m_ResponseFactory;
	int m_HP = 1;

private:
};
