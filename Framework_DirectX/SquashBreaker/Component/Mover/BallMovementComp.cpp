#include "Component/Mover/BallMovementComp.h"
#include "Component/Collider/ColliderCompBase.h"
#include "Level/Level.h"
#include "Util/CollisionMath.h"
#include "GameObject/Ball.h"
#include "GameObject/Param/BallParam.h"
#include "Manager/LevelManager.h"
#include "Manager/CameraManager.h"
#include "GameObject/Camera.h"
#include "Strategy/IHitResponse.h"
#include "SafeDelete.h"

namespace {
	float CAMERA_MIN_DIST;
	float CAMERA_MAX_DIST;
}

BallMovementComp::BallMovementComp(GameBase* owner)
	: MovementComp(owner)
	, m_BIsBounded(false)
	, m_BIsLaunching(false)
	, m_ReseInterval(0.0)
{
	auto ball = dynamic_cast<Ball*>(owner);
	m_Param = ball->param();
	m_BHasMovementLimit = false;

	const auto& cam = CameraManager::m_Instance->currentCamera();
	float startZ = m_Param->startPos().z;
	CAMERA_MIN_DIST = std::abs(cam.followOffset().z) - startZ;
	CAMERA_MAX_DIST = m_Param->maxPos().z;
}

BallMovementComp::~BallMovementComp()
{
}

void BallMovementComp::init()
{
	MovementComp::init();
	m_BIsLaunching = false;
	m_BIsBounded = false;
	OnLostBall.execute();
	m_ReseInterval = 1.0;
}

void BallMovementComp::update(uint32_t frameIndex, double deltaTime)
{
	//	物理挙動解決
	updatePhysics(deltaTime);
	//	壁の判定解決
	resolveWallCollision();
	//	遠近感の補正
	updateVisualScale();

	MovementComp::update(frameIndex, deltaTime);
}

void BallMovementComp::launchToTarget(
	DirectX::SimpleMath::Vector3 targetPos, float V0) 
{
	auto& pos = owner().transform()->position();
	auto dir = (targetPos - pos);
	dir.Normalize();
	m_Velocity = dir * V0;
	m_BIsLaunching = true;
	m_BIsBounded = false;
}

void BallMovementComp::hitBlock(int face, DirectX::SimpleMath::Vector3 hitPos)
{
	auto p = static_cast<const BallParam*>(m_Param);

	if (m_BIsLaunching) {m_BIsLaunching = false;}

	//	応答 速度を反転
	//	左右は velo.x
	//	上下は velo.y
	//	前後 はvelo.z
	float cor = p->COR_BLOCK;
	if (m_Velocity.Length() < p->SPEED_MIN) {
		cor = 1.0f;
	}

	using namespace CollisionMath;
	switch (face)
	{
	case Face::RIGHT:
	case Face::LEFT:
	{
		m_Velocity.x *= -cor;
		m_Velocity.y *= p->FRONT_HIT_VERTICAL_DAMPING;
		break;
	}
	case Face::TOP:
	case Face::BOTTOM: {
		m_Velocity.y *= -cor;
		m_Velocity.y *= p->FRONT_HIT_VERTICAL_DAMPING;
		break;
	}
	case Face::FRONT:
	case Face::BACK: {
		m_Velocity.z *= -cor;
		m_Velocity.y *= p->FRONT_HIT_VERTICAL_DAMPING;
		break;
	}
	}
	setPos(hitPos);
}

DirectX::SimpleMath::Vector3 BallMovementComp::calcFuturePos(float targetPosZ, double deltaTime)
{
	DirectX::SimpleMath::Vector3 velocity = m_Velocity;
	//	手前に進んでるときの計算
	if (velocity.z > 0.f) {
		return DirectX::SimpleMath::Vector3{ FLT_MIN,FLT_MIN, FLT_MIN };
	}


	const auto& ball = dynamic_cast<Ball*>(&owner());
	const auto p = static_cast<const BallParam*>(m_Param);

	DirectX::SimpleMath::Vector3 pos = ball->transform()->worldPos();
	bool isLaunching = m_BIsLaunching;

	const float dt = static_cast<float>(deltaTime);
	float elapsed = 0.0f;
	const float MAX_CALC_TIME = 3.0f;

	const float left = p->MIN_POS.x;
	const float right = p->MAX_POS.x;
	const float down = p->MIN_POS.y;
	const float top = p->MAX_POS.y;
	const float back = p->MIN_POS.z;
	const float front = p->MAX_POS.z;

	while (elapsed < MAX_CALC_TIME)
	{
		DirectX::SimpleMath::Vector3 prevPos = pos;
		pos += velocity * dt;

		if (pos.x < left || pos.x > right) {
			pos.x = std::clamp(pos.x, left, right);
			velocity.x *= -p->COR_BLOCK;
		}
		if (pos.y < down || pos.y > top) {
			pos.y = std::clamp(pos.y, down, top);
			velocity.y *= -p->COR_BLOCK;
		}
		if (pos.z < back || pos.z > front) {
			pos.z = std::clamp(pos.z, back, front);
			velocity.z *= -p->COR_BLOCK;
		}

		bool crossedZ = (prevPos.z <= targetPosZ && pos.z >= targetPosZ) ||
			(prevPos.z >= targetPosZ && pos.z <= targetPosZ);

		if (crossedZ) {
			break;
		}

		elapsed += dt;
	}

	return pos;
}


void BallMovementComp::updatePhysics(double deltaTime)
{
	const float dt = static_cast<float>(deltaTime);
	auto p = static_cast<const BallParam*>(m_Param);
	const auto& pos = owner().transform()->position();
	
	//	重力無効
	//	打ち返した直後(ボールの回転による揚力てきな感じ)
	//	バウンド低くなりすぎないように
	if (m_BIsLaunching ||
		pos.y <= p->IGNORE_GRAVITY_HEIGHT) {
		setGrabity(false);
	}
	else {
		setGrabity(true);
	}

}

void BallMovementComp::hitWall(int Face)
{
	auto p = static_cast<const BallParam*>(m_Param);
	auto& ow = owner();
	auto& pos = ow.transform()->position();

	if (m_BIsLaunching) {
		m_BIsLaunching = false;
	}

	auto ball = dynamic_cast<Ball*>(&ow);
	auto hitRes = ball->hitResFactory()->create(CollisionTag::Static);
	HitInfo info;
	info.face = static_cast<CollisionMath::Face>(Face);
	if (hitRes) {
		hitRes->onHit(ow, info);
		SafeDelete(hitRes);
	}
}

void BallMovementComp::resolveWallCollision()
{
	auto p = static_cast<const BallParam*>(m_Param);
	auto& pos = owner().transform()->position();

	const float left = p->MIN_POS.x;
	const float right = p->MAX_POS.x;
	const float back = p->MIN_POS.z;
	const float front = p->MAX_POS.z;
	const float down = p->MIN_POS.y;
	const float top = p->MAX_POS.y;

	using namespace CollisionMath;
	if (pos.x < left || pos.x > right) {
		setPosX(std::clamp(pos.x, left, right));
		hitWall(Face::RIGHT);
	}

	if (pos.y < down) {
		setPosY(down);
		hitWall(Face::BOTTOM);
	}

	if (pos.y > top) {
		setPosY(top);
		hitWall(Face::TOP);
	}

	if (pos.z > front) {
		setPosZ(front);
		hitWall(Face::FRONT);
	}

	if (pos.z < back) {
		setPosZ(back);
		hitWall(Face::BACK);
	}
}

void BallMovementComp::updateVisualScale()
{
	//auto p = static_cast<const BallParam*>(m_Param);
	//auto& pos = owner().transform()->position();
	//const auto& cam = CameraManager::instance->currentCamera();
	//const float dist = std::abs(pos.z - cam.position().z);
	//float t = std::clamp((dist - CAMERA_MIN_DIST) / 
	//(CAMERA_MAX_DIST - CAMERA_MIN_DIST), 0.0f, 1.0f);

	//float minScale = 0.8f;
	//float maxScale = 1.0f;
	//float scaleFactor = maxScale + t * (minScale - maxScale);

	//setUniformScale(p->START_SCALE.x * scaleFactor);
}






