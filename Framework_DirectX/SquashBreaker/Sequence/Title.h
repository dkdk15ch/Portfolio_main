#pragma once
#include "Sequence/Child.h"
#include "windows.h"
#include <memory>
#include "GameObject/Player.h"

class Image;
class RoopSound;

namespace Sequence {
class Parent;

class Title : public Child
{
public:
	Title();
	virtual ~Title();
	virtual void term()override;

	Child* update(Parent* p, const uint32_t frameIndex, double deltaTime);

	void render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex);
	void renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont);
private:
	std::unique_ptr<Image> m_Image;
	int m_CursorPosition;
	LogPtr<RoopSound> m_BGM;
};

}//	namespace Sequence
