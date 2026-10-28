#pragma once
#include "Sequence/GameChild.h"
#include <memory>

class Image;
class DescriptorPool;
struct ID3D12Device;

namespace Sequence {
namespace Game {

class Ready : public Child{
public:
	Ready();
	virtual ~Ready();
	virtual void init()override final;
	virtual void term()override;

	Base* update(Parent* p, const uint32_t frameIndex, double deltaTime);
	void render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex);
	void renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont);

private:
	std::unique_ptr<Image> m_Image;
	float m_Count;
	bool m_Started;
};

}//	namespace Game
}// namespace Sequene
