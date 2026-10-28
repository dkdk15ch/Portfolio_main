#include "Component/Mover/PlayerMovementComp.h"
#include "GameObject/Player.h"
#include "GameObject/ObjectParam.h"
#include "GameObject/Param/PlayerParam.h"
#include "GameObject/GameBase.h"
#include "InOutput/Pad.h"
#include "InOutput/Audio.h"
#include "Tag.h"
#include "Component/Animator.h"
#include "Component/Collider/BoxColliderComp.h"
#include "GameObject/State/PlayerState.h"
#include "Manager/SquashGameManager.h"

#ifdef _DEBUG
#include "Component/Render/RenderCompBase.h"
#include "ConstantBuffer.h"
#include <DirectXColors.h>
#endif // _DEBUG

namespace {
	CollisionTag DEFAULT_RACKET_TAG;
	constexpr float ASSIST_ENABLE_DISTANCE = 500 * 500;
}

PlayerMovementComp::PlayerMovementComp(GameBase* owner)
	: MovementComp(owner)
	, m_TargetPosition{}
	, m_RacketPos{}
	, m_SwingType(SwingType::FORE)
	, m_StateMachine(MAKE_UNIQUE_LOG<PlayerStateMachine>(owner, this))
{
	auto player = dynamic_cast<Player*>(owner);
	assert(player && "PlayerMoveComp::PlayerMovementComp");

	m_Param = player->param();
	DEFAULT_RACKET_TAG = CollisionTag::Racket;


}

PlayerMovementComp::~PlayerMovementComp()
{
}

void PlayerMovementComp::start()
{
	auto animator = dynamic_cast<Animator*>(owner().getComponent(CompType::Animator));
	animator->setFlipX(true);
	m_StateMachine->init();
}

void PlayerMovementComp::init()
{
	m_TargetPosition = DirectX::SimpleMath::Vector3::Zero;
	m_RacketPos = DirectX::SimpleMath::Vector3::Zero;
}

void PlayerMovementComp::preUpdate(uint32_t frameIndex, double deltaTime)
{
	MovementComp::preUpdate(frameIndex, deltaTime);

	if (m_SpeedBouns <= 1.0f) { return; }

	m_BounsCounter -= deltaTime;
	if (m_BounsCounter <= 0) {
		m_BounsCounter = 0.0;
		m_SpeedBouns = 1.0f;
		owner().setColor(DirectX::Colors::White);
	}
}

void PlayerMovementComp::update(uint32_t frameIndex, double deltaTime)
{
	if (m_StateMachine->currentType().empty()) {
		m_StateMachine->changeState(PlayerStateMachine::IDLE);
	}
	m_StateMachine->update(frameIndex, deltaTime);

	
}

void PlayerMovementComp::postUpdate(uint32_t frameIndex, double deltaTime)
{
	MovementComp::postUpdate(frameIndex, deltaTime);
}

void PlayerMovementComp::setMoveBounsValue(float multValu)
{
	if (multValu <= 1.0f) { return; }
	m_SpeedBouns = multValu;
	m_BounsCounter = BOUNS_TIME;
	owner().setColor(DirectX::Colors::DarkOrange);
}


void PlayerMovementComp::move(double deltaTime)
{
	auto p = static_cast<const PlayerParam*>(m_Param);
	const float dt = static_cast<float>(deltaTime);
	const auto& pos = owner().transform()->position();
	
	auto pad = Pad::m_Instance;
	const float speedX = p->MOVE_SPEED_X * m_SpeedBouns;
	const float speedZ = p->MOVE_SPEED_Z * m_SpeedBouns;
	const float keyboardSpeedX = speedX * 0.8f * m_SpeedBouns;
	const float keyboardSpeedZ = speedZ * 0.8f * m_SpeedBouns;

	if(pad->hasPadInput())
	{//	コントローラ
		setVeloX(pad->m_GamepadState.thumbSticks.leftX * speedX);
		setVeloZ(pad->m_GamepadState.thumbSticks.leftY * speedZ);
	}

	{//	キーボード入力
		if (pad->m_KeyboardState.A) {
			setVeloX(-keyboardSpeedX);
		}
		if (pad->m_KeyboardState.D) {
			setVeloX(keyboardSpeedX);
		}
		if (pad->m_KeyboardState.W) {
			setVeloZ(keyboardSpeedZ);
		}
		if (pad->m_KeyboardState.S) {
			setVeloZ(-keyboardSpeedZ);
		}
	}
}

bool PlayerMovementComp::assistMove(float futureBallPosX)
{
	const auto& playerPos = m_Target->worldPos();
	auto mgr = SquashGameManager::castedInstance();
	const auto& ballPos = mgr->ballCurrentPos();
	float dis = DirectX::SimpleMath::Vector3::DistanceSquared(ballPos, playerPos);
	if (dis > ASSIST_ENABLE_DISTANCE) { return false; }

	//	x方向決定
	float offsetX = -80.f;
	if (m_SwingType == SwingType::BACK) {
		offsetX *= -1.f;
	}  
	float targetPosX = futureBallPosX + offsetX;
	float diffX = targetPosX - playerPos.x;
	//	一定の距離まで近づく
	const float threshold = 3.f;
	if (std::abs(diffX) < threshold)
	{
		setVeloX(0.0f);
		return true;
	}	
	auto p = static_cast<const PlayerParam*>(m_Param);
	const float speedX = p->MOVE_SPEED_X * 0.5f;
	float dir = (diffX > 0.0f) ? 1.0f : -1.0f;
	setVeloX(dir * speedX);
	return false;
}

void PlayerMovementComp::controllAim(double deltaTime)
{
	using namespace DirectX::SimpleMath;
	auto p = static_cast<const PlayerParam*>(m_Param);
	auto pad = Pad::m_Instance;
	const float speed = p->AIM_SPEED * static_cast<float>(deltaTime);
	Vector2 moveDir = Vector2::Zero;

	{//	左スティック
		moveDir.x += pad->m_GamepadState.thumbSticks.leftX;
		moveDir.y += pad->m_GamepadState.thumbSticks.leftY;
	}
	{//	キーボード
		if (pad->m_KeyboardState.W) moveDir.y += 1.0f;
		if (pad->m_KeyboardState.S) moveDir.y -= 1.0f;
		if (pad->m_KeyboardState.A) moveDir.x -= 1.0f;
		if (pad->m_KeyboardState.D) moveDir.x += 1.0f;
	}

	if (moveDir.LengthSquared() > 1.0f)
	{
		moveDir.Normalize();
	}
	m_TargetPosition.x += moveDir.x * speed;
	m_TargetPosition.y += moveDir.y * speed;


	m_TargetPosition.x = std::clamp(m_TargetPosition.x, p->AIM_LIMIT_LEFT, p->AIM_LIMIT_RIGHT);
	m_TargetPosition.y = std::clamp(m_TargetPosition.y, p->AIM_LIMIT_DOWN, p->AIM_LIMIT_TOP);
}

void PlayerMovementComp::aimStart()
{
	//	押した瞬間に、エイムの初期位置をプレイヤー正面へ
	//	何も入力していなければまっすぐ前へ飛ぶように
	auto p = static_cast<const PlayerParam*>(m_Param);
	const auto& pos = owner().transform()->position();
	m_TargetPosition = p->INIT_TARGET_POS;
	m_TargetPosition.x = pos.x;
}

void PlayerMovementComp::aimEnd()
{
	auto p = static_cast<const PlayerParam*>(m_Param);
	auto player = dynamic_cast<Player*>(&owner());
	//	当たり判定有効化
	auto& collider = player->m_RacketCollider;
	collider->setLayer(CollisionTag::Racket);
	Audio::playSE(p->pathSE_Shot(), p->volumeShot());
}

void PlayerMovementComp::changeSwing()
{
	auto pad = Pad::m_Instance;
	if (pad->m_GamepadState.buttons.leftShoulder || 
		pad->m_GamepadState.buttons.rightShoulder ||
		pad->m_KeyTracker.IsKeyReleased(DirectX::Keyboard::Keys::I)
		) {
		//SetSwingType(SwingType::SMASH);

		//auto player = dynamic_cast<Player*>(&owner());
		//auto p = static_cast<const PlayerParam*>(m_Param);
		//player->m_RacketMover->setPos(p->RACKET_RELATIVEPOS_TOP);
		//auto animator = dynamic_cast<Animator*>(player->getComponent(CompType::Animator));
		//animator->setFlipX(false);
	}
	else if (pad->m_GamepadState.triggers.right ||
		pad->m_KeyTracker.IsKeyReleased(DirectX::Keyboard::Keys::L)) {
		setSwingType(SwingType::FORE);
		auto player = dynamic_cast<Player*>(&owner());
		auto p = static_cast<const PlayerParam*>(m_Param);
		
		auto animator = dynamic_cast<Animator*>(player->getComponent(CompType::Animator));
		animator->setFlipX(true);
		player->m_RacketMover->setPos(p->RACKET_RELATIVEPOS_RIGHT);
	}
	else if (pad->m_GamepadState.triggers.left ||
		pad->m_KeyTracker.IsKeyReleased(DirectX::Keyboard::Keys::J)) {
		setSwingType(SwingType::BACK);		
		auto player = dynamic_cast<Player*>(&owner());
		auto p = static_cast<const PlayerParam*>(m_Param);
		
		auto animator = dynamic_cast<Animator*>(player->getComponent(CompType::Animator));
		animator->setFlipX(false);
		player->m_RacketMover->setPos(p->RACKET_RELATIVEPOS_LEFT);
	}
}

void PlayerMovementComp::jump()
{
	if (!m_BIsOnGround) { return; }

	auto p = static_cast<const PlayerParam*>(m_Param);
	setVeloY(p->JUMP_SPEED_Y);
	m_BIsOnGround = false;
}

void PlayerMovementComp::debugColliderChangeColor(DirectX::XMVECTOR color)
{
#ifdef _DEBUG
	auto player = dynamic_cast<Player*>(&owner());
	auto racket = &player->m_RacketCollider->owner();
	if (!racket) { return; }
	auto ptr = racket->getInstanceBuffer(0);
	if (!ptr) { return; }
	ptr->setColor(color);
	ptr = racket->getInstanceBuffer(1);
	if (!ptr) { return; }
	ptr->setColor(color);
#endif
}



