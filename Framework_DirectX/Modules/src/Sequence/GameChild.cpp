#include "Sequence/GameChild.h"
#include "Sequence/GameParent.h"
#include "Manager/LevelManager.h"
#include <cassert>

namespace Sequence {
namespace Game {
	
Child::~Child() {
	if (m_LevelManager) {
		m_LevelManager->resetCurrent();
	}
}


Base* Child::update(Base* p, const uint32_t frameIndex, double deltaTime) {
	Parent* parent = dynamic_cast<Parent*>(p);
	assert(parent && "GameChild::update()");
	return update(parent,frameIndex,deltaTime);
}

}//	namespace Game
}//	namespace Sequence
