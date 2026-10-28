#include "Component/MovementComp.h"
#include "Component/TransformComp.h"
#include "GameObject/ObjectParam.h"
#include "Util/XMathHelper.h"

namespace {
	using namespace DirectX::SimpleMath;
}

MovementComp::MovementComp(GameBase* owner)
	: ComponentBase(owner)
	, m_Param(nullptr)
	, m_LimitResponse(MAKE_UNIQUE_LOG<LimitHitResponse::Stop>(this))
	, m_BHasMovementLimit(true)
	, m_BIsOnGround(false)
{
	m_Target = owner->transform();
	init();
}

void MovementComp::init()
{
	auto param = owner().param();
	if (param) {
		m_Param = param;
		setPos(m_Param->startPos());
		setRot(m_Param->startRot());
		setScale(m_Param->startScale());
		if (m_Param->minPos() == Vector3::Zero && m_Param->maxPos() == Vector3::Zero) {
			m_BHasMovementLimit = false;
		}
		m_Gravity = 9.81f * m_Param->mass();
	}
	m_Velocity = DirectX::SimpleMath::Vector3::Zero;
}

void MovementComp::preUpdate(uint32_t frameIndex, double deltaTime) {
	const float dt = static_cast<float>(deltaTime);
	Vector3 velo = m_Velocity;
	float FloorFric = 1.0f;
	float airD = 1.0f;

	if (m_Param) {
		FloorFric = m_Param->floorFriction();
		airD = m_Param->airDrag();
	}
	if (m_BIsOnGround) {
		velo.x *= FloorFric;
		velo.z *= FloorFric;
	}
	else {
		velo.x *= airD;
		velo.z *= airD;
		if (m_BIsEnableGravity) {
			velo.y -= m_Gravity * dt;
		}
	}
	setVelo(velo);
}

void MovementComp::postUpdate(uint32_t frameIndex, double deltaTime) {
	if (!m_Target) { return; }

	if (m_Velocity == DirectX::SimpleMath::Vector3::Zero) {
		return;
	}
	const float dt = static_cast<float>(deltaTime);
	auto& pos = m_Target->m_Position;
	pos += m_Velocity * dt;
	//	制限範囲
	if (!m_BHasMovementLimit || !m_Param) {// 制限なし
		m_Target->updateMatrix();
		return;
	}
	if (!m_LimitResponse) { return; }

	m_LimitResponse->onHit();
	m_Target->updateMatrix();
}

void MovementComp::setTransformFromParam()
{
	auto& o = owner();
	setPos(o.param()->startPos());
	setRot(o.param()->startRot());
	setScale(o.param()->startScale());
}

void MovementComp::setTransform(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 rot, DirectX::SimpleMath::Vector3 scale)
{
	if (m_Target->isLocked(TransformLock::PosAll) ||
		m_Target->isLocked(TransformLock::RotAll) ||
		m_Target->isLocked(TransformLock::ScaleAll)) {
		messageIsLocked();
		return;
	}

	m_Target->m_Position = pos;
	m_Target->m_Rotation = rot;
	m_Target->m_Scale = scale;
}

#pragma region Pos/Rot/Scale
void MovementComp::setPos(DirectX::SimpleMath::Vector3 pos) {
	if (m_Target->isLocked(TransformLock::PosAll)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Position = pos;
}
void MovementComp::addPos(DirectX::SimpleMath::Vector3 pos) {
	if (m_Target->isLocked(TransformLock::PosAll)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Position += pos;
}
void MovementComp::setPosX(float x) {
	if (m_Target->isLocked(TransformLock::PosX)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Position.x = x;
}
void MovementComp::setPosY(float y) {
	if (m_Target->isLocked(TransformLock::PosY)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Position.y = y;
}
void MovementComp::setPosZ(float z) {
	if (m_Target->isLocked(TransformLock::PosZ)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Position.z = z;
}
void MovementComp::addPosX(float x) {
	if (m_Target->isLocked(TransformLock::PosX)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Position.x += x;
}
void MovementComp::addPosY(float y) {
	if (m_Target->isLocked(TransformLock::PosY)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Position.y += y;
}
void MovementComp::addPosZ(float z) {
	if (m_Target->isLocked(TransformLock::PosZ)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Position.z += z;
}

void MovementComp::mulPosX(float x)
{
	if (m_Target->isLocked(TransformLock::PosX)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Position.x *= x;
}

void MovementComp::mulPosY(float y)
{
	if (m_Target->isLocked(TransformLock::PosY)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Position.y *= y;
}

void MovementComp::mulPosZ(float z)
{
	if (m_Target->isLocked(TransformLock::PosZ)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Position.z *= z;
}

void MovementComp::setRot(DirectX::SimpleMath::Vector3 rot) {
	if (m_Target->isLocked(TransformLock::RotAll)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Rotation = rot;
}
void MovementComp::addRot(DirectX::SimpleMath::Vector3 rot) {
	if (m_Target->isLocked(TransformLock::RotAll)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Rotation += rot;
}
void MovementComp::setRotX(float x) {
	if (m_Target->isLocked(TransformLock::RotX)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Rotation.x = x;
}
void MovementComp::setRotY(float y) {
	if (m_Target->isLocked(TransformLock::RotY)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Rotation.y = y;
}
void MovementComp::setRotZ(float z) {
	if (m_Target->isLocked(TransformLock::RotZ)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Rotation.z = z;
}
void MovementComp::addRotX(float x) {
	if (m_Target->isLocked(TransformLock::RotX)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Rotation.x += x;
}
void MovementComp::addRotY(float y) {
	if (m_Target->isLocked(TransformLock::RotY)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Rotation.y += y;
}
void MovementComp::addRotZ(float z) {
	if (m_Target->isLocked(TransformLock::RotZ)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Rotation.z += z;
}

void MovementComp::mulRotX(float x)
{
	if (m_Target->isLocked(TransformLock::RotX)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Rotation.x *= x;
}

void MovementComp::mulRotY(float y)
{
	if (m_Target->isLocked(TransformLock::RotY)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Rotation.y *= y;
}

void MovementComp::mulRotZ(float z)
{
	if (m_Target->isLocked(TransformLock::RotZ)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Rotation.z *= z;
}

void MovementComp::setScale(DirectX::SimpleMath::Vector3 scale) {
	if (m_Target->isLocked(TransformLock::ScaleAll)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Scale = scale;
}
void MovementComp::setUniformScale(float scale)
{
	if (m_Target->isLocked(TransformLock::ScaleAll)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Scale.x = scale;
	m_Target->m_Scale.y = scale;
	m_Target->m_Scale.z = scale;

}
void MovementComp::addScale(DirectX::SimpleMath::Vector3 scale) {
	if (m_Target->isLocked(TransformLock::ScaleAll)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Scale += scale;
}
void MovementComp::setScaleX(float x) {
	if (m_Target->isLocked(TransformLock::ScaleX)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Scale.x = x;
}
void MovementComp::setScaleY(float y) {
	if (m_Target->isLocked(TransformLock::ScaleY)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Scale.y = y;
}
void MovementComp::setScaleZ(float z) {
	if (m_Target->isLocked(TransformLock::ScaleZ)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Scale.z = z;
}
void MovementComp::addScaleX(float x) {
	if (m_Target->isLocked(TransformLock::ScaleX)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Scale.x += x;
}
void MovementComp::addScaleY(float y) {
	if (m_Target->isLocked(TransformLock::ScaleY)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Scale.y += y;
}
void MovementComp::addScaleZ(float z) {
	if (m_Target->isLocked(TransformLock::ScaleZ)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Scale.z += z;
}
void MovementComp::mulScaleX(float x)
{
	if (m_Target->isLocked(TransformLock::ScaleX)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Scale.x *= x;
}
void MovementComp::mulScaleY(float y)
{
	if (m_Target->isLocked(TransformLock::ScaleY)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Scale.y *= y;
}
void MovementComp::mulScaleZ(float z)
{
	if (m_Target->isLocked(TransformLock::ScaleZ)) {
		messageIsLocked();
		return;
	}
	m_Target->m_Scale.z *= z;
}
#pragma endregion

#pragma region Physic

DirectX::SimpleMath::Vector3 MovementComp::direction()const {
	if (XMathHelper::IsNearlyZero(m_Velocity)) {
		return DirectX::SimpleMath::Vector3::Zero;
	}
	auto temp = m_Velocity;
	temp.Normalize();
	return temp;
}


#pragma endregion


void MovementComp::lookAt(DirectX::SimpleMath::Vector3 target)
{
	Vector3 pos = m_Target->m_Position;
	Vector3 dir = target - pos;
	dir.Normalize();

	float yaw = std::atan2(dir.x, dir.z);
	float pitch = std::asin(-dir.y);
	m_Target->m_Rotation = Vector3(pitch, yaw, 0.0f);
}

void MovementComp::messageIsLocked()
{
	ELOG("Warning : MovementComp::setPos - %s is locked", owner().name().c_str());
}

void LimitHitResponse::Stop::onHit()
{
	const auto m_Param = param();
	const auto& pos = posision();
	const float& left = m_Param->minPos().x;
	const float& right = m_Param->maxPos().x;
	const float& front = m_Param->maxPos().z;
	const float& back = m_Param->minPos().z;
	const float& down = 0.0f;
	const float& top = m_Param->maxPos().y;
	if (pos.x < left) {
		m_Mover->setPosX(left);
		m_Mover->setVeloX(0.0f);
	}
	if (pos.x > right) {
		m_Mover->setPosX(right);
		m_Mover->setVeloX(0.0f);
	}
	if (pos.y < down) {
		m_Mover->setPosY(down);
		m_Mover->setVeloY(0.0f);
		setOnGround(true);
	}
	if (pos.y > top) {
		m_Mover->setPosY(top);
		m_Mover->setVeloY(0.0f);
	}
	if (pos.z > front) {
		m_Mover->setPosZ(front);
		m_Mover->setVeloZ(0.0f);
	}
	if (pos.z < back) {
		m_Mover->setPosZ(back);
		m_Mover->setVeloY(0.0f);
	}
}

void LimitHitResponse::Reflection::onHit()
{
	const auto m_Param = param();
	const auto& pos = posision();
	const float& left = m_Param->minPos().x;
	const float& right = m_Param->maxPos().x;
	const float& front = m_Param->maxPos().z;
	const float& back = m_Param->minPos().z;
	const float& down = 0.0f;
	const float& top = m_Param->maxPos().y;
	DirectX::SimpleMath::Vector3 velo = velocity() * -1.f;

	if (pos.x < left) {
		m_Mover->setPosX(left);
		m_Mover->setVeloX(velo.x);
	}
	if (pos.x > right) {
		m_Mover->setPosX(right);
		m_Mover->setVeloX(velo.x);
	}
	if (pos.y < down) {
		m_Mover->setPosY(down);
		m_Mover->setVeloY(velo.y);
		setOnGround(true);
	}
	if (pos.y > top) {
		m_Mover->setPosY(top);
		m_Mover->setVeloY(velo.y);
	}
	if (pos.z > front) {
		m_Mover->setPosZ(front);
		m_Mover->setVeloZ(velo.z);
	}
	if (pos.z < back) {
		m_Mover->setPosZ(back);
		m_Mover->setVeloZ(velo.z);
	}
}
