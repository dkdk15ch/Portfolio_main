#include "Block.h"
#include "Component/MovementComp.h"
#include "Component/ComponentType.h"
#include "Manager/LevelManager.h"
#include "Level/Level.h"
#include "GameObject/Spawner.h"
#include "GameObject/Item/ClearItem.h"
#include "GameObject/Enemy/Bat.h"

#include <iostream>

namespace {
	template <typename T>
	void bindSpawner(Block* block, Level* level) {
		auto spawner = level->getSpawner([](Spawner* s) {
			return s->isType<T>();
			});

		block->onDestroyed.bind([block, spawner]() {
			XMathHelper::EulerTRS trs = block->transform()->eulerTRS();
			spawner->spawn(trs);
			});
	}
}

Block::Block(const std::string& name) 
	: GameBase(name)
{

}

void Block::bind(BlockIn in){
	if(in == BlockIn::NONE){ return; }
	m_In = in;
	
	auto level = LevelManager::m_Instance->currentLevel();
	assert(level);

	switch(in){
	case BlockIn::ITEM_CLEAR:
		{	
			bindSpawner<ClearItem>(this, level);
			break;
		}
	case BlockIn::ENEMY_BAT:
		{
			bindSpawner<Bat>(this, level);
			break;
		}
	}
}

void Block::destroy()
{
	onDestroyed.execute();
	destroyCount++;
}
