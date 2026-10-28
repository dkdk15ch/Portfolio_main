#include "UI/TargetUI.h"
#include "Component/Render/RenderCompBase.h"
#include "Component/MovementComp.h"
#include "Component/Mover/PlayerMovementComp.h"
#include "Component/Mover/BallMovementComp.h"
#include "Component/Collider/ColliderCompBase.h"
#include "Manager/ColliderManager.h"
#include "Manager/LevelManager.h"
#include "GameObject/Param/CollisionLayerParam.h"
#include "Level/Level.h"
#include <DirectXColors.h>
#include "Manager/SquashGameManager.h"
#include <iostream>
#include <format>

TargetUI::TargetUI()
	: GameBase("TargetUI")
	, m_Render(nullptr)
	, m_Mover(nullptr)
	, m_PlayerMover(nullptr)
	, m_BlockCheckRay(MAKE_UNIQUE_LOG<RayDesc>())
{
	addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(this, GPSOkey::SIMPLE_ALPHA));
	m_Render = static_cast<RenderCompBase*>(getComponent(CompType::RenderComp));
	m_Render->setPlaneMesh();
	m_Render->setTexture(L"res/Textures/Effect/Ring.dds");
	m_Render->setVisible(false);
	addComponent(MAKE_UNIQUE_LOG<MovementComp>(this));
	m_Mover = static_cast<MovementComp*>(getComponent(CompType::MovementComp));
	m_Mover->setUniformScale(50.f);
	m_Mover->setGrabity(false);
	//	レイ設定
	m_BlockCheckRay->maxDistance = 30.0f;
	auto layerParam = CollisionLayerParam::layerParam();
	m_BlockCheckRay->targetTag.setTag(layerParam->targetUIRayTarget());
	m_BlockCheckRay->radius = 0.1f;
	m_GameManager = SquashGameManager::castedInstance();
}

void TargetUI::implStart()
{
	auto level = LevelManager::m_Instance->currentLevel();
	if (!level) { return; }
	auto player = level->getObject("Player_00");
	m_PlayerMover = dynamic_cast<PlayerMovementComp*>(player->getComponent(CompType::MovementComp));
	auto StateMachine = m_PlayerMover->stateMachine();
	StateMachine->OnEnterAimState.bind([this]() {
		this->setEnableColor();
		});

	auto ball = level->getObject("Ball_00");
	auto ballMover = dynamic_cast<BallMovementComp*>(ball->getComponent(CompType::MovementComp));
	ballMover->OnObstacleHit.bind([this]() {
		this->invisible();
	});
}

void TargetUI::implupdate(uint32_t frameIndex, double deltaTime) {
	if (m_PrevTargetPos != m_PlayerMover->targetPosition()) {
		m_Mover->setPos(m_PlayerMover->targetPosition());
		auto rayResult = rayCastToBlock(frameIndex, deltaTime);
		m_Mover->setPos(rayResult.pos);
		m_Render->setVisible(true);
		//if(XMathHelper::IsNearlyEqual(rayResult.normal, DirectX::SimpleMath::Vector3::Up)
		//	|| XMathHelper::IsNearlyEqual(rayResult.normal, DirectX::SimpleMath::Vector3::Down
		//)) {
		//	m_Mover->lookAt(rayResult.pos + DirectX::SimpleMath::Vector3::Backward);
		//}
		//else {
		//	m_Mover->lookAt(rayResult.pos + rayResult.normal);
		//}
		m_Mover->lookAt(rayResult.pos + DirectX::SimpleMath::Vector3::Backward);

	}
	m_PrevTargetPos = m_PlayerMover->targetPosition();
}

void TargetUI::implPreRender(const uint32_t frameIndex) {
	//float dis = m_GameManager->diffZPlayerToBall();
	//if (dis  < 600.f) {
	//	m_Render->setVisible(true);
	//}
}

void TargetUI::invisible()
{
	m_Render->setVisible(false);
}

void TargetUI::setDisableColor() {
	setColor(DirectX::Colors::White);
}

void TargetUI::setEnableColor() {
	setColor(DirectX::Colors::Red);
}

TargetUI::RayResult TargetUI::rayCastToBlock(uint32_t frameIndex, double deltaTime)
{
	using namespace DirectX::SimpleMath;

	RayResult result;

	float dt = static_cast<float>(deltaTime);
	auto colliderMgr = ColliderManager::m_Instance;
	float dist = MAX_RAY_DIST;
	Vector3 currentStart = m_GameManager->playerPos();
	currentStart.y += 100.f;
	Vector3 targetPos = m_PlayerMover->targetPosition();
	Vector3 dir = targetPos - currentStart;
	dir.Normalize();

	HitInfo info = HitInfo{};
	m_BlockCheckRay->start = currentStart;
	m_BlockCheckRay->direction = dir;
	m_BlockCheckRay->maxDistance = dist;
	m_BlockCheckRay->radius = 10.f;
	bool bIsSkippedOnHit = true;
	if (colliderMgr->raycast(*m_BlockCheckRay, info, frameIndex, bIsSkippedOnHit)) {
		result.pos = info.hitPos + info.normal;
		result.normal = info.normal;		
		return result;
	}

	result.pos = targetPos;
	result.normal = DirectX::SimpleMath::Vector3(0.f,0.f, -1.f);
	return result;
}