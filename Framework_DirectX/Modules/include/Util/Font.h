#pragma once
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include "SafeDelete.h"
#include <format>
#include <SpriteFont.h>

class Font
{
public:
	Font();
	~Font();

	//	設定用クラス
	class Desc {
	public:
		std::wstring text;
		DirectX::XMFLOAT2 position;
		DirectX::XMVECTOR color;
		float rotation;
		DirectX::XMFLOAT2 origin;
		DirectX::XMFLOAT2 scale;

		Desc() :
			text(L" "),
			position(10.0f, 10.0f),
			color(DirectX::Colors::White),
			rotation(0.0f),
			origin(DirectX::SimpleMath::Vector2::Zero),
			scale(1.0f,1.0f)
		{ }
		~Desc();
	};

	static void draw(DirectX::SpriteFont& spriteFont, Desc& desc);
	static DirectX::XMFLOAT2 getFontLength(DirectX::SpriteFont& spriteFont, const std::wstring& text, DirectX::XMFLOAT2 scale);
};
