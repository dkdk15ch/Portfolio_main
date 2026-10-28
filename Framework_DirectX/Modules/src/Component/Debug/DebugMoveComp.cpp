#include "Component/Debug/DebugMoveComp.h"
#include "InOutput/Pad.h"

void DebugMoveComp::update(uint32_t frameIndex, double deltaTime)
{
	auto pad = Pad::m_Instance;
	const float speedX = 0.03f;
	const float speedZ = 0.03f;

	{//	キーボード入力
		if (pad->m_KeyboardState.A) {
			addRotY(speedX);
		}
		if (pad->m_KeyboardState.D) {
			addRotY(-speedX);
		}
		if (pad->m_KeyboardState.W) {
			addRotX(-speedX);
		}
		if (pad->m_KeyboardState.S) {
			addRotX(speedX);
		}
	}
}
