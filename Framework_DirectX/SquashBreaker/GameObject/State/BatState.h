#pragma once
#include <cstdint>
#include "State/IState.h"

class GameBase;
class Animator;
class BatMovementComp;
class Bat;

class BatStateMachine : public StateMachineBase {
	friend class IBatState;
public:
	BatStateMachine(GameBase* owner, BatMovementComp* mover);
	void init();

	inline static constexpr const char* BORN = "Born";
	inline static constexpr const char* IDLE = "Idle";
	inline static constexpr const char* DEATH = "Death";
	inline static constexpr const char* ATTACK = "Attack";
	inline static constexpr const char* DESPAWN = "Despawn";

private:
	BatMovementComp* m_Mover;
	Animator* m_Animator;
};

class IBatState : public IState{
public:
	friend BatMovementComp;

	IBatState(BatStateMachine* stateMachine);
	virtual ~IBatState() = default;

protected:
	BatMovementComp* mover()const {
		return static_cast<BatStateMachine*>(m_StateMachine)->m_Mover;
	};
	Animator* animator()const {
		return static_cast<BatStateMachine*>(m_StateMachine)->m_Animator;
	};

	void setRandVelocity();
	void changeState(const std::string& nextState);
};


namespace BatState {

	class Born : public IBatState {
	public:
		using IBatState::IBatState;

		void enter() override final;
		void update(uint32_t frameIndex, double deltaTime) override final;
		void exit() override final;


	private:
		double m_SpawnTimer;
	};

	class Idle : public IBatState {
	public:
		using IBatState::IBatState;
		void enter() override final;
		void update(uint32_t frameIndex, double deltaTime) override final;
		void exit() override final;

	private:
		Bat* m_CastedOwner = nullptr;
		double m_AttackInterval;
	};

	class Death : public IBatState {
	public:
		using IBatState::IBatState;
		void enter() override final;
		void update(uint32_t frameIndex, double deltaTime) override final;
		void exit() override final;

	private:
		double m_DespawnTimer;
	};

	class Attack : public IBatState {
	public:
		using IBatState::IBatState;
		void enter() override final;
		void update(uint32_t frameIndex, double deltaTime) override final;
		void exit() override final;
	};

	class Despawn : public IBatState {
	public:
		using IBatState::IBatState;
		void enter() override final;
		void update(uint32_t frameIndex, double deltaTime) override final;
		void exit() override final;
	};
}