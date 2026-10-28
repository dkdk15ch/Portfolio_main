#include "GameObject/State/BatState.h"
#include "Component/Animator.h"
#include "GameObject/Enemy/Enemy.h"
#include "GameObject/Enemy/Bat.h"
#include "GameObject/Ball.h"
#include "GameObject/Param/BatParam.h"
#include "Component/Collider/SphereColliderComp.h"
#include "Component/Mover/BallMovementComp.h"
#include "Component/Mover/BatMovementComp.h"
#include "InOutput/Audio.h"

BatStateMachine::BatStateMachine(GameBase* owner, BatMovementComp* mover)
	: StateMachineBase(owner)
	, m_Mover(mover)
	, m_Animator(nullptr)
{
	m_StateList[BORN] = MAKE_UNIQUE_LOG<BatState::Born>(this);
	m_StateList[IDLE] = MAKE_UNIQUE_LOG<BatState::Idle>(this);
	m_StateList[DEATH] = MAKE_UNIQUE_LOG<BatState::Death>(this);
	m_StateList[ATTACK] = MAKE_UNIQUE_LOG<BatState::Attack>(this);
	m_StateList[DESPAWN] = MAKE_UNIQUE_LOG<BatState::Despawn>(this);
}

void BatStateMachine::init() {
	m_Animator = dynamic_cast<Animator*>(m_Owner->getComponent(CompType::Animator));
	assert(m_Animator);
}

namespace BatState {

	void Born::enter() {
		animator()->setNextAnimation("Death");
		animator()->setTimeScale(0.0);
		if (auto collider = static_cast<Enemy*>(owner())->collider()) {
			collider->disable();
		}
		if (auto param = dynamic_cast<const BatParam*>(owner()->param())) {
			m_SpawnTimer = param->spawnTime();
		}
	}

	void Born::update(uint32_t frameIndex, double deltaTime) {
		if (m_SpawnTimer <= 0) {
			animator()->setTimeScale(1.0);
		}
		else {
			m_SpawnTimer -= deltaTime;
			return;
		}

		if (!animator()->isPlaying()) {
			changeState(BatStateMachine::IDLE);
			return;
		}
	}

	void Born::exit() {
		if (auto param = dynamic_cast<const BatParam*>(owner()->param())) {
			mover()->setRotX(param->bornOffsetRotX());
			mover()->addPosY(param->bornOffsetPosY());
		}

 		if (auto collider = static_cast<Enemy*>(owner())->collider()) {
			collider->enable();
		}
	}

	void Idle::enter() {
		animator()->setNextAnimation("Flying");
		m_CastedOwner = dynamic_cast<Bat*>(owner());
		assert(m_CastedOwner);
		setRandVelocity();
		if (auto param = dynamic_cast<const BatParam*>(owner()->param())) {
			m_AttackInterval = param->attackIntrval();
		}

	}

	void Idle::update(uint32_t frameIndex, double deltaTime) {
		//	死亡へ
		if (m_CastedOwner->hp() <= 0) {
			changeState(BatStateMachine::DEATH);
			return;
		}
		//	攻撃へ
		if (m_AttackInterval <= 0) {
			changeState(BatStateMachine::ATTACK);
			return;
		}

		m_AttackInterval -= deltaTime;
		// 移動
	}

	void Idle::exit() {
		mover()->setVeloZero();
	}

	void Death::enter() {
		animator()->setNextAnimation("Death");
		mover()->setVeloZero();
		if (auto p = dynamic_cast<const BatParam*>(owner()->param())) {
			m_DespawnTimer = p->despawnTime();
			Audio::playSE(p->sE_Dead(), p->volumeDead());
		}
	}

	void Death::update(uint32_t frameIndex, double deltaTime) {
		if (animator()->isPlaying()) {
			return;
		}
		if (m_DespawnTimer <= 0) {
			changeState(BatStateMachine::DESPAWN);
			return;
		}
		m_DespawnTimer -= deltaTime;

	}

	void Death::exit() {
		auto enemy = dynamic_cast<Enemy*>(owner());
		assert(enemy);
		enemy->collider()->setLayer(CollisionTag::disable);
	}

	void Attack::enter() {
		animator()->setNextAnimation("Attack");
		//	ボールの状態初期化して、発射
		auto bat = dynamic_cast<Bat*>(owner());
		bat->resetAttackBall();
		auto ball = bat->attackBall();
		const auto& launchPos = owner()->transform()->position();
		ball->mover()->setPos(launchPos);
		auto launchTarget = DirectX::SimpleMath::Vector3(
			launchPos.x, launchPos.y, 0.f
		);
		if (auto p = dynamic_cast<const BatParam*>(owner()->param())) {
			ball->mover()->launchToTarget(launchTarget, p->lunchForce());
			Audio::playSE(p->sE_Attack(), p->volumeAttack());
		}

	}

	void Attack::update(uint32_t frameIndex, double deltaTime) {
		if (!animator()->isPlaying()) {
			changeState(BatStateMachine::IDLE);
			return;
		}

	}

	void Attack::exit() {
		auto bat = dynamic_cast<Bat*>(owner());
		auto ball = bat->attackBall();
	}

	void Despawn::enter() {
		owner()->disableNextFrame();
	}

	void Despawn::update(uint32_t frameIndex, double deltaTime) {
		owner()->disableNextFrame();
	}

	void Despawn::exit() {

	}

}// namespace BatState


IBatState::IBatState(BatStateMachine* stateMachine)
	: IState(stateMachine)
{
}

void IBatState::setRandVelocity()
{
	mover()->setRandVelocity();
}

void IBatState::changeState(const std::string& nextState)
{
	m_StateMachine->changeState(nextState);
}

