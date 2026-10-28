#include "Manager/SquashGameManager.h"
#include "GameObject/Ball.h"
#include "GameObject/Player.h"
#include "Component/Mover/BallMovementComp.h"
#include "Component/TransformComp.h"
#include "Level/Level.h"
#include "Manager/LevelManager.h"


void SquashGameManager::Reset() {
	m_BallStock = 3;
}

void SquashGameManager::bind() {
	auto mgr = LevelManager::m_Instance;
	assert(mgr);

	auto level = mgr->currentLevel();
	auto ball = dynamic_cast<Ball*>(level->getObject("Ball_00"));

	if (!ball) {
		ELOG("Worning : GameManager::Bind - ball is null");
		return;
	}

	ball->mover()->OnLostBall.bind([this, ball]() {
		subBallStock();
		ball->disable();
		this->m_BallResetInterval = 0.5;
	});

	m_BallTransform = dynamic_cast<TransformComp*>(ball->getComponent(CompType::TransformComp));
	m_PlayerBall = dynamic_cast<Ball*>(level->getObject("Ball_00"));
	m_Player = dynamic_cast<Player*>(level->getObject("Player_00"));

}

void SquashGameManager::update(double deltaTime)
{
	if (m_BallResetInterval > 0.0) {
		m_BallResetInterval -= deltaTime;
		if (m_BallResetInterval <= 0.0) {
			m_BallResetInterval = 0.0;
			m_PlayerBall->enableNextFrame();
		}
	}
}

DirectX::SimpleMath::Vector3 SquashGameManager::playerPos() const
{
	if (!m_Player) { return DirectX::SimpleMath::Vector3::Zero; };
	return m_Player->transform()->position();
}

DirectX::SimpleMath::Vector3 SquashGameManager::ballCurrentPos() const
{
	if (!m_BallTransform) { return DirectX::SimpleMath::Vector3::Zero; }
	return m_BallTransform->worldPos();
}

DirectX::SimpleMath::Vector3 SquashGameManager::ballFuturePos(float posZ) const
{
	DirectX::SimpleMath::Vector3 result{FLT_MIN,FLT_MIN, FLT_MIN};
	auto mgr = LevelManager::m_Instance;
	auto level = mgr->currentLevel();
	if (!level) { return result; }
	if (!m_PlayerBall) { return result; }
	auto mover = m_PlayerBall->mover();
	if (!mover) { return result; }

	return mover->calcFuturePos(posZ, deltaTime());
}

bool SquashGameManager::isBallDroping() const
{
	if (!m_PlayerBall) { return false; }
	auto mover = m_PlayerBall->getComponent(CompType::MovementComp);
	if (!mover) { return false; }

	float veloZ = static_cast<MovementComp*>(mover)->velocity().z;
	return std::abs(veloZ) < 0.001f;
}

float SquashGameManager::diffZPlayerToBall() const
{
	if (!m_Player || !m_BallTransform) { return FLT_MAX; }
	float diff = m_Player->transform()->position().z - m_BallTransform->position().z;
	return std::abs(diff);
}

void SquashGameManager::subBallStock()
{
	if (m_BallStock == 0) {
		OnLostLastBallStock.execute();
		m_PlayerBall->disable(); 
		return;
	}
	m_BallStock--;
	OnChangeBallStock.execute(m_BallStock);
}
