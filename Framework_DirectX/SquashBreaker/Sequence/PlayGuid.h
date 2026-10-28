#pragma once
#include "Sequence/Child.h"
#include "windows.h"
#include <memory>
#include "GameObject/Player.h"

class Image;

namespace Sequence {
	class Parent;

	class PlayGuid : public Child
	{
	public:
		PlayGuid();
		virtual ~PlayGuid();
		virtual void term()override;

		Child* update(Parent* p, const uint32_t frameIndex, double deltaTime);

		void render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex);
		void renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont);
	private:
		std::vector<std::unique_ptr<Image>> m_GuidImages;
		std::vector<std::unique_ptr<Image>> m_UtilImages;
		
		int m_Current = 0;
	};

}//	namespace Sequence
