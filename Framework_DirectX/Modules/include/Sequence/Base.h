#pragma once
#include <stdint.h>

namespace Sequence {

class Base {
public:
	virtual ~Base(){}
	virtual Base* update(Base* p, const uint32_t frameIndex, double deltaTime) = 0;
	virtual void term() = 0;
};

}//	namespace Sequence
