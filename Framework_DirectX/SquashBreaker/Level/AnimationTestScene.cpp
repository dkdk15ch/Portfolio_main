#include "Level/AnimationTestScene.h"
#include "GameObject/Player.h"
#include "GameObject/Ball.h"
#include "GameObject/BlockWall.h"
#include "GameObject/PlaneObject.h"
#include "GameObject/Camera.h"
#include "InOutput/Audio.h"
#include "Manager/CameraManager.h"
#include "Manager/ColliderManager.h"
#include "Manager/RenderManager.h"
#include "Component/Debug/DebugMoveComp.h"
#include "GameObject/Spawner.h"
#include "GameObject/Enemy/Bat.h"
#include "GameObject/Item/ClearItem.h"
#include "Level/ClearCondition/ItemClearCondition.h"

#include "GameObject/Enemy/Bat.h"

namespace {
	const std::string sceneKey = "GameScene";
}

AnimationTestScene::AnimationTestScene(std::string sceneKey)
	: Level(std::move(sceneKey))
{
	std::vector<LogPtr<GameBase>> objects;
	
	//auto player = MAKE_UNIQUE_LOG<Player>("Player");
	//player->removeComponent(CompType::MovementComp);
	//player->addComponent(MAKE_UNIQUE_LOG < DebugMoveComp>(player.get()));
	//objects.push_back(std::move(player));
	
	//objects.push_back(MAKE_UNIQUE_LOG<PlaneObject>
	//	("Floor", 1, "Floor"));
	//objects.push_back(MAKE_UNIQUE_LOG<PlaneObject>
	//	("FrontWall", 1, "FrontWall"));

	Spawner::create<ClearItem>("ClearItemSpawner", 1);

	//objects.push_back(MAKE_UNIQUE_LOG<Bat>());
	
	objects.push_back(MAKE_UNIQUE_LOG<TestAnimation>("TestAnima"));
	// Levelへ移す
	addObjects(std::move(objects));

	m_ColliderMgr = ColliderManager::m_Instance;

	auto cameraDesc = Camera::Desc();
	cameraDesc.farZ = 11000.f;
	auto cameraMgr = CameraManager::m_Instance;
	cameraMgr->addCamera("TestCamera", MAKE_UNIQUE_LOG<Camera>(cameraDesc));
	cameraMgr->setCurrentCamera("TestCamera");


	auto& cam = CameraManager::m_Instance->currentCamera();
	auto relativePos = 
		//DirectX::SimpleMath::Vector3(0.0f, 400.f, -950.f);
		//DirectX::SimpleMath::Vector3(0.0f, 200.f, -400.f);
		DirectX::SimpleMath::Vector3(0.0f, 3.f, -8.f);
	cam.setFollowOffset(relativePos);
	auto test = getObject<TestAnimation>("TestAnima");
	auto Player_ = getObject<Player>("Player_00");
	auto bat = getObject<Bat>("Bat_00");


	const GameBase& target = *test;
	//const GameBase& target = *Player_;
	cam.follow(target);
}

AnimationTestScene::~AnimationTestScene()
{
	m_ColliderMgr->clear();
	RenderManager::m_Instance->allClear();
}


void AnimationTestScene::update(const uint32_t frameIndex, double deltaTime)
{
	Level::update(frameIndex, deltaTime);
	//	カメラを更新
	auto& cam = CameraManager::m_Instance->currentCamera();
	cam.update(frameIndex, deltaTime);
	//	当たり判定
	m_ColliderMgr->update(frameIndex);
}

void AnimationTestScene::term()
{
	Level::term();
}


