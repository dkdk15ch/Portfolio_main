#include "Sequence/Title.h"
#include "Sequence/Parent.h"
#include "Sequence/GameParent.h"
#include "Sequence/Ready.h"
#include "Sequence/PlayGuid.h"
#include <ResourceUploadBatch.h>
#include "DrawHelper/Image.h"
#include "SafeDelete.h"
#include "InOutput/Pad.h"
#include "Util/Loader.h"
#include "Util/Font.h"
#include "GraphicContext.h"
#include "Manager/SquashGameManager.h"
#include "Manager/AssetManager.h"
#include "Manager/DrawManager.h"
#include "InOutput/Audio.h"

namespace {
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	namespace Text {
		enum {
			START = 0,
			PLAY_GUID,
			EXIT,
			Count,
		};

		XMFLOAT2 defaultScale = XMFLOAT2(2.0f, 2.0f);
		float defaultLenY = 0.f;
		float bigLenY = 0.f;
		float offset = 5.f;
	}//	namespace Text

	Font::Desc fontDesc[Text::Count];
}


namespace Sequence {
Title::Title() :
	m_Image(nullptr), m_CursorPosition(0)
{
	auto graphicContext = GraphicContext::m_Instance;
	auto device = graphicContext->device();
	auto pool = graphicContext->poolRes();
	auto drawMgr = DrawManager::m_Instance;
	auto& spriteFont = drawMgr->spriteFont();


	//	画像作成
	auto data = AssetManager::m_Instance->Read("res/Images/Title2.dds");
	m_Image = Loader::image(device, pool, L"res/Images/Title2.dds", data.data(), data.size());

	//	テキスト描画設定
	fontDesc[Text::START].text = std::format(L"GameStart !");
	fontDesc[Text::START].position = XMFLOAT2(30.0f, 750.0f);
	fontDesc[Text::START].color = Colors::Green;
	fontDesc[Text::START].scale = Text::defaultScale * 2.0f;
	auto bigSize = Font::getFontLength(spriteFont, fontDesc[Text::START].text, fontDesc[Text::START].scale);
	Text::bigLenY = bigSize.y;
	
	fontDesc[Text::PLAY_GUID] = fontDesc[Text::START];
	fontDesc[Text::PLAY_GUID].text = std::format(L"Play Guid");;
	fontDesc[Text::PLAY_GUID].position.y += Text::bigLenY + Text::offset;
	fontDesc[Text::PLAY_GUID].scale = Text::defaultScale;
	auto defaultSize = Font::getFontLength(spriteFont, fontDesc[Text::PLAY_GUID].text, fontDesc[Text::PLAY_GUID].scale);
	Text::defaultLenY = defaultSize.y;

	fontDesc[Text::EXIT] = fontDesc[Text::PLAY_GUID];
	fontDesc[Text::EXIT].text = std::format(L"Game Exit");;
	fontDesc[Text::EXIT].position.y += Text::defaultLenY + Text::offset;

	m_BGM = MAKE_UNIQUE_LOG<RoopSound>("res/sound/BGM/BGM_Title.wav");
	m_BGM->setVolume(2.f);
	m_BGM->playRoop();

	SquashGameManager::castedInstance()->Reset();
}


Title::~Title()
{
	m_BGM->stop();
}


void Title::term()
{
	m_Image.reset();
}

Child* Title::update(Parent* parent, const uint32_t frameIndex, double deltaTime)
{
	Child* next = this;
	auto pad = Pad::m_Instance;

	//	Spaceが押されたら選択しているものを実行
	//	上下で選択対象変更
	if (pad->isTriggered(Pad::Button::SPACE))
	{
		if (m_CursorPosition == Text::START)
		{
 			next = new Game::Parent(new Game::Ready());
		}
		else if (m_CursorPosition == Text::PLAY_GUID)
		{
			next = new Sequence::PlayGuid();
		}
		else if (m_CursorPosition == Text::EXIT)
		{
			PostQuitMessage(0);
		}
	}
	else if (pad->isTriggered(Pad::Button::W))
	{
		--m_CursorPosition;
		if (m_CursorPosition < 0)
		{
			m_CursorPosition = Text::Count - 1;
		}
	}
	else if (pad->isTriggered(Pad::Button::S))
	{
		++m_CursorPosition;
		if (m_CursorPosition > Text::Count - 1)
		{
			m_CursorPosition = 0;
		}
	}

	//	選ばれてるものだけサイズを大きく
	for (int i = 0; i < Text::Count; ++i) {
		if (i == m_CursorPosition) {
			fontDesc[i].scale = XMFLOAT2(Text::defaultScale.x * 2.f, Text::defaultScale.y * 2.f);
			if (i == Text::START) { continue; }
			fontDesc[i].position.y = fontDesc[i - 1].position.y + Text::defaultLenY + Text::offset;
		}
		else {
			fontDesc[i].scale = Text::defaultScale;
			if (i == Text::START) { continue; }

			float offset;
			if (i - 1 == m_CursorPosition) {
				offset = Text::bigLenY + Text::offset;
			}
			else {
				offset = Text::defaultLenY + Text::offset;
			}
			fontDesc[i].position.y = fontDesc[i - 1].position.y + offset;
		}
	}
	
	return next;
}

void Title::render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex) 
{
}

void Title::renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont)
{
	//	背景描画
	m_Image->drawFullScreen(batch);
	//	文字描画
	for (auto& font : fontDesc)
	{
		Font::draw(spriteFont,font);
	}
}

}//	namespace Sequence

