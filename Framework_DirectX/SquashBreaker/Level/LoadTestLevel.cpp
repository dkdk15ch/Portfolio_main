#include "Level/LoadTestLevel.h"
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
#include "GameObject/TestAnimation.h"
#include "GameObject/Test/TestLoadObj.h"

#include "Manager/AssetManager.h"

LoadTestLevel::LoadTestLevel(std::string jsonKey)
{
	std::vector<LogPtr<GameBase>> objects;

	objects.push_back(MAKE_UNIQUE_LOG<TestLoadObj>("TestAnima"));

	//objects.push_back(MAKE_UNIQUE_LOG<Bat>());

	// Levelへ移す
	addObjects(std::move(objects));

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
	auto test = getObject<TestLoadObj>("TestAnima");
	auto Player_ = getObject<Player>("Player_00");
	auto bat = getObject<Bat>("Bat_00");


	const GameBase& target = *test;
	//const GameBase& target = *Player_;
	cam.follow(target);

	m_BGM = MAKE_UNIQUE_LOG<RoopSound>("res/sound/BGM/BGM_GamePlay.wav");
	auto assetMgr = AssetManager::m_Instance;

}

LoadTestLevel::~LoadTestLevel()
{
}

void LoadTestLevel::update(const uint32_t frameIndex, double deltaTime)
{
	Level::update(frameIndex, deltaTime);
	//	カメラを更新
	auto& cam = CameraManager::m_Instance->currentCamera();
	cam.update(frameIndex, deltaTime);
}

void LoadTestLevel::term()
{
	Level::term();
}
