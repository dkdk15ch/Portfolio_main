#include "Level/ClearCondition/ItemClearCondition.h"
#include "Level/Level.h"
#include "GameObject/Item/ClearItem.h"
#include "Manager/LevelManager.h"
#include "Manager/SquashGameManager.h"


void ItemClearCondition::init()
{
	auto mgr = LevelManager::m_Instance;
	const auto& level = mgr->currentLevel();

	if (!m_ClearItem) {
		m_ClearItem = level->getObject<ClearItem>();
		m_ClearItem->OnGain.bind([this]() {
				setCondition(GameCondition::Clear);
			});
	}
	if(!m_GameManger){
		m_GameManger = SquashGameManager::castedInstance();
		assert(m_GameManger);
		m_GameManger->OnLostLastBallStock.bind([this]() {
				setCondition(GameCondition::GameOver);
			});
	}
	assert(m_ClearItem && m_GameManger);
}

bool ItemClearCondition::checkClear() const
{
	assert(m_ClearItem);
	return m_ClearItem->isGained();
}

bool ItemClearCondition::checkGameOver() const
{
	assert(m_GameManger);
	return !m_GameManger->hasBallStock();
}
