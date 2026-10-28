#include "Component/ComponentBase.h"
#include <assert.h>

ComponentBase::ComponentBase(GameBase* ownerObj):
	m_Owner(nullptr)
{
	if (ownerObj) { m_Owner = ownerObj; }
	assert(ownerObj && "ComponentBase::ComponentBase");
}

GameBase& ComponentBase::owner()
{
	assert(m_Owner && "ComponentBase::owner");
	return *m_Owner;
}
void ComponentBase::init()
{
}

void ComponentBase::start()
{
}

void ComponentBase::preUpdate(uint32_t frameIndex, double deltaTime)
{
}

void ComponentBase::update(uint32_t frameIndex,	double deltaTime) {

}

void ComponentBase::postUpdate(uint32_t frameIndex, double deltaTime)
{
}

void ComponentBase::enable()
{

}

void ComponentBase::disable()
{
}

void ComponentBase::Destroy()
{

}

void ComponentBase::term()
{

}

void ComponentBase::preRender(const uint32_t frameIndex)
{

}

void ComponentBase::render(ID3D12GraphicsCommandList* pCmdList,
	const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer) 
{
}

void ComponentBase::postRender(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer)
{

}

void ComponentBase::collisionBegin(const HitInfo& other)
{
}

void ComponentBase::collisionEnd(const HitInfo& other)
{
}

void ComponentBase::changeParent(GameBase& prev, GameBase& current)
{
}

void ComponentBase::setDestroy()
{
}
