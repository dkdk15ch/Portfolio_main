#pragma once
#include "PointerHelper.h"
#include <vector>
#include <d3d12.h>
#include <string>
#include "Level/ClearCondition.h"

class Level;
class GameBase;

class LevelManager {
public:
	static inline LevelManager* m_Instance = nullptr;

	LevelManager();
	~LevelManager();
	void term();

	GameCondition checkClear();
	void update(const uint32_t frameIndex, double deltaTime);
	void draw(ID3D12GraphicsCommandList* pCmdList,
		const uint32_t frameIndex,
		D3D12_GPU_VIRTUAL_ADDRESS lightBuffer);

	Level* currentLevel()const {
		return m_Levels[m_Current] ? 
			m_Levels[m_Current].get() : nullptr;
	}
	void changeCurrent();
	void resetCurrent();

	Level* nextLevel()const {
		return m_Levels[1 - m_Current] ? 
			m_Levels[1 - m_Current].get() : nullptr;
	}
	void setLevel(LogPtr<Level> level) {
		if (!m_Levels[m_Current]) {
			m_Levels[m_Current] = std::move(level);
			return;
		}
		m_Levels[1 - m_Current] = std::move(level);
	}

	void requestAddObject(LogPtr<GameBase>&& object);
	void requestAddObjects(std::vector<LogPtr<GameBase>>&& objectList);
	void requestRemoveObject(std::string name);
	void requestEnableObject(std::string name);
	void requestDisableObject(std::string name);
	
private:
	void processRequests();
	void processAddObjects();
	void processRemoveObjects();
	void processEnableObjets();
	void processDisableObjets();

	uint32_t m_Current;
	std::vector<LogPtr<Level>> m_Levels;
	std::vector <LogPtr<GameBase>> m_AddObjects;
	std::vector<std::string> m_RemoveObjects;
	std::vector<std::string> m_EnableObjects;
	std::vector<std::string> m_DisableObjects;
};