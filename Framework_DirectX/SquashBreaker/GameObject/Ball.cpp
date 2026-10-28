#include "Ball.h"
#include "GameObject/PrimitiveMesh.h"
#include "GameObject/Strategy/BallHitResponse.h"
#include "GameObject/Param/CollisionLayerParam.h"
#include "Component/Collider/SphereColliderComp.h"
#include "Component/Mover/BallMovementComp.h"
#include "Component/Render/RenderCompBase.h"
#include "Manager/ColliderManager.h"
#include "Manager/ObjectParamManager.h"
#include "Manager/LevelManager.h"
#include "SafeDelete.h"
#include "Util/CollisionMath.h"
#include "Util/Loader.h"
#include <iostream>
#include <DirectXColors.h>

namespace {
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	const std::string baseName = "Ball";
}// namespace 


Ball::Ball(std::string uniqueName) 
	: GameBase(std::move(makeUniqueName(uniqueName)))
	, m_BlockCheckRay{MAKE_UNIQUE_LOG<RayDesc>()}
	, m_HitResponseFactory(MAKE_UNIQUE_LOG<BallHitResponse::Factory>())
{
	std::wstring nameW = StringToWString(name());
	auto mgr = ObjectParamManager::m_Instance;
	auto param = mgr->getParam(baseName);
	if (!param) {
		mgr->addParam(baseName, baseName, MAKE_UNIQUE_LOG<BallParam>());
		param = mgr->getParam(baseName);
	}
	auto ballParam = dynamic_cast<const BallParam*>(param);
	setParam(ballParam);
	if (!ballParam->m_BIsInitSceneBoundingData) {
		mgr->updateSceneBindingData(baseName);
	}
	m_TimeScale = ballParam->DEFAULT_TIME_SCALE;
	//	mover
	addComponent(MAKE_UNIQUE_LOG<BallMovementComp>(this));
	m_Mover = dynamic_cast<BallMovementComp*>(getComponent(CompType::MovementComp));
	// collider
	addComponent(MAKE_UNIQUE_LOG<SphereColliderComp>(this));
	m_Collider = static_cast<SphereColliderComp*>(getComponent(CompType::ColliderComp));
	auto layerParam = CollisionLayerParam::layerParam();
	m_Collider->setLayer(CollisionTag::Ball);
	m_Collider->targetLayer().setTag(layerParam->ballTarget());
	checkRayToBlock();
	m_Collider->show();
	//	render設定
	addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(this));
	auto render = static_cast<RenderCompBase*>(getComponent(CompType::RenderComp));
	auto priMesh = PrimitiveMesh::m_Instance;
	assert(render);
	render->setMesh(priMesh->sphereMesh());
	Loader::texture(
		ballParam->TEXT_PATH.c_str(),
		render->material(), 
		nameW);
	//	レイ設定
	m_BlockCheckRay->maxDistance = 30.0f;
	m_BlockCheckRay->radius = transform()->scale().x;
	m_BlockCheckRay->targetTag.addTag(CollisionTag::Block);	
	//	影
	auto shadow = MAKE_UNIQUE_LOG<GameBase>(name() + std::string("_Shadow"));
	m_Shadow = shadow.get();
	//	render
	shadow->addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(shadow.get()));
	auto shadowRender = static_cast<RenderCompBase*>(shadow->getComponent(CompType::RenderComp));
	shadowRender->setSphereMesh();
	shadow->setColor(DirectX::Colors::Black);
	// mover
	shadow->addComponent(MAKE_UNIQUE_LOG<MovementComp>(shadow.get()));
	m_ShadowMover = static_cast<MovementComp*>(shadow->getComponent(CompType::MovementComp));
	m_ShadowMover->setPosY(0.f);
	m_ShadowMover->setUniformScale(transform()->scale().x);
	m_ShadowMover->setScaleY(0.1f);
	m_ShadowMover->setGrabity(false);
	shadow->transform()->lock(TransformLock::PosY);
	//	登録
	auto levelMgr = LevelManager::m_Instance;
	levelMgr->requestAddObject(std::move(shadow));
}

Ball::~Ball()
{
	
}

void Ball::term()
{
	GameBase::term();
}

void Ball::checkRayToBlock()
{
	m_BlockCheckRay->targetTag.addTag(CollisionTag::Block);
}

void Ball::ignoreRayToBlock()
{
	m_BlockCheckRay->targetTag.removeTag(CollisionTag::Block);
}

void Ball::enable()
{
	GameBase::enable();
	m_Shadow->enable();
}

void Ball::disable()
{
	GameBase::disable();
	m_Shadow->disable();
}

void Ball::implupdate(uint32_t frameIndex, double deltaTime)
{
	GameBase::implupdate(frameIndex, deltaTime);
	rayCastToBlock(frameIndex, deltaTime);
	//	影
	auto pos = transform()->worldPos();
	m_ShadowMover->setPosX(pos.x);
	m_ShadowMover->setPosZ(pos.z);
}

void Ball::implOnHit(const HitInfo& info)
{
	const auto& layer = info.other->layer();

	//	レイヤーごとの応答
	assert(m_HitResponseFactory);
	auto hitRes = m_HitResponseFactory->create(layer);
	if (hitRes) {
		hitRes->onHit(*this, info);
		SafeDelete(hitRes);
	}
}

void Ball::rayCastToBlock(
	uint32_t frameIndex, double deltaTime)
{
	float dt = static_cast<float>(deltaTime);
	auto colliderMgr = ColliderManager::m_Instance;

	float totalDist = (m_Mover->velocity() * dt).Length();
	float halfDist = totalDist * 0.5f;
	Vector3 currentStart = transform()->position();
	Vector3 dir = m_Mover->direction();

	//	精度が足りなかったので、2回チェック
	// 1回目：前半分のチェック
	HitInfo info = HitInfo{};
	info.self = m_Collider;
	m_BlockCheckRay->start = currentStart;
	m_BlockCheckRay->direction = dir;
	m_BlockCheckRay->maxDistance = halfDist;
	if (colliderMgr->raycast(*m_BlockCheckRay, info, frameIndex)) {
		transform()->updateMatrix();
		//m_InstanceData[0].m_Transform->updateMatrix();
		return; 
	}
	// 2回目：後半分のチェック（始点を1回目の終点にずらす）
	m_BlockCheckRay->start = currentStart + (dir * halfDist);
	m_BlockCheckRay->maxDistance = halfDist;
	if (colliderMgr->raycast(*m_BlockCheckRay, info, frameIndex)) {
		transform()->updateMatrix();
		return;
	}
}




