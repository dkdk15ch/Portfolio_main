#include "Component/Animator.h"
#include "Component/Collider/BoxColliderComp.h"
#include "Component/Collider/ColliderCompBase.h"
#include "Component/MovementComp.h"
#include "Component/Mover/BallMovementComp.h"
#include "Component/Mover/PlayerMovementComp.h"
#include "Component/Render/ModelRenderComp.h"
#include "Component/Render/RenderCompBase.h"
#include "GameObject/Ball.h"
#include "GameObject/SphereObject.h"
#include "GameObject/Param/CollisionLayerParam.h"
#include "GameObject/Player.h"
#include "GameObject/Strategy/PlayerHitResponse.h"
#include "GameObject/State/PlayerState.h"
#include "Level/Level.h"
#include "Manager/LevelManager.h"
#include "Manager/ObjectParamManager.h"
#include "Manager/SquashGameManager.h"
#include "Manager/ColliderManager.h"
#include "SafeDelete.h"
#include "Util/Loader.h"
#include <DirectXColors.h>
#include "UI/TargetUI.h"
#include "InOutput/Audio.h"

namespace {
	const std::string baseName = "Player";
	const double HIT_EFFECT_TIME = 0.5;
}

namespace PlayersObjct {
	class Racket : public GameBase {
	public:
		Racket(Player* parent, ModelNode* RightHand)
			: GameBase(parent->name() + std::string("_Racket"))
			, m_Effect(nullptr)
		{
			setParent(parent);
			m_Player = parent;
			auto p = static_cast<const PlayerParam*>(m_Player->param());
			//	collider
			addComponent(MAKE_UNIQUE_LOG<BoxColliderComp>(this, ColliderCompBase::Shape::AABB));
			addComponent(MAKE_UNIQUE_LOG<MovementComp>(this));
			auto& mover = parent->m_RacketMover;
			mover = static_cast<MovementComp*>(getComponent(CompType::MovementComp));
			mover->setGrabity(false);
			mover->setPos(p->racketRelativePosRight());
			mover->setScale(p->racketScale());
			parent->m_RacketCollider = dynamic_cast<BoxColliderComp*>
				(getComponent(CompType::ColliderComp));
			parent->m_RacketCollider->setLayer(CollisionTag::disable);
			auto layerParam = CollisionLayerParam::layerParam();
			parent->m_RacketCollider->targetLayer().setTag(layerParam->racketTarget());
			parent->m_RacketCollider->show();
			//	Render
			//	別オブジェクトで作成
			auto renderObj = MAKE_UNIQUE_LOG<GameBase>(m_Name + "Mesh");
			renderObj->setParent(parent);
			renderObj->transform()->attachNode(RightHand);
			renderObj->addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(renderObj.get(), GPSOkey::SIMPLE));
			auto racketRender = static_cast<RenderCompBase*>(renderObj->getComponent(CompType::RenderComp));
			racketRender->setMeshFromPath(p->racketMeshPath());
			racketRender->setTexture(p->racketTextPath());
			renderObj->addComponent(MAKE_UNIQUE_LOG<MovementComp>(renderObj.get()));
			m_Mover = static_cast<MovementComp*>(renderObj->getComponent(CompType::MovementComp));
			m_Mover->setGrabity(false);
			m_Mover->setUniformScale(p->racketOffsetScale());
			m_Mover->setPosY(p->racketOffsetPos().y);
			m_Mover->setPosZ(p->racketOffsetPos().z);
			m_Mover->setRotX(p->racketOffsetRot().x);
			m_Mover->setRotY(p->racketOffsetRot().y);
			//	ガット
			auto gut = MAKE_UNIQUE_LOG<GameBase>("Gut");
			gut->setParent(renderObj.get());
			gut->addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(gut.get(), GPSOkey::SIMPLE_ALPHA));
			auto gatRender = static_cast<RenderCompBase*>(gut->getComponent(CompType::RenderComp));
			gatRender->setPlaneMesh();
			gatRender->setTexture(p->gutPath(), DirectX::XMFLOAT2(p->gutTilingX(), p->gutTilingY()));
			gut->initStaticTransform(
				p->gutPos(),
				p->gutRot(),
				p->gutScale()
			);
			//	effect
			auto effectObj = MAKE_UNIQUE_LOG<GameBase>("RacketEffect");
			effectObj->setParent(this);
			effectObj->addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(effectObj.get(), GPSOkey::SIMPLE_ALPHA));
			m_Effect = static_cast<RenderCompBase*>(effectObj->getComponent(CompType::RenderComp));
			m_Effect->setPlaneMesh();
			m_Effect->setTexture(p->effectTextPath());
			m_Effect->setVisible(false);

			auto levelMgr = LevelManager::m_Instance;
			levelMgr->requestAddObject(std::move(renderObj));
			levelMgr->requestAddObject(std::move(gut));
			levelMgr->requestAddObject(std::move(effectObj));
		}
		virtual ~Racket() = default;

		void implPreupdate(uint32_t frameIndex, double deltaTime) override final {
			const auto layer = m_Player->m_RacketCollider->layer();
			if (layer != CollisionTag::disable) {
				m_Player->m_RacketCollider->setLayer(CollisionTag::disable);
			}

			if (m_EffectShowTime < 0) {
				return;
			}
			m_EffectShowTime += deltaTime;

			auto p = static_cast<const PlayerParam*>(m_Player->param());
			if (m_EffectShowTime >= p->showStart()) {
				m_Effect->setVisible(true);
			}
			if (m_EffectShowTime >= p->showEnd()) {
				m_Effect->setVisible(false);
				m_EffectShowTime = -1.0;
			}
		}
		
		void implOnHit(const HitInfo& info)override final {
			auto ball = dynamic_cast<Ball*>(&info.other->owner());
			assert(ball && "Player::implOnHit");
			auto p = static_cast<const PlayerParam*>(m_Player->param());
			auto mover = ball->mover();

			float lunchForce = p->launchForce();
			if (float posZ = m_Player->transform()->worldPos().z >= p->launchCorrectionStartZ()) {
				float wallZ = LevelManager::m_Instance->currentLevel()->wallFront();
				float ratio = 1 - (posZ / wallZ);
				ratio = std::clamp(ratio, p->launchMinRatio(), 1.0f);
				lunchForce *= ratio;
			}

			mover->launchToTarget(
				m_Player->m_Mover->targetPosition(),
				lunchForce
			);

			if (m_Effect) {
				m_EffectShowTime = 0.0;
			}
		}

		Player* m_Player;
		RenderCompBase* m_Effect;
		double m_EffectShowTime = -1.0;
		static inline double SHOW_START = 0.8;
		static inline double SHOW_END = 1.17;
		MovementComp* m_Mover;
	};

	class BodyCollider : public GameBase {
	public:
		BodyCollider(Player* parent, const PlayerParam* param, ModelNode* spine) 
			: GameBase(parent->name() + "_BodyCollider")
			, m_ResponseFactory(MAKE_UNIQUE_LOG<PlayerHitResponse::Factory>())
		{
			setParent(parent);
			addComponent(MAKE_UNIQUE_LOG<BoxColliderComp>(this, ColliderCompBase::Shape::AABB));
			auto& t = *this->transform();
			t.unLock(TransformLock::RotAll);
			t.lock(TransformLock::LocalAll);
			t.attachNode(spine);
			initStaticTransform(
				param->bodyPos(),
				DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f),
				param->bodyScale());
			auto collider = static_cast<BoxColliderComp*>(getComponent(CompType::ColliderComp));
			collider->show();
			auto layerParam = CollisionLayerParam::layerParam();
			collider->setLayer(CollisionTag::Player);
			collider->targetLayer().setTag(layerParam->playerTarget());
		}
		void implOnHit(const HitInfo& info)override final {
			const auto& layer = info.other->layer();
			auto hitRes = m_ResponseFactory->create(layer);
			if (hitRes) {
				hitRes->onHit(*this, info);
				SafeDelete(hitRes);
			}
		}
		LogPtr<IHitResponseFactory> m_ResponseFactory;
	};
	
	class BallCase : public GameBase {
	public:
		BallCase(Player* parent, ModelNode* spine)
			: GameBase(parent->name() + std::string("_BallCase")) {
			setParent(parent);
			m_Param = static_cast<const PlayerParam*>(parent->param());
			auto& p = m_Param;
			addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(this, GPSOkey::SIMPLE_ALPHA));
			auto render = static_cast<RenderCompBase*>(getComponent(CompType::RenderComp));
			render->setMeshFromPath(p->ballCaseMesh());
			render->setTexture(p->ballCaseText());
			auto& t = *this->transform();
			t.attachNode(spine);
			initStaticTransform(
				p->ballCasePos(),
				DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f),
				p->ballCaseScale());

			auto mgr = SquashGameManager::castedInstance();
			assert(mgr);

			mgr->OnChangeBallStock.bind([this](int newStock) {
				ChangeBall(newStock);
				});

		}

		void implStart()override final {
			auto p = static_cast<const PlayerParam*>(parent()->param());

			auto mgr = SquashGameManager::castedInstance();
			assert(mgr);

			for (int i = 0; i < mgr->ballStock(); ++i) {
				auto ball = MAKE_UNIQUE_LOG<SphereObject>("BallStock");
				ball->setParent(this);
				auto ballRender = static_cast<RenderCompBase*>(ball->getComponent(CompType::RenderComp));
				auto ballParam = ObjectParamManager::m_Instance->getParam("Ball");
				ballRender->setTexture(ballParam->textPath());
				ball->initStaticTransform(
					DirectX::SimpleMath::Vector3(0.f, 65.f * (i + 1) - 130.f, 0.f),
					DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f),
					p->ballStockScale());
				m_Balls.push_back(ball.get());
				auto levelMgr = LevelManager::m_Instance;
				levelMgr->requestAddObject(std::move(ball));
			}
		}

		~BallCase() {}

		void ChangeBall(int newStock) {
			for (size_t i = 0; i < m_Balls.size(); ++i) {
				if (static_cast<int>(i) < newStock) {
					m_Balls[i]->enableNextFrame();
				}
				else {
					m_Balls[i]->disableNextFrame();
					Audio::playSE(m_Param->sE_BllCase(), m_Param->volumeBallCase());
				}
			}
		}
	private:
		std::vector<GameBase*> m_Balls;
		const PlayerParam* m_Param;
	}; }

Player::Player(
	std::string uniqueName, bool bIsPlayerOnly)
	: GameBase(std::move(makeUniqueName(uniqueName)))
	, m_RacketCollider(nullptr)
	, m_RacketMover(nullptr)
{
	std::string key = baseName;
	auto mgr = ObjectParamManager::m_Instance;
	auto param = mgr->getParam(key);
	if (!param) {
		mgr->addParam(key, key, MAKE_UNIQUE_LOG<PlayerParam>());
		param = mgr->getParam(key);
	}
	auto playerParam = dynamic_cast<const PlayerParam*>(param);
	setParam(playerParam);
	if (!playerParam->m_BIsInitSceneBoundingData) {
		mgr->updateSceneBindingData(key);
	}
	assert(m_Param && "Player::Player() - m_Param is nullptr");
	auto& p = playerParam;
	//	mover
	addComponent(MAKE_UNIQUE_LOG<PlayerMovementComp>(this));
	m_Mover = dynamic_cast<PlayerMovementComp*>(getComponent(CompType::MovementComp));
	assert(m_Mover && "Player::Player() - m_Mover is nullptr");
	// render
	addComponent(MAKE_UNIQUE_LOG<Animator>(this));
	addComponent(MAKE_UNIQUE_LOG<ModelRenderComp>(this, p->playerMesh()));
	auto animator = static_cast<Animator*>(getComponent(CompType::Animator));
	auto model = animator->targetModel();
	Loader::animation(p->animaPathIdle(), model, p->animaLoopIdle(), p->animaScaleIdle());
	Loader::animation(p->animaPathRun(), model, p->animaLoopRun(), p->animaScaleRun());
	Loader::animation(p->animaPathSwing(), model, p->animaLoopSwing(), p->animaScaleSwing());
	Loader::animation(p->animaPathSmash(), model, p->animaLoopSmash(), p->animaScaleSmash());
	Loader::animation(p->animaPathJump(), model, p->animaLoopJump(), p->animaScaleJump());
	Loader::animation(p->animaPathHitRes(), model, p->animaLoopHitRes(), p->animaScaleHitRes());

	///////		PlayersObject	/////////
	if (bIsPlayerOnly) { return; }

	//	collider 
	auto spineNode = animator->findCloneNode("mixamorig6:Spine");
	assert(spineNode);
	auto colliderObj = MAKE_UNIQUE_LOG<PlayersObjct::BodyCollider>(this, p,spineNode);
	//	racket
	auto rightHandNode = animator->findCloneNode("mixamorig6:RightHand");
	assert(rightHandNode);
	auto racket = MAKE_UNIQUE_LOG<PlayersObjct::Racket>(this, rightHandNode);
	//	ballCase
	auto ballCase = MAKE_UNIQUE_LOG<PlayersObjct::BallCase>(this, spineNode);
	//	Shadow
	auto shadow = MAKE_UNIQUE_LOG<GameBase>(name() + std::string("_Shadow"));
	shadow->addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(shadow.get(),GPSOkey::SIMPLE_ALPHA));
	auto shadowRender = static_cast<RenderCompBase*>(shadow->getComponent(CompType::RenderComp));
	shadowRender->setSphereMesh();
	const DirectX::XMVECTORF32 SoftBlack = { { { 0.1f, 0.1f, 0.1f, 0.7f } } };
	shadow->setColor(SoftBlack);
	shadow->addComponent(MAKE_UNIQUE_LOG<MovementComp>(shadow.get()));
	m_ShadowMover = static_cast<MovementComp*>(shadow->getComponent(CompType::MovementComp));
	m_ShadowMover->setScaleX(30.f);
	m_ShadowMover->setScaleY(0.1f);
	m_ShadowMover->setScaleZ(20.f);
	m_ShadowMover->setPosZ(2.f);
	m_ShadowMover->setPosY(0.01f);
	m_ShadowMover->setGrabity(false);
	shadow->transform()->lock(TransformLock::PosY);
	auto levelMgr = LevelManager::m_Instance;
	levelMgr->requestAddObject(std::move(colliderObj));
	levelMgr->requestAddObject(std::move(racket));
	levelMgr->requestAddObject(std::move(ballCase));
	levelMgr->requestAddObject(std::move(shadow));
	levelMgr->requestAddObject(MAKE_UNIQUE_LOG<TargetUI>());
}


Player::~Player()
{
}


void Player::term()
{
	GameBase::term();
}

void Player::implupdate(uint32_t frameIndex, double deltaTime)
{
	GameBase::implupdate(frameIndex, deltaTime);
	//	影
	

	if (!m_ShadowMover) { return; }
	auto pos = transform()->worldPos();
	m_ShadowMover->setPosX(pos.x);
	m_ShadowMover->setPosZ(pos.z);

}
