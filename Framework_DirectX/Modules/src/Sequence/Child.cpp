#include "Sequence/Child.h"
#include "Sequence/Parent.h"
#include <cassert>

namespace Sequence {
	
Child::~Child() {}


Base* Child::update(Base* p, const uint32_t frameIndex, double deltaTime) {
	Parent* parent = dynamic_cast<Parent*>(p);
	assert(parent && "Child::update()");
	return update(parent, frameIndex,deltaTime);
}


void term() {

}

}//	namespace Sequence
