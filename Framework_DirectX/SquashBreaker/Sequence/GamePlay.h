#pragma once
#include "Sequence/GameChild.h"
#include <memory>

class LevelManager;

namespace Sequence {
namespace Game {

class Play : public Child{
public:
	Play();
	virtual ~Play();
	virtual void init()override final;
	virtual void term()override;
	
	Base* update(Parent* p, const uint32_t frameIndex, double deltaTime);
	void render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex);
	void renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont);
private:

	bool m_BEnableSlomo;
	Base* transitionClerar(double deltaTime);
	Base* transitionGameOver(double deltaTime);
};
}// namespacce Game
}//	namespace Sequence
