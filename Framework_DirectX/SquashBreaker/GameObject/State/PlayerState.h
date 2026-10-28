#pragma once
#include <cstdint>
#include <DirectXMath.h>
#include "State/IState.h"
#include "Delegate.h"

class GameBase;
class PlayerMovementComp;
class Animator;
class Pad;
class Player;

namespace PlayerHitResponse {
	class Block;
	class Wall;
	class Ball;
}

class PlayerStateMachine : public StateMachineBase {
	friend class IPlayerState;
public:
	PlayerStateMachine(GameBase* owner, PlayerMovementComp* mover);
	void init();

	inline static constexpr const char* IDLE = "Idle";
	inline static constexpr const char* MOVE = "Move";
	inline static constexpr const char* JUMP = "Jump";
	inline static constexpr const char* AIM = "Aim";
	inline static constexpr const char* TAKE_BACK = "TakeBack";
	inline static constexpr const char* CHECK_BALL = "CheckBallDistance";
	inline static constexpr const char* SWING = "Swing";
	inline static constexpr const char* HIT = "Hit";
	inline static constexpr const char* DAMAGE = "Damage";

	Delegate::Singlecast<void(void)> OnEnterAimState;

private:
	PlayerMovementComp* m_Mover;
	Animator* m_Animator;
	Pad* m_Pad;
};

class IPlayerState : public IState{
	friend class PlayerHitResponse::Block;
	friend class PlayerHitResponse::Wall;
	friend class PlayerHitResponse::Ball;

public:
	IPlayerState(PlayerStateMachine* stateMachine);
	virtual ~IPlayerState() = default;
protected:

	PlayerMovementComp* mover()const {
		return static_cast<PlayerStateMachine*>(m_StateMachine)->m_Mover;
	};
	Animator* animator()const {
		return static_cast<PlayerStateMachine*>(m_StateMachine)->m_Animator;
	};
	Pad* pad()const {
		return static_cast<PlayerStateMachine*>(m_StateMachine)->m_Pad;
	};

	Player* castedOwner()const;

	//	Wrapper
	void changeState(const std::string& nextState);
	void move(double deltaTime);
	bool assistMove(float ballPosX);
	void controllAim(double deltaTime);
	void aimStart();
	void aimEnd();
	void changeSwing();
	void jump();
	void debugColliderChangeColor(DirectX::XMVECTOR color);
	//	ToState
	bool toJumpState();
	bool toCheckBallDistanceState();
	bool toAimState();
	bool toMoveState();
private:
	static inline bool m_BIsInit = false;
};

namespace PlayerState {
	class Idle : public IPlayerState {
	public:
		using IPlayerState::IPlayerState;
		void enter() override final;
		void update(uint32_t frameIndex, double deltaTime) override final;
	};

	class Move : public IPlayerState {
	public:
		using IPlayerState::IPlayerState;
		void enter() override final;
		void update(uint32_t frameIndex, double deltaTime) override final;
	};

	class Jump : public IPlayerState {
		friend PlayerStateMachine;
	public:
		using IPlayerState::IPlayerState;
		void enter() override final;
		void update(uint32_t frameIndex, double deltaTime) override final;
		void exit() override final;
	private:
		bool m_BCanCancel = false;
		bool m_BIsFirstFrame = true;
		bool m_BIsPlayedSE = false;
		static inline uint32_t CANCEL_FRAME; 
		static inline float CANCEL_BONUS;
	};

	class Aim : public IPlayerState {
		friend PlayerStateMachine;
	public:
		using IPlayerState::IPlayerState;
		void enter() override final;
		void update(uint32_t frameIndex, double deltaTime) override final;
		void exit() override final;

	private:
		float m_BallFuturePos = FLT_MIN;
		bool m_BIsCalcatedBallFuturePos = false;
		static inline float CAN_AIM_DISTANCE;
	};

	class TakeBack : public IPlayerState {
		friend PlayerStateMachine;
	public:
		using IPlayerState::IPlayerState;
		void enter() override final;
		void update(uint32_t frameIndex, double deltaTime) override final;
		void exit() override final;
	private:
		double m_StartTimeScale = 1.0;
		static inline double TAKE_BACK_TIME;
		double m_TakeBackTime = 1000.0;
	};

	class CheckBallDistance : public IPlayerState {
		friend PlayerStateMachine;
	public:
		using IPlayerState::IPlayerState;
		void enter() override final;
		void update(uint32_t frameIndex, double deltaTime) override final;
		void exit() override final;

		void setIsEnemyBall();

	private:
		static inline float CAN_AIM_DISTANCE;
		bool m_BIsEnemyBallInCollider = false;
	};

	class Swing : public IPlayerState {
		friend PlayerStateMachine;
	public:
		using IPlayerState::IPlayerState;
		void enter() override final;
		void update(uint32_t frameIndex, double deltaTime) override final;
		void exit() override final;

	private:
		bool m_BCanCancel = false;
		uint32_t m_CanCancelStartFrame = 1000; 
		static inline uint32_t CANCEL_FRAME;
	};

	class Hit : public IPlayerState {
	public:
		using IPlayerState::IPlayerState;
		void enter() override final;
		void update(uint32_t frameIndex, double deltaTime) override final;
		void exit() override final;
	};

	class Damage : public IPlayerState {
	public:
		using IPlayerState::IPlayerState;
		void enter() override final;
		void update(uint32_t frameIndex, double deltaTime) override final;
		void exit() override final;
	};

};// namespace PlayerState