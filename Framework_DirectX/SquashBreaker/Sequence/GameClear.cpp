#include "Sequence/GameClear.h"
#include "Sequence/GamePlay.h"
#include "Sequence/Title.h"
#include "Sequence/Ready.h"
#include <ResourceUploadBatch.h>
#include "DrawHelper/Image.h"
#include "InOutput/Pad.h"
#include "Manager/DrawManager.h"
#include "Util/Font.h"
#include "Util/Loader.h"
#include "GraphicContext.h"
#include "Manager/SquashGameManager.h"
#include "Manager/LevelManager.h"
#include "Manager/LightManager.h"
#include "SafeDelete.h"
#include "Level/ClearLevel.h"
#include "Manager/AssetManager.h"
#include "InOutput/Audio.h"

namespace {
	using namespace DirectX;
	using namespace DirectX::SimpleMath;


	namespace Text {
		enum {
			START = 0,
			REPLAY,
			Count,
		};

		XMFLOAT2 defaultScale = XMFLOAT2(3.0f, 3.0f);
	}//	namespace Text

	Font::Desc fontDesc[Text::Count];
}


namespace Sequence {
namespace Game {

Clear::Clear()
{

}
	

Clear::~Clear()
{
}

void Clear::init()
{
	auto graphicContext = GraphicContext::m_Instance;
	auto device = graphicContext->device();
	auto pool = graphicContext->poolRes();

	auto data = AssetManager::m_Instance->Read("res/Images/GameClear.dds");
	m_Image = Loader::image(device, pool, L"res/Images/GameClear.dds", 
		data.data(), data.size());

	//	テキスト描画設定
	fontDesc[Text::START].text = std::format(L"Start");
	fontDesc[Text::START].position = XMFLOAT2(30.0f, 700.0f);
	fontDesc[Text::START].color = Colors::Green;
	fontDesc[Text::START].scale = Text::defaultScale;

	fontDesc[Text::REPLAY] = fontDesc[Text::START];
	fontDesc[Text::REPLAY].text = std::format(L"Replay");;
	fontDesc[Text::REPLAY].position.y += 200.0f;

	m_LevelManager = LevelManager::m_Instance;
	m_LevelManager->setLevel(MAKE_UNIQUE_LOG<ClearLevel>());
	m_LevelManager->changeCurrent();
	m_LevelManager->currentLevel()->init();

	m_BGM = MAKE_UNIQUE_LOG<RoopSound>("res/sound/BGM/BGM_GameClear.wav");
	m_BGM->setVolume(0.5f);
	m_BGM->playRoop();
}


void Clear::term()
{
	m_Image.reset();
}


Base* Clear::update(Parent* p, const uint32_t frameIndex, double deltaTime)
{
	Base* next = this;
	auto pad = Pad::m_Instance;

	//	Spaceが押されたら選択しているものを実行
	//	上下で選択対象変更
	if (pad->isTriggered(Pad::Button::SPACE))
	{
		if (m_CursorPosition == Text::START)
		{
			SquashGameManager::castedInstance()->Reset();
			next = new Sequence::Title();
		}
		else if (m_CursorPosition == Text::REPLAY)
		{
			SquashGameManager::castedInstance()->Reset();

#ifdef _DEBUG
			next = new Play();
#else
			next = new Ready();
#endif // _DEBUG
		}

	}
	else if (pad->isTriggered(Pad::Button::W))
	{
		--m_CursorPosition;
		if (m_CursorPosition < 0)
		{
			m_CursorPosition = 1;
		}
	}
	else if (pad->isTriggered(Pad::Button::S))
	{
		++m_CursorPosition;
		if (m_CursorPosition > 1)
		{
			m_CursorPosition = 0;
		}
	}

	//	選ばれてるものだけサイズを大きく
	for (int i = 0; i < Text::Count; ++i) {
		if (i == m_CursorPosition) {
			fontDesc[i].scale = XMFLOAT2(Text::defaultScale.x * 1.5f, Text::defaultScale.y * 1.5f);
		}
		else {
			fontDesc[i].scale = Text::defaultScale;
		}
	}

	m_LevelManager->update(frameIndex, deltaTime);
	return next;
}

void Clear::render(
	ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex) 
{
	auto light = LightManager::m_Instance->getLightAddress();
	//	シーンの描画
	m_LevelManager->draw(pCmdList, frameIndex, light);

}


void Clear::renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont){
	//	背景描画
	m_Image->drawBackGround(batch);
	//	文字描画
	for (auto& font : fontDesc)
	{
		Font::draw(spriteFont,font);
	}
}


}//	namespace Game
}//	namespace Sequence



