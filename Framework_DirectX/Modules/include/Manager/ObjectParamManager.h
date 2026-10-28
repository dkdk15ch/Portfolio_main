#pragma once
#include "PointerHelper.h"
#include <unordered_map>
#include <string>

class ObjectParam;
class CollisionLayerParam;

class ObjectParamManager {
public:
	static ObjectParamManager* m_Instance;

	ObjectParamManager();
	~ObjectParamManager();

	const ObjectParam* getParam(const std::string& key);

	void addParam(const std::string& jsonKey, const std::string& registKey, LogPtr<ObjectParam> newParam);
	void updateSceneBindingData(const std::string& jsonKey);

	void resetSceneDataFlag();
	void setCurrentSceneDataKey(std::string key) {
		m_CurrentSceneDataKey = key;
	}

private:
	std::unordered_map<std::string,  LogPtr<ObjectParam>> m_ParamMap;
	std::string m_CurrentSceneDataKey;
};