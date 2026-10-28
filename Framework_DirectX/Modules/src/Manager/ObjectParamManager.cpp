#include "Manager/ObjectParamManager.h"
#include "GameObject/ObjectParam.h"

ObjectParamManager* ObjectParamManager::m_Instance = nullptr;

ObjectParamManager::ObjectParamManager()
{
	if (m_Instance) return;

	m_Instance = this;
}

ObjectParamManager::~ObjectParamManager()
{
	m_ParamMap.clear();
}

const ObjectParam* ObjectParamManager::getParam(const 
	std::string& key)
{
	auto it = m_ParamMap.find(key);
	if (it == m_ParamMap.end()) {
		return nullptr;
	}

	return it->second.get();
}

void ObjectParamManager::addParam(
	const std::string& jsonKey,
	const std::string& registKey,
	LogPtr<ObjectParam> newParam)
{
	if (m_ParamMap.contains(registKey)) { return; }

	newParam->moveFromJson(jsonKey, m_CurrentSceneDataKey);
	m_ParamMap[registKey] = std::move(newParam);
}

void ObjectParamManager::updateSceneBindingData(const std::string& jsonKey)
{
	auto it = m_ParamMap.find(jsonKey);
	if (it == m_ParamMap.end()) { return;}
	if (it->second->m_BIsInitSceneBoundingData) {	return;}

	it->second->moveFromJson(jsonKey, m_CurrentSceneDataKey);
}

void ObjectParamManager::resetSceneDataFlag() {
	for (auto& p : m_ParamMap) {
		p.second->m_BIsInitSceneBoundingData = false;
	}
}
