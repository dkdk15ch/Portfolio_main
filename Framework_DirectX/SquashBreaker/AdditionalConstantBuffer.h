#pragma once
#include "ConstantBuffer.h"

namespace CB {
	struct GlobalData {
		DirectX::XMFLOAT2 windowSize;
		float globalTime;
		float deltaTime;
	};
}