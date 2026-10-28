#include "Sequence/PlayGuid.h"
#include "Sequence/Title.h"
#include "Sequence/Parent.h"
#include "Sequence/GameParent.h"
#include "Sequence/Ready.h"
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

namespace {
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	namespace GuidImage {
		enum {
			HOW_TO_PLAY,
			START,
			LOST,
			RALLY,
			Count,
		};
	};// namespace Images

	namespace UtilImage {
		enum {
			D_BUTTON,
			START,
			Count,
		};
	}

	namespace Dpad {
		float top;
		float bottom;
		constexpr float size = 128;
		constexpr float offsetX = 20.f;
	};// namespace buttonIcon

	namespace StartButton {
		RECT rect;
		constexpr float size = 128;
	}// namespace startButton

	namespace Text {
		enum {
			PAGES,
			TITLE,
			Count,
		};
		float titleFontEndPosX = 0.f;
	}//	namespace Text
	Font::Desc fontDesc[Text::Count];
}//	namespace


namespace Sequence {
	PlayGuid::PlayGuid() 
		: m_GuidImages(GuidImage::Count)
		, m_UtilImages(UtilImage::Count)
	{
		auto graphicContext = GraphicContext::m_Instance;
		auto device = graphicContext->device();
		auto pool = graphicContext->poolRes();
		auto drawMgr = DrawManager::m_Instance;
		auto& spriteFont = drawMgr->spriteFont();
		auto assetMgr = AssetManager::m_Instance;
		//	操作方法
		std::string howToPlayGuid = "res/Images/PlayGuid0.dds";
		auto data = assetMgr->Read(howToPlayGuid.c_str());
		m_GuidImages[GuidImage::HOW_TO_PLAY] = Loader::image(device, pool, StringToWString(howToPlayGuid).c_str(), data.data(), data.size());
		std::string startGuid = "res/Images/PlayGuid5.dds";
		data = assetMgr->Read(startGuid.c_str());
		m_GuidImages[GuidImage::START] = Loader::image(device, pool, StringToWString(startGuid).c_str(), data.data(), data.size());
		std::string gameoverGuid = "res/Images/PlayGuid6.dds";
		data = assetMgr->Read(gameoverGuid.c_str());
		m_GuidImages[GuidImage::LOST] = Loader::image(device, pool, StringToWString(gameoverGuid).c_str(), data.data(), data.size());
		std::string rallyGuid = "res/Images/PlayGuid7.dds";
		data = assetMgr->Read(rallyGuid.c_str());
		m_GuidImages[GuidImage::RALLY] = Loader::image(device, pool, StringToWString(rallyGuid).c_str(), data.data(), data.size());
		//	Icon画像
		std::string D_PadIcon = "res/Images/DpadRight.dds";
		data = assetMgr->Read(D_PadIcon.c_str());
		m_UtilImages[UtilImage::D_BUTTON] = Loader::image(device, pool, StringToWString(D_PadIcon).c_str(), data.data(), data.size());
		auto app = App::m_Instance;
		float height = app->height();
		const float centerY = height * 0.5f;
		Dpad::top = centerY - Dpad::size * 0.5f;
		Dpad::bottom = centerY + Dpad::size * 0.5f;

		std::string startButtonIcon = "res/Images/StartButton.dds";
		data = assetMgr->Read(startButtonIcon.c_str());
		m_UtilImages[UtilImage::START] = Loader::image(device, pool, StringToWString(startButtonIcon).c_str(), data.data(), data.size());

		//	テキスト描画設定
		float width = app->width();
		fontDesc[Text::PAGES].text = std::format(L"Page {} / {}", 
			m_Current + 1, static_cast<int>(GuidImage::Count));
		fontDesc[Text::PAGES].position = XMFLOAT2(width - 400.f, height - 100.f);
		fontDesc[Text::PAGES].color = Colors::Gray;
		fontDesc[Text::PAGES].scale = XMFLOAT2(2.0f, 2.0f);

		
		fontDesc[Text::TITLE].text = std::format(L"Title : ");
		fontDesc[Text::TITLE].position = XMFLOAT2(50.f, height - 100.f);
		fontDesc[Text::TITLE].color = Colors::Gray;
		fontDesc[Text::TITLE].scale = XMFLOAT2(2.0f, 2.0f);
		auto len = Font::getFontLength(
			spriteFont, fontDesc[Text::TITLE].text, fontDesc[Text::TITLE].scale);
		Text::titleFontEndPosX = fontDesc[Text::TITLE].position.x + len.x;

		float left = Text::titleFontEndPosX + 10.f;
		float top = fontDesc[Text::TITLE].position.y + len.y * 0.5f - StartButton::size * 0.5f;
		StartButton::rect = RECT(left, top, left + StartButton::size, top + StartButton::size);

		SquashGameManager::castedInstance()->Reset();
	}


	PlayGuid::~PlayGuid()
	{
	}


	void PlayGuid::term()
	{
		m_GuidImages.clear();
	}


	Child* PlayGuid::update(Parent* parent, const uint32_t frameIndex, double deltaTime)
	{
		Child* next = this;
		auto pad = Pad::m_Instance;

		using ButtonState = DirectX::GamePad::ButtonStateTracker::ButtonState;
		if (pad->isTriggered(Pad::Button::ESC) ||
			(pad->m_PadTracker.start == ButtonState::PRESSED)) {
			next = new Sequence::Title();
		}

		if (pad->isTriggered(Pad::Button::A))
		{
			--m_Current;
			if (m_Current < 0)
			{
				m_Current = GuidImage::Count - 1;
			}
			fontDesc[Text::PAGES].text = std::format(L"Page {} / {}",
				m_Current + 1, static_cast<int>(GuidImage::Count));
		}
		else if (pad->isTriggered(Pad::Button::D))
		{
			++m_Current;
			if (m_Current > GuidImage::Count - 1)
			{
				m_Current = 0;
			}
			fontDesc[Text::PAGES].text = std::format(L"Page {} / {}",
				m_Current + 1, static_cast<int>(GuidImage::Count));
		}

		return next;
	}

	void PlayGuid::render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex)
	{
	}

	void PlayGuid::renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont)
	{
		//	画像描画
		m_GuidImages[m_Current]->drawBackGround(batch);

		auto app = App::m_Instance;
		auto width = app->width();
		float left = width - Dpad::size - Dpad::offsetX;
		float right = width - Dpad::offsetX;
		auto rect = RECT(left, Dpad::top, right, Dpad::bottom);
		m_UtilImages[UtilImage::D_BUTTON]->drawStretchRect(batch, rect, nullptr, DirectX::Colors::Gray);

		left = Dpad::offsetX;
		right = left + Dpad::size;
		rect = RECT(left , Dpad::top, right , Dpad::bottom);
		auto ori = DirectX::XMFLOAT2(Dpad::size * 0.5f, Dpad::size * 0.5f);
		m_UtilImages[UtilImage::D_BUTTON]->drawStretchRect(batch, rect, nullptr , DirectX::Colors::Gray, DirectX::XM_PI, ori);
		m_UtilImages[UtilImage::START]->drawStretchRect(batch, StartButton::rect, nullptr, DirectX::Colors::Gray); 



		for(auto& font : fontDesc)
		{
			Font::draw(spriteFont, font);
		}
	}

}//	namespace Sequence

