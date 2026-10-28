#include "Util/Font.h"
#include "GraphicContext.h"
#include "FIleUtil.h"
#include <SpriteFont.h>


Font::Font()
{
}


Font::~Font()
{
}


void Font::draw(DirectX::SpriteFont& spriteFont, Desc& desc)
{
	auto spriteBatch = GraphicContext::m_Instance->spriteBatch();
	assert(spriteBatch && L"Error : Font::draw() spriteBatch = nullptr");
	
	spriteFont.DrawString(
		spriteBatch,
		desc.text.c_str(),
		desc.position,
		desc.color,
		desc.rotation,
		DirectX::SimpleMath::Vector2::Zero,
		desc.scale
	);
}


DirectX::XMFLOAT2 Font::getFontLength(DirectX::SpriteFont& spriteFont, const std::wstring& text, DirectX::XMFLOAT2 scale)
{
	auto size = spriteFont.MeasureString(text.c_str());
	float lenX = DirectX::XMVectorGetX(size) * scale.x;
	float lenY = DirectX::XMVectorGetY(size) * scale.y;

	return DirectX::XMFLOAT2(lenX, lenY);
}


Font::Desc::~Desc()
{
}
