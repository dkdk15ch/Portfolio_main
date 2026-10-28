#pragma once
#include <cstdint>
#include "Delegate.h"
#include "Manager/GameManager.h"
#include <SimpleMath.h>

class TransformComp;
class Ball;
class Player;

class SquashGameManager : public GameManager {
public:
	static inline SquashGameManager* castedInstance() {
		if (!m_Instance) { return nullptr; }
		return static_cast<SquashGameManager*>(m_Instance);
	}

	using GameManager::GameManager;

	void Reset();
	void bind() override final;
	void update(double deltaTime) override final;

	Delegate::Singlecast<void(int)> OnChangeBallStock;
	Delegate::Singlecast<void()> OnLostLastBallStock;

	//	player
	Player* player()const { return m_Player; }
	DirectX::SimpleMath::Vector3 playerPos()const;
	//	playerBall
	DirectX::SimpleMath::Vector3 ballCurrentPos() const;
	DirectX::SimpleMath::Vector3 ballFuturePos(float posZ) const;
	bool isBallDroping()const;
	float diffZPlayerToBall()const;
	//	playerBallStock
	bool hasBallStock()const { return 0 < m_BallStock; }
	uint32_t ballStock()const { return m_BallStock; }
	void subBallStock();
	void addBallStock() { m_BallStock++; 
		OnChangeBallStock.execute(m_BallStock);
	}


	

private:
	Ball* m_PlayerBall;
	Player* m_Player;
	uint32_t m_BallStock = 3;
	TransformComp* m_BallTransform = nullptr;
	double m_BallResetInterval = 0.0;
};