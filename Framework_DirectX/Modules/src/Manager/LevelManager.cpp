#include "Manager/LevelManager.h"
#include "Level/Level.h"
#include "GameObject/UniqueName.h"
#include "GameObject/GameBase.h"
#include "Level/ClearCondition.h"

LevelManager::LevelManager()
	: m_Current(0)
	, m_Levels(2)
{
	if (m_Instance) return;

	m_Instance = this;
}

LevelManager::~LevelManager()
{
	term();
}

void LevelManager::term()
{
	m_Levels.clear();
}

GameCondition LevelManager::checkClear()
{
	if (!m_Levels[m_Current]->isExcutedAwake()) { return GameCondition::Playing; }
	if(!m_Levels[m_Current]->clearCondition()) { return GameCondition::Playing; }

	return m_Levels[m_Current]->clearCondition()->check();
}

void LevelManager::update(const uint32_t frameIndex, double deltaTime){
	processRequests();
	m_Levels[m_Current]->awake();
	m_Levels[m_Current]->update(frameIndex, deltaTime);
}
void LevelManager::draw(ID3D12GraphicsCommandList* pCmdList,
	const uint32_t frameIndex,
	D3D12_GPU_VIRTUAL_ADDRESS lightBuffer) {
	m_Levels[m_Current]->draw(pCmdList, frameIndex, lightBuffer);
}

void LevelManager::changeCurrent()
{
	if (!m_Levels[1 - m_Current]) { return; }
	m_Current = 1 - m_Current;
	UniqueName::clearMap();
}

void LevelManager::resetCurrent()
{
	m_Levels[m_Current].reset();
	UniqueName::clearMap();
}

void LevelManager::requestAddObject(LogPtr<GameBase>&& object) {
	m_AddObjects.push_back(std::move(object));
}

void LevelManager::requestAddObjects(std::vector<LogPtr<GameBase>>&& objectList)
{
	for (auto&& obj : objectList) {
		m_AddObjects.push_back(std::move(obj));
	}
}

void LevelManager::requestRemoveObject(std::string name)
{
	m_RemoveObjects.push_back(std::move(name));
}

void LevelManager::requestEnableObject(std::string name)
{
	m_EnableObjects.push_back(std::move(name));
}

void LevelManager::requestDisableObject(std::string name)
{
	m_DisableObjects.push_back(std::move(name));
}

void LevelManager::processRequests()
{
	processAddObjects();
	processRemoveObjects();
	processEnableObjets();
	processDisableObjets();
}

void LevelManager::processAddObjects()
{
	if (m_AddObjects.empty()) { return; }
	m_Levels[m_Current]->addObjects(std::move(m_AddObjects));
	m_AddObjects.clear();
}

void LevelManager::processRemoveObjects()
{
	if (m_RemoveObjects.empty()) { return; }
	m_Levels[m_Current]->remove(std::move(m_RemoveObjects));
	m_RemoveObjects.clear();
}

void LevelManager::processEnableObjets()
{
	if (m_EnableObjects.empty()) { return; }
	//	重複排除
	std::sort(m_EnableObjects.begin(), m_EnableObjects.end());
	m_EnableObjects.erase(std::unique(m_EnableObjects.begin(), m_EnableObjects.end()), m_EnableObjects.end());

	for (const auto& name : m_EnableObjects) {
		auto obj = m_Levels[m_Current]->getObject<GameBase>(name);
		if (obj) {
			obj->enable();
		}
	}
	m_EnableObjects.clear();
}

void LevelManager::processDisableObjets()
{
	if (m_DisableObjects.empty()) { return; }
	//	重複排除
	std::sort(m_DisableObjects.begin(), m_DisableObjects.end());
	m_DisableObjects.erase(std::unique(m_DisableObjects.begin(), m_DisableObjects.end()), m_DisableObjects.end());

	for (const auto& name : m_DisableObjects) {
		auto obj = m_Levels[m_Current]->getObject<GameBase>(name);
		if (obj) {
			obj->disable();
		}
	}
	m_DisableObjects.clear();
}


