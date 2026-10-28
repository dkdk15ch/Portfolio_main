#include "TestAnimation.h"
#include "GameObject/Camera.h"
#include "Manager/AssetManager.h"
#include "StringHelper.h"
#include "PointerHelper.h"
#include "Animation/Model.h"
#include "Animation/ModelMesh.h"
#include "Animation/Bone.h"
#include "Animation/Animation.h"
#include "Animation/AnimationKeyFrame.h"
#include "Component/Debug/DebugMoveComp.h"
#include "Component/Render/RenderCompBase.h"
#include "Component/Render/ModelRenderComp.h"
#include "Component/Animator.h"
#include "Component/MovementComp.h"
#include "Component/TransformComp.h"
#include "InOutput/Pad.h"
#include "GameObject/SphereObject.h"
#include "GameObject/PlaneObject.h"

#include "Level/Level.h"
#include "Manager/LevelManager.h"
#include "Util/Loader.h"


#include <iostream>



TestAnimation::TestAnimation(const std::string& name)
	: GameBase(name)
	, m_TestModel(MAKE_UNIQUE_LOG<Model>()) 
{
	// debug
	addComponent(MAKE_UNIQUE_LOG<DebugMoveComp>(this));
	m_Mover = dynamic_cast<DebugMoveComp*>(getComponent(CompType::MovementComp));
	assert(m_Mover && "m_Mover is nullptr");
	m_Mover->setPos(DirectX::SimpleMath::Vector3(
		50.0f, 0.f, 0.f
	));
	m_Mover->setUniformScale(1.f);
	m_Mover->setGrabity(false);
	//	Model
	auto modelPath = L"res/Mesh/Player/Ch20_nonPBR.fbx";
	auto assetMgr = AssetManager::m_Instance;
	auto key = assetMgr->getKeyFromPath(modelPath, AssetType::MODEL);
	if (key.empty()) {
		auto model = MAKE_UNIQUE_LOG<Model>();
		model->init(modelPath);
		m_ModelName = model->name();
		assetMgr->Add(modelPath ,m_ModelName, std::move(model));
		key = assetMgr->getKeyFromPath(modelPath, AssetType::MODEL);
	}
	else {
		 auto model = assetMgr->getModel(key);
		 m_ModelName = model->name();
	}
	// Animation add
	auto m = assetMgr->getModel(m_ModelName);
	Loader::animation(L"res/Animation/Player/Idle.fbx", m, true, 1.5);
	Loader::animation(L"res/Animation/Player/Swing.fbx",	m, true, 1.0);
	// Comp Model
	addComponent(MAKE_UNIQUE_LOG<Animator>(this));
	auto ani = dynamic_cast<Animator*>(getComponent(CompType::Animator));
	//ani->SetModel(key);

	addComponent(MAKE_UNIQUE_LOG<ModelRenderComp>(this,m_ModelName));

	auto render = getComponent(CompType::RenderComp);

	// racket
	auto racket = MAKE_UNIQUE_LOG<GameBase>("TennisRacket");
	m_RacketObj = racket.get();
	racket->setParent(this);
	racket->addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(racket.get(), GPSOkey::SIMPLE));

	auto racketRender = static_cast<RenderCompBase*>(racket->getComponent(CompType::RenderComp));
	racketRender->setMeshFromPath(L"res/Mesh/Player/Racket.fbx");
	racketRender->setTexture(L"res/Textures/Player/Racket.dds");
	racket->addComponent(MAKE_UNIQUE_LOG<MovementComp>(racket.get()));
	m_RacketMover = static_cast<MovementComp*>(racket->getComponent (CompType::MovementComp));
	m_RacketMover->setGrabity(false);
	m_RacketMover->setUniformScale(0.01f);
	m_RacketMover->setPosY(0.08f);
	m_RacketMover->setPosZ(-0.04f);
	m_RacketMover->setRotX(1.63f);
	m_RacketMover->setRotY(1.98f);

	auto racketString = MAKE_UNIQUE_LOG<GameBase>("RacketString");
	racketString->setParent(racket.get());
	racketString->addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(racketString.get(), GPSOkey::SIMPLE_ALPHA));
	auto gatRender = static_cast<RenderCompBase*>(racketString->getComponent(CompType::RenderComp));
	gatRender->setPlaneMesh();
	gatRender->setTexture(L"res/Textures/Player/String.dds", DirectX::XMFLOAT2(10.f, 10.f));
	racketString->addComponent(MAKE_UNIQUE_LOG<MovementComp>(racketString.get()));
	m_RacketStringMover = static_cast<MovementComp*>(racketString->getComponent(CompType::MovementComp));
	m_RacketStringMover->setTransform(
		DirectX::SimpleMath::Vector3(4.f, 0.f, 115.f),
		DirectX::SimpleMath::Vector3(DirectX::XM_PIDIV2, 0.f,  0.f),
		DirectX::SimpleMath::Vector3(30.f, 45.f, 0.f)
	);
	m_RacketStringMover->setGrabity(false);

	auto levelMgr = LevelManager::m_Instance;
	levelMgr->requestAddObject(std::move(racket));
	levelMgr->requestAddObject(std::move(racketString));
	


}

TestAnimation::~TestAnimation()
{
	m_TestModel.reset();
}

void TestAnimation::implStart()
{
	auto ani = dynamic_cast<Animator*>(getComponent(CompType::Animator));
	auto attachNode = ani->findCloneNode("mixamorig6:RightHand");
	assert(attachNode);
	m_RacketObj->transform()->attachNode(attachNode);
}

void TestAnimation::implPreupdate(uint32_t frameIndex, double deltaTime)
{
	if (m_BToggleTimeScale) {
		auto ani = dynamic_cast<Animator*>(getComponent(CompType::Animator));
		ani->setTimeScale(1.0 - ani->timeScale());
		m_BToggleTimeScale = false;
	}
}

void TestAnimation::implupdate(uint32_t frameIndex, double deltaTime)
{
	auto pad = Pad::m_Instance;
	if (pad->m_KeyboardState.R) {
		m_Mover->addRotY(0.05f);
	}

	if (pad->isTriggered(Pad::Button::TAB)) {
		auto ani = dynamic_cast<Animator*>(getComponent(CompType::Animator));
		const auto& model = ani->targetModel();
		//auto aniKey = model->name() + "|Damage";
		//auto aniKey = model->name() + "|Idle";
		auto aniKey = model->name() + "|Swing";
		auto nextAnimation = model->findAnimation(aniKey);
		ani->setNextAnimation(nextAnimation);
	}

	if (pad->isTriggered(Pad::Button::SPACE)) {
		m_BToggleTimeScale = !m_BToggleTimeScale;
	}

	if (pad->isTriggered(Pad::Button::D1)) {
		auto ani = dynamic_cast<Animator*>(getComponent(CompType::Animator));
		ani->setFlipX(true);
	}

	if (pad->isTriggered(Pad::Button::D2)) {
		auto ani = dynamic_cast<Animator*>(getComponent(CompType::Animator));
		ani->setFlipX(false);
	}

	 
	if (pad->isTriggered(Pad::Button::D4)) {
		m_RacketStringMover->addPosX(0.1f);
	}
	if (pad->isTriggered(Pad::Button::D5)) {
		m_RacketStringMover->addPosY(0.1f);
	}
	if (pad->isTriggered(Pad::Button::D6)) {
		m_RacketStringMover->addPosZ(1.0f);
	}

	GameBase::implupdate(frameIndex, deltaTime);

}

void TestAnimation::implOnHit(const HitInfo& other)
{
}



