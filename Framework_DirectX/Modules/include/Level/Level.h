#pragma once
#include <vector>
#include <string>
#include <memory>
#include <d3d12.h>
#include <unordered_map>
#include "PointerHelper.h"
#include "Util/IJsonData.h"

class GameBase;
class RoopSound;
class LevelManager;
class Spawner;
class ClearCondition;

class Level : public ISceneData
{
	friend LevelManager;

public:
	Level(std::string jsonKey ="");
	virtual ~Level();
	virtual void init() {};
	virtual void term();
	//	game cycle
	virtual void awake();
	bool isExcutedAwake()const { return m_BIsExecutedAwake; }
	virtual void update(const uint32_t frameIndex, double deltaTime);
	void draw(ID3D12GraphicsCommandList* pCmdList,
		const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer);
	//	経過時間関連
	void setSlomotion(double slowSclae);
	void resetSlomotion();
	//	ステージ情報
	float wallLeft() { return WALL_LEFT_POS; }
	float wallRight() { return WALL_RIGHT_POS; }
	float wallFront() { return WALL_FRONT_POS; }
	float wallBack() { return WALL_BACK_POS; }
	float roofHeight() { return ROOF_POS; }

	template<typename T = GameBase>
	T* getObject(const std::string& name) {
		auto it = m_ObjectMap.find(name);
		if (it == m_ObjectMap.end()) {
			ELOG("Error : Scene::getObject - %s", name.c_str());
			return nullptr;
		}
		return dynamic_cast<T*>(it->second);
	}
	template<typename T = GameBase>
	T* getObject() {
		for (auto& [name, obj] : m_ObjectMap) {
			if (auto casted = dynamic_cast<T*>(obj)) {
				return casted;
			}
		}
		return nullptr;
	}

	template <typename Func>
	Spawner* getSpawner(Func checkSpawnerType){
		for (auto& [name, obj] : m_ObjectMap) {
			if (auto spawner = dynamic_cast<Spawner*>(obj)) {
				if (checkSpawnerType(spawner)) {
					return spawner;
				}
			}
		}
		return nullptr;
	}

	const std::string& jsonKey()const {
		return m_JsonKey;
	}

	ClearCondition* clearCondition()const { return m_ClearCondition.get(); }

protected:
	std::string m_JsonKey;
	virtual void moveFromJson(const std::string& key) override;
	//	GameObject
	std::unordered_map<std::string, GameBase*> m_ObjectMap;
	std::vector<LogPtr<GameBase>> m_ObjectList;
	void addObjects(std::vector<LogPtr<GameBase>>&& objects);
	void addObject(LogPtr<GameBase>&& object);
	void remove(std::vector<std::string>&& nameList);
	// SceneData
	float WALL_LEFT_POS;
	float WALL_RIGHT_POS;
	float WALL_FRONT_POS;
	float WALL_BACK_POS;
	float ROOF_POS;

	std::string BGM_PATH;
	float m_VolmeBGM;
	LogPtr<RoopSound> m_BGM;
	LogPtr<ClearCondition> m_ClearCondition;
	

private:
	double m_TimeScale;
	bool m_BIsExecutedAwake = false;
};

