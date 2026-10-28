#pragma once
#include "Component/Render/RenderCompBase.h"
#include "ConstantBuffer.h"


class InstancingRenderComp : public RenderCompBase {
public:
	InstancingRenderComp(GameBase* owner);
	virtual ~InstancingRenderComp();

};

class InstancingRenderBuffer : public RenderBufferBase {
public:
	InstancingRenderBuffer();
	virtual ~InstancingRenderBuffer() = default;

	void updateWorld(GameBase& owner, uint32_t  frameIndex) override final;
	void initRenderBuffer(
		GameBase* owner,
		Camera& camera) override final;	
};