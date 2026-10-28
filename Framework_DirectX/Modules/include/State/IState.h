#pragma once
#include <cstdint>
#include <DirectXMath.h>
#include "PointerHelper.h"
#include <unordered_map>

class GameBase;
class StateMachineBase;

class IState {
public:
    IState(StateMachineBase* stateMachine) : m_StateMachine(stateMachine) {}
	virtual ~IState() = default;

    virtual void enter() = 0;
    virtual void update(uint32_t frameIndex, double deltaTime) = 0;
    virtual void exit() {}
    virtual void Reset() {}

protected:
    StateMachineBase* m_StateMachine = nullptr;
    GameBase* owner()const;
};

class StateMachineBase {
public:
    StateMachineBase(GameBase* owner) : m_Owner(owner){}
    virtual ~StateMachineBase() { m_StateList.clear(); }

    void changeState(const std::string& nextState) {
        auto it = m_StateList.find(nextState); 
        if (it != m_StateList.end()) {
            if (m_CurrentState) {
                m_CurrentState->exit();
            }
            m_CurrentState = it->second.get();
            m_CurrentState->enter();

            m_CurrentType = nextState;
        }
    }

    void update(uint32_t frameIndex, double deltaTime) {
        if (!m_CurrentState) { return; }
        m_CurrentState->update(frameIndex, deltaTime);
    }

    GameBase* owner()const { return m_Owner; }
    const std::string& currentType()const { return m_CurrentType; }
    bool isCurrent(const std::string& type) {
        return type == m_CurrentType;
    }


protected:
    std::unordered_map<std::string, LogPtr<IState>> m_StateList;
    IState* m_CurrentState = nullptr;
    std::string m_CurrentType;
    GameBase* m_Owner;
};

inline GameBase* IState::owner() const {
    return m_StateMachine->owner();
}