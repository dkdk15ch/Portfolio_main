#include "GameObject/State/PlayerState.h"
#include "Component/Mover/PlayerMovementComp.h"
#include "InOutput/Pad.h"
#include "InOutput/Audio.h"
#include "Component/Animator.h"
#include "Animation/Animation.h"
#include <DirectXColors.h>
#include "GameObject/Param/PlayerParam.h"
#include "GameObject/Player.h"
#include "Manager/SquashGameManager.h"

namespace {
	constexpr float AutoSwingOffset = 0.f;
}

PlayerStateMachine::PlayerStateMachine(GameBase* owner, PlayerMovementComp* mover)
	: StateMachineBase(owner)
	, m_Mover(mover)
	, m_Animator(nullptr)
	, m_Pad(nullptr)
{
	m_StateList[IDLE] = MAKE_UNIQUE_LOG<PlayerState::Idle>(this);
	m_StateList[IDLE] = MAKE_UNIQUE_LOG<PlayerState::Idle>(this);
	m_StateList[MOVE] = MAKE_UNIQUE_LOG<PlayerState::Move>(this);
	m_StateList[JUMP] = MAKE_UNIQUE_LOG<PlayerState::Jump>(this);
	m_StateList[AIM] = MAKE_UNIQUE_LOG<PlayerState::Aim>(this);
	m_StateList[TAKE_BACK] = MAKE_UNIQUE_LOG<PlayerState::TakeBack>(this);
	m_StateList[CHECK_BALL] = MAKE_UNIQUE_LOG<PlayerState::CheckBallDistance>(this);
	m_StateList[SWING] = MAKE_UNIQUE_LOG<PlayerState::Swing>(this);
	m_StateList[HIT] = MAKE_UNIQUE_LOG<PlayerState::Hit>(this);
	m_StateList[DAMAGE] = MAKE_UNIQUE_LOG<PlayerState::Damage>(this);
}

void PlayerStateMachine::init() {
	m_Animator = dynamic_cast<Animator*>(m_Owner->getComponent(CompType::Animator));
	assert(m_Animator);
	m_Pad = Pad::m_Instance;
	assert(m_Pad);

	auto p = dynamic_cast<const PlayerParam*>(m_Owner->param());
	PlayerState::Jump::CANCEL_FRAME = p->jumpCancelFrame();
	PlayerState::Jump::CANCEL_BONUS = p->jumpCancelBonus();
	PlayerState::Aim::CAN_AIM_DISTANCE = p->canAimDistance();
	PlayerState::CheckBallDistance::CAN_AIM_DISTANCE = p->canAimDistance();
	PlayerState::TakeBack::TAKE_BACK_TIME = p->takeBackTime();
	PlayerState::Swing::CANCEL_FRAME = p->swingCancelFrame();
}


#pragma region Wrapper

void IPlayerState::move(double deltaTime) { mover()->move(deltaTime); }
bool IPlayerState::assistMove(float ballPosX) {return mover()->assistMove(ballPosX); }
void IPlayerState::controllAim(double deltaTime) { mover()->controllAim(deltaTime); }
void IPlayerState::aimStart() { mover()->aimStart(); }
void IPlayerState::aimEnd() { mover()->aimEnd(); }
void IPlayerState::changeSwing() { mover()->changeSwing(); }
void IPlayerState::jump() { mover()->jump(); }
void IPlayerState::debugColliderChangeColor(DirectX::XMVECTOR color)
{
	mover()->debugColliderChangeColor(color);
}
#pragma endregion

#pragma region State

IPlayerState::IPlayerState(PlayerStateMachine* stateMachine)
	: IState(stateMachine)
{
}

namespace PlayerState {
	typedef DirectX::GamePad::ButtonStateTracker PadTracker;

#pragma region Idle
	void Idle::enter() {
		animator()->setNextAnimation("Idle");
	};
	void Idle::update(uint32_t frameIndex, double deltaTime) {
		//	Check ChangeState
		if (toJumpState()) return;
		if (toAimState()) return;
		if (toMoveState()) return;

		changeSwing();
	};
#pragma endregion

#pragma region Move
	void Move::enter() {
		animator()->setNextAnimation("Run");
	};
	void Move::update(uint32_t frameIndex, double deltaTime) {
		//	Check ChangeState
		if (toJumpState()) return;
		if (toAimState()) return;
		if (!pad()->hasMoveInput()) {
			changeState(PlayerStateMachine::IDLE);
			return;
		}

		move(deltaTime);

		changeSwing();
	};
#pragma endregion

#pragma region Jump
	void Jump::enter() {
		animator()->setNextAnimation("Jump");
		jump();
	};
	void Jump::update(uint32_t frameIndex, double deltaTime) {
		if (m_BIsFirstFrame) {
			m_BIsFirstFrame = false;
			return;
		}
		//	単押しされたらキャンセル
		//	おしっぱで通常ジャンプ
		uint32_t currentF = animator()->currentFrame();
		bool bIsPlayingAnima = animator()->isPlaying();
		if (bIsPlayingAnima && currentF < CANCEL_FRAME) {
    			if (!(pad()->m_GamepadState.buttons.a ||
				pad()->m_KeyboardState.Space)) {
				m_BCanCancel = true;
			}
		
			if (m_BCanCancel) {
				changeState(PlayerStateMachine::IDLE);
				return;
			}
		}
		else {
   			m_BCanCancel = false;
			if (!m_BIsPlayedSE) {
				auto p = static_cast<const PlayerParam*>(castedOwner()->param());
				Audio::playSE(p->pathSE_Jump(), p->volumeJump());
				m_BIsPlayedSE = true;
			}
		}

		//	Check ChangeState
		if (toAimState()) return;
		if (mover()->isOnGround()) {
			changeState(PlayerStateMachine::IDLE);
			return;
		}

		changeSwing();
	};
	void Jump::exit() {
		if (m_BCanCancel) {
			auto playerMover = mover();
			if (playerMover) {
				playerMover->setVeloY(0.0f);
				playerMover->setMoveBounsValue(CANCEL_BONUS);
			}
			auto p = static_cast<const PlayerParam*>(castedOwner()->param());
			Audio::playSE(p->pathSE_SplitStep(), p->volumeSplitStep());
		}
		m_BIsFirstFrame = false;
		m_BCanCancel = false;
		m_BIsPlayedSE = false;
	};
#pragma endregion

#pragma region Aim
	void Aim::enter() {
		animator()->setNextAnimation("Run");
		aimStart();
		auto castedSM = dynamic_cast<PlayerStateMachine*>(m_StateMachine);
		castedSM->OnEnterAimState.execute();
	};
	void Aim::update(uint32_t frameIndex, double deltaTime) {
		//	Check ChangeState
		if (toJumpState()) { return; }
		
		if (m_BIsCalcatedBallFuturePos && assistMove(m_BallFuturePos)) {
			changeState(PlayerStateMachine::TAKE_BACK);
			return;
		}
		if (pad()->m_PadTracker.b == PadTracker::RELEASED ||
			pad()->m_KeyTracker.released.K ) {
			changeState(PlayerStateMachine::SWING);
			return;
		}

		auto mgr = SquashGameManager::castedInstance();
		auto pos = mgr->ballCurrentPos();
		auto playerPosZ = owner()->transform()->worldPos().z;
		if (pos.z <= playerPosZ + AutoSwingOffset &&
			!mgr->isBallDroping()) {
			changeState(PlayerStateMachine::SWING);
			return;
		}
	
		if (!m_BIsCalcatedBallFuturePos) {
			float targetPos = owner()->transform()->worldPos().z;
			auto futurePos = SquashGameManager::castedInstance()->ballFuturePos(targetPos);
			m_BallFuturePos = futurePos.x;
			if (m_BallFuturePos != FLT_MIN) {
				m_BIsCalcatedBallFuturePos = true;
			}
		}

		changeSwing();
		controllAim(deltaTime);

#ifdef _DEBUG
		if (pad()->m_PadTracker.a == PadTracker::PRESSED ||
			pad()->m_KeyTracker.pressed.K) {
			debugColliderChangeColor(DirectX::Colors::Yellow);
		}
#endif // _DEBUG
	};
	void Aim::exit() {
			int type = mover()->swingType();
			if (type == SwingType::BACK) {
				animator()->setNextAnimation("Swing");
			}
			if (type == SwingType::FORE) {
				animator()->setNextAnimation("Swing");
			}
			if (type == SwingType::SMASH) {
				animator()->setNextAnimation("Smash");
			}

			m_BallFuturePos = FLT_MIN;
			m_BIsCalcatedBallFuturePos = false;

#ifdef _DEBUG
		debugColliderChangeColor(DirectX::Colors::Red);
#endif // _DEBUG
	};
#pragma endregion

#pragma region TakeBack
	void TakeBack::enter()
	{
		animator()->setTimeScale(m_StartTimeScale);
		m_TakeBackTime = TAKE_BACK_TIME;
	}
	void TakeBack::update(uint32_t frameIndex, double deltaTime)
	{
		if (toJumpState()) { return; }

		if (m_TakeBackTime > 0) {
			m_TakeBackTime -= deltaTime;
		}
		else
		{
			animator()->setTimeScale(0.0);
		}

		if (pad()->m_PadTracker.b == PadTracker::RELEASED ||
			pad()->m_KeyTracker.released.K) {
			changeState(PlayerStateMachine::SWING);
			return;
		}

		auto mgr = SquashGameManager::castedInstance();
		auto pos = mgr->ballCurrentPos();
		if (pos.z <= owner()->transform()->worldPos().z + AutoSwingOffset &&
			!mgr->isBallDroping()) {
			changeState(PlayerStateMachine::SWING);
			return;
		}


		controllAim(deltaTime);
	}
	void TakeBack::exit()
	{
		animator()->setTimeScale(m_StartTimeScale);
		m_StartTimeScale = 1.0f;
		m_TakeBackTime = 1000.0;
	}
#pragma endregion

#pragma region CheckBallDistance
	void CheckBallDistance::enter()
	{
		aimEnd();
	}
	void CheckBallDistance::update(uint32_t frameIndex, double deltaTime)
	{
		//	ボールとの距離が閾値内にあるとき移行可能。
		auto mgr = SquashGameManager::castedInstance();
		const auto& ballPos = mgr->ballCurrentPos();
		const auto& playerPos = owner()->transform()->worldPos();
		float dis = DirectX::SimpleMath::Vector3::DistanceSquared(ballPos, playerPos);
		if (dis > CAN_AIM_DISTANCE || m_BIsEnemyBallInCollider) { 
			changeState(PlayerStateMachine::AIM);
			return; 
		}

		changeState(PlayerStateMachine::IDLE);
	}
	void CheckBallDistance::exit()
	{
		m_BIsEnemyBallInCollider = false;
	}
	void CheckBallDistance::setIsEnemyBall()
	{
		m_BIsEnemyBallInCollider = true;
	}
#pragma endregion

#pragma region Swing
	void Swing::enter() {
		animator()->setTimeScale(1.0);
		aimEnd();
		m_CanCancelStartFrame = static_cast<uint32_t>(animator()->duration()) - CANCEL_FRAME;

		auto pos = SquashGameManager::castedInstance()->ballCurrentPos();
	};
	void Swing::update(uint32_t frameIndex, double deltaTime) {
		//	キャンセル可能フレームからジャンプキャンセル
		//	その前にジャンプ押してると、キャンセル不可
		if (animator()->currentFrame() >= m_CanCancelStartFrame) {
			if (m_BCanCancel && toJumpState()) {
				return;
			}
		}
		else {
			if (pad()->m_GamepadState.buttons.b ||
				pad()->m_KeyboardState.Space) {
				m_BCanCancel = false;
			}
		}

		if (!animator()->isPlaying()) {
			changeState(PlayerStateMachine::IDLE);
			return;
		}
	};
	void Swing::exit() {
		m_BCanCancel = false;
	};
#pragma endregion

#pragma region Hit
	void Hit::enter() {
		animator()->setNextAnimation("HitResponse");
		animator()->setTimeScale(2.0);	// Param
	};
	void Hit::update(uint32_t frameIndex, double deltaTime) {
		if (!animator()->isPlaying()) {
			changeState(PlayerStateMachine::IDLE);
			return;
		}
	};
	void Hit::exit() {
		animator()->setTimeScale(1.0);
	};
#pragma endregion

#pragma region Damage
	void Damage::enter() {
		animator()->setNextAnimation("HitResponse");
		animator()->setTimeScale(1.7);		// Param
		
		auto p = static_cast<const PlayerParam*>(castedOwner()->param());
		Audio::playSE(p->pathSE_Damage(), p->volumeDamage());
	};
	void Damage::update(uint32_t frameIndex, double deltaTime) {
		if (!animator()->isPlaying()) {
			changeState(PlayerStateMachine::IDLE);
			return;
		}
	};
	void Damage::exit() {
		animator()->setTimeScale(1.0);
	};
#pragma endregion

};//	namespace PlayerState


Player* IPlayerState::castedOwner() const
{
	return static_cast<Player*>(owner());
}

void IPlayerState::changeState(const std::string& nextState)
{
	m_StateMachine->changeState(nextState);
}

bool IPlayerState::toJumpState()
{
	if (!mover()->isOnGround()) {
		return false;
	}

	if (pad()->m_GamepadState.buttons.a ||
		pad()->m_KeyboardState.Space) {
		changeState(PlayerStateMachine::JUMP);
		return true;
	}
	return false;
}

bool IPlayerState::toCheckBallDistanceState()
{
	typedef DirectX::GamePad::ButtonStateTracker PadTracker;
	if (pad()->m_PadTracker.b == PadTracker::PRESSED ||
		pad()->m_KeyTracker.pressed.K)
	{
		changeState(PlayerStateMachine::CHECK_BALL);
		return true;
	}
	return false;
}

bool IPlayerState::toAimState()
{
	typedef DirectX::GamePad::ButtonStateTracker PadTracker;
	if (pad()->m_PadTracker.b == PadTracker::PRESSED ||
		pad()->m_KeyTracker.pressed.K)
	{
		//auto mgr = SquashGameManager::castedInstance();
		//const auto& ballPos = mgr->ballCurrentPos();
		//const auto& playerPos = owner()->transform()->worldPos();
		//float dis = DirectX::SimpleMath::Vector3::DistanceSquared(ballPos, playerPos);
		//if (dis > PlayerState::Aim::CAN_AIM_DISTANCE) {
		//	return false;
		//}

		changeState(PlayerStateMachine::AIM);
		return true;
	}
	return false;
}

bool IPlayerState::toMoveState()
{
	if (pad()->hasMoveInput()) {
		changeState(PlayerStateMachine::MOVE);
		return true;
	}
	return false;
}

#pragma endregion


