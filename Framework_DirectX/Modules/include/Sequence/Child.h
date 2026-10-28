#pragma once
#include "Sequence/Base.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>

namespace Sequence {
class Parent;
    
class Child : public Base
{
public:
    virtual ~Child();
    
    Base* update(Base*, const uint32_t frameIndex, double deltaTime);
    virtual Base* update(Parent* p, const uint32_t frameIndex, double deltaTime) = 0;
    virtual void render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex) = 0;
    virtual void renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont) = 0;
};

}// namespace Sequence


