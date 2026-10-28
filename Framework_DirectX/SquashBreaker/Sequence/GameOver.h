#pragma once
#include "Sequence/GameChild.h"
#include "PointerHelper.h"

class Image;
class RoopSound;
struct ID3D12Device;
class DescriptorPool;
namespace DirectX { class ResourceUploadBatch; }

namespace Sequence {
	namespace Game {

		class GameOver : public Child {
		public:
			GameOver();
			virtual ~GameOver();
			virtual void init()override final;
			virtual void term()override;

			Base* update(Parent* p, const uint32_t frameIndex, double deltaTime);
			void render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex);
			void renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont);
		private:
			std::unique_ptr<Image> m_Image;
			int m_CursorPosition;
			LogPtr<RoopSound> m_BGM;
		};

	}// namespacce Game
}//	namespace Sequence
