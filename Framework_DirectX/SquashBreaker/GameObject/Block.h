#pragma once
#include "GameObject/GameBase.h"
#include "Delegate.h"
#include "Util/XMathHelper.h"

enum class BlockIn{
	NONE,
	ITEM_CLEAR,
	ENEMY_BAT,	
};

class Block : public GameBase
{
public:
	Block(const std::string& name);
	virtual ~Block() = default;
	
	Delegate::Singlecast<void()> onDestroyed;

	void bind(BlockIn in);
	void destroy();
	
	BlockIn in()const { return m_In; }

private:
	BlockIn m_In = BlockIn::NONE;
	int destroyCount = 0;
};

