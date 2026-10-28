#pragma once
#include <cstdint> 
#include "Component/ComponentType.h"
#include <d3d12.h>

class GameBase;
struct HitInfo;

class ComponentBase
{
public:
	ComponentBase(GameBase* owner);
	virtual ~ComponentBase() = default;

	void setOwner(GameBase* owner) {
		if (owner) { this->m_Owner = owner; }
	}
	GameBase& owner();
	virtual CompType compType() = 0;

	virtual void init();
	virtual void start();
	virtual void preUpdate(uint32_t frameIndex, double deltaTime);
	virtual void update(uint32_t frameIndex, double deltaTime);
	virtual void postUpdate(uint32_t frameIndex, double deltaTime);
	virtual void collisionBegin(const HitInfo& other);
	virtual void collisionEnd(const HitInfo& other);
	virtual void preRender(const uint32_t frameIndex);
	virtual void render(ID3D12GraphicsCommandList* pCmdList,
		const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer) ;
	virtual void postRender(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer);
	virtual void changeParent(GameBase& prev, GameBase& current);
	virtual void enable();
	virtual void disable();
	virtual void setDestroy();
	virtual void Destroy();
	virtual void term();

private:
	GameBase* m_Owner;
};

