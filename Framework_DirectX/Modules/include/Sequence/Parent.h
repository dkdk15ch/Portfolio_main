#pragma once
#include <SpriteBatch.h>
#include <SpriteFont.h>

namespace Sequence
{
	class Child;
	class Base;

	class Parent
	{
	public:
		//	基本更新
		void update(const uint32_t frameIndex, double deltaTime);
		void render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex);
		void renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont);

		static void create(Base* base);
		static void destroy();
		static Parent* instance();

	private:
		Parent(Base* base);
		~Parent();

		Child* m_Child;

		static Parent* m_Instance;
	};
	
}//	namespace Sequence
