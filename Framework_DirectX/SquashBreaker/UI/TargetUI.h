#pragma once
#include "GameObject/GameBase.h"

class Player;
class PlayerMovementComp;
class Ball;
struct RayDesc;
class SquashGameManager;

class TargetUI : public GameBase {
public:
	TargetUI();

	void invisible();
	void setDisableColor();
	void setEnableColor();

protected:
	void implStart() override final;
	void implupdate(uint32_t frameIndex, double deltaTime) override final;
	void implPreRender(const uint32_t frameIndex) override final;

private:
	struct RayResult {
		DirectX::SimpleMath::Vector3 pos = DirectX::SimpleMath::Vector3::Zero;
		DirectX::SimpleMath::Vector3 normal = DirectX::SimpleMath::Vector3::Zero;
	};
	RayResult rayCastToBlock(uint32_t frameIndex, double deltaTime);

	RenderCompBase* m_Render;
	MovementComp* m_Mover;
	PlayerMovementComp* m_PlayerMover;
	LogPtr<RayDesc> m_BlockCheckRay;
	SquashGameManager* m_GameManager;
	inline static float MAX_RAY_DIST = 3000.f;
	DirectX::SimpleMath::Vector3 m_PrevTargetPos;
};

