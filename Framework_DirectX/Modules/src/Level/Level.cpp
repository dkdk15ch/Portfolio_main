#include "Level/Level.h"
#include "GameObject/Camera.h"
#include "Manager/CameraManager.h"
#include "Manager/RenderManager.h"
#include "Manager/GameManager.h"
#include "SafeDelete.h"
#include "Component/Render/RenderCompBase.h"
#include "Manager/ObjectParamManager.h"
#include "GameObject/GameBase.h"
#include "InOutput/Audio.h"
#include "Level/ClearCondition.h"


Level::Level(std::string jsonKey)
	: m_TimeScale(1.0)
	, m_JsonKey(std::move(jsonKey))
	, m_ClearCondition(nullptr)
{
	//	カメラ生成
	//	GameBaseに必要なので、最初に作る
	auto cameraDesc = Camera::Desc();
	auto cameraMgr = CameraManager::m_Instance;
	cameraMgr->addCamera("PlayerCamera", MAKE_UNIQUE_LOG<Camera>(cameraDesc));
	cameraMgr->setCurrentCamera("PlayerCamera");
	//	パラメータ読み取り
	if (!m_JsonKey.empty()) {
		moveFromJson(m_JsonKey);
		ObjectParamManager::m_Instance->
			setCurrentSceneDataKey(m_JsonKey);
	}
	//	BGMのロード
	if (!BGM_PATH.empty()) {
		auto key = Audio::loadSound(BGM_PATH);
		if (!key.empty()) {
			m_BGM = MAKE_UNIQUE_LOG<RoopSound>(key);
		}
	}
}

Level::~Level()
{
	m_ObjectList.clear();
	auto cameraMgr = CameraManager::m_Instance;
	cameraMgr->removeAll();

	m_BGM.reset();
}

void Level::term() {
	for (const auto& obj : m_ObjectList) {
		obj->term();
	}
	m_ObjectList.clear();
}

void Level::awake()
{
	if (m_BIsExecutedAwake) { return; }

	if (m_BGM) {
		m_BGM->playRoop();
		m_BGM->setVolume(m_VolmeBGM);
	}

	GameManager::m_Instance->bind();

	if (!m_ObjectList.empty()) {
		for (auto& obj : m_ObjectList) {
			
		}
	}

	if (m_ClearCondition) {
		m_ClearCondition->init();
	}
	else {
		ELOG("Warning : Level::Awake - ClearCondition is null");
	}

	m_BIsExecutedAwake = true;
}

void Level::update(const uint32_t frameIndex, double deltaTime) {
	const double dt = deltaTime * m_TimeScale;
	for (const auto& obj : m_ObjectList) {
		obj->preUpdate(frameIndex, dt);
	}
	for (const auto& obj : m_ObjectList) {
		obj->start();
	}
	for (const auto& obj : m_ObjectList) {
		obj->update(frameIndex, dt);
	}
	for (const auto& obj : m_ObjectList) {
		obj->postUpdate(frameIndex, dt);
	}
}

void Level::draw(
	ID3D12GraphicsCommandList* pCmdList,
	const uint32_t frameIndex,
	D3D12_GPU_VIRTUAL_ADDRESS lightBuffer)
{
	for (const auto& obj : m_ObjectList) {
		obj->preRender(frameIndex);
	}
	auto renderMgr = RenderManager::m_Instance;
	renderMgr->draw(pCmdList, frameIndex, lightBuffer);
	for (const auto& obj : m_ObjectList) {
		obj->postRender(pCmdList, frameIndex, lightBuffer);
	}
}

void Level::setSlomotion(double scale) {
	m_TimeScale = scale;
}

void Level::resetSlomotion() {
	m_TimeScale = 1.0;
}

void Level::addObjects(std::vector<LogPtr<GameBase>>&& objects) {
	m_ObjectList.reserve(m_ObjectList.size() + objects.size());
	for (auto&& obj : objects) {
		addObject(std::move(obj));
	}
}

void Level::addObject(LogPtr<GameBase>&& object) {
	if (m_ObjectMap.contains(object->name())) {
		return;
	}
	m_ObjectMap[object->name()] = object.get();
	m_ObjectList.push_back(std::move(object));
}

void Level::remove(std::vector<std::string>&& nameList)
{
	for (const auto& name : nameList) {
		auto it = m_ObjectMap.find(name);
		if (it == m_ObjectMap.end()) continue;
		GameBase* target = it->second;
		m_ObjectMap.erase(it);

		std::erase_if(m_ObjectList, 
			[target](const LogPtr<GameBase>& ptr) {
			return ptr.get() == target;
			});
	}
}



void Level::moveFromJson(const std::string& key)
{
	using Scene = IJsonData::SceneData;
	auto scene = ISceneData::sceneData(key);
	//	GroundData
	auto p = scene.actorParam("GroundData");
	WALL_LEFT_POS = p.getFloatSafe("wallLeft");
	WALL_RIGHT_POS = p.getFloatSafe("wallRight");
	WALL_FRONT_POS = p.getFloatSafe("wallFront");
	WALL_BACK_POS = p.getFloatSafe("wallBack");
	ROOF_POS = p.getFloatSafe("roofHeight");
	//	Sound
	p = scene.actorParam("Sound");
	BGM_PATH = p.getStringSafe("BGM");
	m_VolmeBGM = p.getFloatSafe("VOL_BGM");
	//	Camera
	auto cameraMgr = CameraManager::m_Instance;
	auto& c = cameraMgr->currentCamera();
	p = scene.actorParam("Camera");
	c.setFollowOffset(p.getVector3Safe("FOLLOW_OFFSET"));
}
