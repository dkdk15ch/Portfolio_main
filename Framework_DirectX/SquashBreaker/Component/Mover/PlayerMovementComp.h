#pragma once
#include "Component/MovementComp.h"
#include "Component/ComponentBase.h"
#include "GameObject/State/PlayerState.h"

namespace PlayerHitResponse { 
	class Block;
	class Wall;
	class Ball; 
}
class PlayerStateMachine;

namespace SwingType {
	enum {
		FORE = 0,
		BACK = 1,
		SMASH = 2,
	};
}

class PlayerMovementComp : public MovementComp
{
	friend class IPlayerState;
	friend class PlayerHitResponse::Block;
	friend class PlayerHitResponse::Wall;
	friend class PlayerHitResponse::Ball;
public:
	PlayerMovementComp(GameBase* owner);
	virtual ~PlayerMovementComp();

	void start()override final;
	void init()override final;

	void preUpdate(uint32_t frameIndex, double deltaTime) override final;
	void update(uint32_t frameIndex, double deltaTime) override final;

	void postUpdate(uint32_t frameIndex, double deltaTime) override final;

	const DirectX::SimpleMath::Vector3& targetPosition()const { return m_TargetPosition; }

	int swingType()const { return m_SwingType; }
	void setSwingType(int type) { m_SwingType = type; }
	void setMoveBounsValue(float multValu);

	PlayerStateMachine* stateMachine() const{ return m_StateMachine.get(); }

private:
	LogPtr<PlayerStateMachine> m_StateMachine;
	int m_SwingType;
	float m_SpeedBouns = 1.0f;
	static inline double BOUNS_TIME = 0.5;
	double m_BounsCounter = -1.0;

	//	照準関連
	DirectX::SimpleMath::Vector3 m_TargetPosition;
	//	ラケット関連
	DirectX::SimpleMath::Vector3 m_RacketPos;

	void move(double deltaTime);
	/**
	 * @brief			エイム時にボールの位置へ補助移動
	 * @param ballPosX	目標位置
	 * @return			移動完了してればtrue 
	 */
	bool assistMove(float targetPosX);

	void controllAim(double deltaTime);
	void aimStart();
	void aimEnd();
	void changeSwing();
	void jump();

	void debugColliderChangeColor(DirectX::XMVECTOR color);
};

