#include "Sequence/Ready.h"
#include "Sequence/GamePlay.h"
#include "Sequence/GameParent.h"
#include "Sequence/Base.h"
#include "DrawHelper/Image.h"
#include "Manager/DrawManager.h"
#include "Util/Font.h"
#include "App.h"
#include "DescriptorPool.h"
#include <windows.h>
#include <SpriteBatch.h>
#include <SimpleMath.h>

namespace {
	using namespace DirectX;
	using namespace DirectX::SimpleMath;
	
	const float COUNT_SHOW_GO = 0.2f;
	const float COUNT_NEXT_SEQ = 0.5f;

	namespace {
		namespace Text {
			enum {
				READY = 0,
				GO,
				Count,
			};

			XMFLOAT2 defaultScale = XMFLOAT2(3.0f, 3.0f);
		}//	namespace Text

		Font::Desc fontDesc[Text::Count];
	}

	DirectX::XMFLOAT2 center;

	void SetPosition(int index, DirectX::XMFLOAT2 center) {
		auto app = App::m_Instance;
		auto drawMgr = DrawManager::m_Instance;
		auto& spriteFont = drawMgr->spriteFont();
		auto len = Font::getFontLength(spriteFont,fontDesc[index].text, fontDesc[index].scale);
		float x = center.x - len.x * 0.5f;
		float y = center.y - len.y * 0.5f;
		fontDesc[index].position = XMFLOAT2(x, y);
	}

	bool IsChangedPosition;
}


namespace Sequence {
namespace Game {

Ready::Ready() :
	m_Image(nullptr),
	m_Count(0),
	m_Started(false)
{

}

Ready::~Ready() 
{
}

void Ready::init()
{
	m_Image = std::make_unique<Image>();

	//	テキスト描画設定
	//	[READY]
	auto text = std::format(L"[READY]");
	fontDesc[Text::READY].text = text;
	fontDesc[Text::READY].scale = Text::defaultScale;
	auto app = App::m_Instance;
	center.x = app->width() * 0.5f;
	center.y = app->height() * 0.5f;
	SetPosition(Text::READY, center);
	fontDesc[Text::READY].color = Colors::Black;
	//	[GO]
	fontDesc[Text::GO] = fontDesc[Text::READY];
	text = std::format(L"Ready...");
	fontDesc[Text::GO].text = text;
	SetPosition(Text::GO, XMFLOAT2(center.x, center.y + 200.0f));
}


void Ready::term()
{
	m_Image.reset();
}


Base* Ready::update(Parent* p, const uint32_t frameIndex, double deltaTime) {
	Base* next = this;
	if (!m_Started)
	{
		p->startLoading();
		m_Started = true;
	}

	m_Count += static_cast<float>(deltaTime);

	//	更新処理
	if (m_Count >= COUNT_NEXT_SEQ) {
		next = new Play();
		
		return next;
	}
	else if (m_Count >= COUNT_SHOW_GO) {
		auto text = std::format(L"Go");
		fontDesc[Text::GO].text = text;
		fontDesc[Text::GO].scale = Text::defaultScale * 2.0f;
		SetPosition(Text::GO, XMFLOAT2(center.x, center.y + 200.0f));
	}
	else {
		auto text = std::format(L"Ready...");
		fontDesc[Text::GO].text = text;
		fontDesc[Text::GO].scale = Text::defaultScale;
	}
	return next;
}


void Ready::render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex) 
{
}
	

void Ready::renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont) {
	m_Image->drawFullScreen(batch);
	Font::draw(spriteFont,fontDesc[Text::READY]);
	Font::draw(spriteFont,fontDesc[Text::GO]);
}

}//	namespace Game
}//	namespace Sequence
