#include "GameObject/Enemy/Bat.h"
#include "GameObject/Ball.h"
#include "Component/Render/ModelRenderComp.h"
#include "Component/Animator.h"
#include "Component/MovementComp.h"
#include "Manager/AssetManager.h"
#include "Manager/LevelManager.h"
#include "Manager/ColliderManager.h"
#include "Manager/ObjectParamManager.h"
#include "Util/Loader.h"
#include "GameObject/Strategy/EnemyHitResponse.h"
#include "GameObject/Strategy/EnemyBallHitResponse.h"
#include "GameObject/Param/CollisionLayerParam.h"
#include "Component/Mover/BatMovementComp.h"
#include "Component/Collider/SphereColliderComp.h"
#include <DirectXColors.h>
#include "GameObject/Param/BatParam.h"


namespace{
	const std::string baseName = "Bat";
}

Bat::Bat()
	: Enemy(baseName)
	, m_Animator(nullptr)
	
{
	//	param
	std::wstring nameW = StringToWString(name());
	std::string key = baseName;
	auto mgr = ObjectParamManager::m_Instance;
	auto param = mgr->getParam(key);
	if (!param) {
		mgr->addParam(key, key, MAKE_UNIQUE_LOG<BatParam>());
		param = mgr->getParam(key);
	}
	auto p = dynamic_cast<const BatParam*>(param);
	setParam(p);
	//	HitResponse
	m_ResponseFactory = MAKE_UNIQUE_LOG<EnemyHitResponse::Factory>();
	//	mover
	removeComponent(CompType::MovementComp);
	addComponent(MAKE_UNIQUE_LOG<BatMovementComp>(this));
	m_Mover = static_cast<BatMovementComp*>(getComponent(CompType::MovementComp));
	m_Mover->setUniformScale(p->batScale());
	m_Mover->setRot(p->batRot());
	m_Mover->setGrabity(false);
	//	Render
	addComponent(MAKE_UNIQUE_LOG<Animator>(this));
	addComponent(MAKE_UNIQUE_LOG<ModelRenderComp>(this, p->batMesh()));
	auto animator = static_cast<Animator*>(getComponent(CompType::Animator));
	auto model = animator->targetModel();
	//	animation
	Loader::animation(p->pathFlying(), model, p->loopFlying(), p->scaleFlying());
	Loader::animation(p->pathDeath(), model, p->loopDeath(), p->scaleDeath());
	Loader::animation(p->pathAttack(), model, p->loopAttack(), p->scaleAttack());
	// collider
	auto headNode = animator->findCloneNode("Head2");
	assert(headNode);
	m_ColliderObj->transform()->lock(TransformLock::LocalAll);
	m_ColliderObj->transform()->attachNode(headNode);	
	m_ColliderObj->initStaticTransform(
		p->batColliderPos(),
		p->batColliderRot(),
		p->batColliderScale());
	m_Collider->disable();
	//	EnemyBall
	auto ball = MAKE_UNIQUE_LOG<Ball>("BatBall");
	m_AttackBall = ball.get();
	LogPtr<IHitResponseFactory> factory = StaticCast_Unique<IHitResponseFactory>(MAKE_UNIQUE_LOG<EnemyBallHitResponse::Factory>());
	ball->setHitResponse(std::move(factory));
	ball->setColor(DirectX::Colors::Red);
	m_AttackBall->ignoreRayToBlock();
	auto ballCollider = dynamic_cast<SphereColliderComp*>(m_AttackBall->getComponent(CompType::ColliderComp));
	ballCollider->setLayer(CollisionTag::EnemyBall);
	auto layerParam = CollisionLayerParam::layerParam();
	ballCollider->targetLayer().setTag(layerParam->enemyBallTarget());
	ballCollider->setInitLayer(CollisionTag::EnemyBall);
	ball->disable();

	auto levelMgr = LevelManager::m_Instance;
	levelMgr->requestAddObject(std::move(ball));
}

Bat::~Bat()
{
}

void Bat::resetAttackBall()
{
	if (!m_AttackBall) { return; }
	m_AttackBall->enableNextFrame();
	m_AttackBall->ignoreRayToBlock();
	auto ballCollider = dynamic_cast<SphereColliderComp*>(m_AttackBall->getComponent(CompType::ColliderComp));
	ballCollider->targetLayer().setTag(static_cast<uint32_t>(144));
	ballCollider->setLayer(CollisionTag::EnemyBall);
	ballCollider->setInitLayer(CollisionTag::EnemyBall);

}

