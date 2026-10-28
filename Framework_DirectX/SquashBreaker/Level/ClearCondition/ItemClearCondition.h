#pragma once
#include "Level/ClearCondition.h"

class ClearItem;
class SquashGameManager;

class ItemClearCondition : public ClearCondition {
public:
	void init()override final;
protected:
	bool checkClear() const override final;
	bool checkGameOver() const override final;

private:
	ClearItem* m_ClearItem;
	SquashGameManager* m_GameManger;


};