#include "Level/GameLevel.h"
#include "GameObject/Player.h"
#include "GameObject/Ball.h"
#include "GameObject/BlockWall.h"
#include "GameObject/PlaneObject.h"
#include "GameObject/Camera.h"
#include "InOutput/Audio.h"
#include "Manager/CameraManager.h"
#include "Manager/ColliderManager.h"
#include "Manager/RenderManager.h"
#include "GameObject/Param/CollisionLayerParam.h"
#include "GameObject/SphereObject.h"
#include "GameObject/CubeObject.h"
#include "Manager/LevelManager.h"
#include "Component/Render/RenderCompBase.h"
#include "GameObject/Spawner.h"
#include "GameObject/Enemy/Bat.h"
#include "GameObject/Item/ClearItem.h"
#include "Level/ClearCondition/ItemClearCondition.h"

namespace {
	const std::string sceneKey = "GameScene";
}

GameLevel::GameLevel(std::string sceneKey)
	: Level(std::move(sceneKey))
{

}


GameLevel::~GameLevel()
{
	m_ColliderMgr->clear();
	RenderManager::m_Instance->allClear();
}

void GameLevel::init()
{
	std::vector<LogPtr<GameBase>> objects;
	objects.push_back(MAKE_UNIQUE_LOG<Player>("Player"));
	objects.push_back(MAKE_UNIQUE_LOG<Ball>("Ball"));
	objects.push_back(MAKE_UNIQUE_LOG<BlockWall>("BlockWall"));
	////	壁・床
	PrimitiveObjDesc LevelGeometryDesc;
	LevelGeometryDesc.hasRender = true;
	LevelGeometryDesc.hasCollider = true;
	LevelGeometryDesc.ColliderOffsetT = DirectX::SimpleMath::Vector3(0.f, 0.f, 50.f);
	LevelGeometryDesc.ColliderOffsetS = DirectX::SimpleMath::Vector3(1.f, 1.f, 50.f);
	LevelGeometryDesc.self = CollisionTag::Static;
	auto layerParam = CollisionLayerParam::layerParam();
	LevelGeometryDesc.targetLayer = layerParam->staticTarget();

	LevelGeometryDesc.paramKey = "Floor";
	objects.push_back(MAKE_UNIQUE_LOG<PlaneObject>
		(LevelGeometryDesc, LevelGeometryDesc.paramKey));
	LevelGeometryDesc.paramKey = "FrontWall";
	objects.push_back(MAKE_UNIQUE_LOG<PlaneObject>
		(LevelGeometryDesc, LevelGeometryDesc.paramKey));
	LevelGeometryDesc.paramKey = "RightWall";
	objects.push_back(MAKE_UNIQUE_LOG<PlaneObject>
		(LevelGeometryDesc, LevelGeometryDesc.paramKey));

	LevelGeometryDesc.ColliderOffsetT = DirectX::SimpleMath::Vector3(0.f, 0.f, -50.f);
	LevelGeometryDesc.ColliderOffsetS = DirectX::SimpleMath::Vector3(1.f, 1.f, 50.f);
	LevelGeometryDesc.paramKey = "LeftWall";
	objects.push_back(MAKE_UNIQUE_LOG<PlaneObject>
		(LevelGeometryDesc, LevelGeometryDesc.paramKey));
	LevelGeometryDesc.paramKey = "Roof";
	LevelGeometryDesc.hasRender = false;
	objects.push_back(MAKE_UNIQUE_LOG<PlaneObject>
		(LevelGeometryDesc, LevelGeometryDesc.paramKey));
	//	spawner
	using Scene = IJsonData::SceneData;
	auto scene = ISceneData::sceneData(m_JsonKey);
	auto p = scene.actorParam("BatSpawner");
	Spawner::create<Bat>("BatSpawner", p.getUInt32Safe("MAX_NUM"));
	p = scene.actorParam("ClearItemSpawner");
	Spawner::create<ClearItem>("ClearItemSpawner", p.getUInt32Safe("MAX_NUM"));

	addObjects(std::move(objects));

	m_ColliderMgr = ColliderManager::m_Instance;
	m_ClearCondition = MAKE_UNIQUE_LOG<ItemClearCondition>();
	m_Player = getObject<Player>();

	//stopBall();
}

void GameLevel::awake()
{
	Level::awake();

	m_BGM->setVolume(0.5f);
}


void GameLevel::update(const uint32_t frameIndex, double deltaTime)
{
	Level::update(frameIndex, deltaTime);
	//	カメラを更新
	auto& cam = CameraManager::m_Instance->currentCamera();
	cam.follow(*m_Player);
	cam.update(frameIndex, deltaTime);
	//	当たり判定
	m_ColliderMgr->update(frameIndex);
}


void GameLevel::term()
{
	Level::term();
}

void GameLevel::stopBall()
{
#ifdef _DEBUG
	auto ball = getObject("Ball_00");
	if (!ball) { return; }

	ball->setSlomotion(0.0);
#endif // _DEBUG
}
