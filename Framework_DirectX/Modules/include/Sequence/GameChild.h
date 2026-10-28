#pragma once
#include "Sequence/Base.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
class LevelManager;

namespace Sequence {
namespace Game {
class Parent;

class Child : public Base
{
public:
	virtual ~Child();
	Base* update(Base* p, const uint32_t frameIndex, double deltaTime);
	virtual void render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex) = 0;
	virtual void renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont) = 0;
	virtual void init() = 0;
	virtual Base* update(Parent* p, const uint32_t frameIndex, double deltaTime) = 0;

protected:
	LevelManager* m_LevelManager;

};

}//	namespcae Game
}//	namespace Sequence
