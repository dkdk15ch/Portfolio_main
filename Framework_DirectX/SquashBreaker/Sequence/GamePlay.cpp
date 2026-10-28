#include "Sequence/GamePlay.h"
#include <Windows.h>
#include "InOutput/Pad.h"
#include "Level/GameLevel.h"
#include "GameObject/BlockWall.h"
#include "Sequence/GameClear.h"
#include "Sequence/GameOver.h"
#include "Sequence/Title.h"
#include "Manager/LightManager.h"
#include "Manager/DrawManager.h"
#include "Manager/LevelManager.h"


#include "Level/TestScene.h"
#include "Level/AnimationTestScene.h"
#include "Level/LoadTestLevel.h"

namespace {
	float x = 1.0f;
	float y = 0.0f;
	float z = 0.0f;

	DirectX::SimpleMath::Vector3 cameraRelativePos =
	{ 0.0f, 200.0f, 450 }; 

	static double DELAY_TO_CLEAR = 2.0;
	double countDelay = 0.0;
	int currentFilter = 0;
}

namespace Sequence {
	namespace Game {

		Play::Play() : m_BEnableSlomo(false)
		{
		}


		Play::~Play()
		{
		}

		void Play::init()
		{
			m_LevelManager = LevelManager::m_Instance;
			m_LevelManager->setLevel(MAKE_UNIQUE_LOG<GameLevel>("GameScene"));

			//m_LevelManager->setLevel(MAKE_UNIQUE_LOG<TestScene>("GameScene"));
			//m_LevelManager->setLevel(MAKE_UNIQUE_LOG<AnimationTestScene>("GameScene"));
			//m_LevelManager->setLevel(MAKE_UNIQUE_LOG<LoadTestLevel>("GameScene"));


			m_LevelManager->changeCurrent();
			m_LevelManager->currentLevel()->init();
		}


		void Play::term()
		{
			auto drawMgr = DrawManager::m_Instance;
			drawMgr->setFilter(DrawManager::FilterType::NONE);
		}


		Base* Play::update(Parent* p, const uint32_t frameIndex, double deltaTime) {
			Base* next = this;

			auto level = m_LevelManager->currentLevel();
#ifdef _DEBUG
			auto pad = Pad::m_Instance;
			if (pad->isTriggered(Pad::Button::D0)) {
				return new Clear();
			}
			if (pad->isTriggered(Pad::Button::MINUS)) {
				return new GameOver();
			}

			if (pad->isTriggered(Pad::Button::D9)) {
				level->setSlomotion(0.5);
			}
			if (pad->isTriggered(Pad::Button::D8)) {
				level->resetSlomotion();
			}

			if (pad->isTriggered(Pad::Button::D7)) {
				auto drawMgr = DrawManager::m_Instance;
				currentFilter++;
				int f = currentFilter % static_cast<int>(DrawManager::FilterType::FILTER_NUM);
				drawMgr->setFilter(static_cast<DrawManager::FilterType>(f));
			}

			using ButtonState = DirectX::GamePad::ButtonStateTracker::ButtonState;
			if (pad->isTriggered(Pad::Button::ESC) ||
				(pad->m_PadTracker.start == ButtonState::PRESSED)) {
				auto block = level->getObject<BlockWall>();
				block->allDestory();
			}

#else
			auto pad = Pad::m_Instance;
			using ButtonState = DirectX::GamePad::ButtonStateTracker::ButtonState;

			if (pad->isTriggered(Pad::Button::ESC) || 
				(pad->m_PadTracker.start == ButtonState::PRESSED)) {
				return new Title();
			}

#endif // _DEBUG

	//	クリア条件のチェック
	auto condition = m_LevelManager->checkClear();
	switch (condition)
	{
	case GameCondition::Playing:
		break;
	case GameCondition::Clear:
	{
		return transitionClerar(deltaTime);
	}
	//case GameCondition::Next:
	//	break;
	case GameCondition::GameOver:
	{
		return transitionGameOver(deltaTime);
	}
	default:
		break;
	}

	//	更新処理
	m_LevelManager->update(frameIndex, deltaTime);
	return next;
}


void Play::renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont) {

}


void Play::render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex) {
	auto light = LightManager::m_Instance->getLightAddress();
	//	シーンの描画
	m_LevelManager->draw(pCmdList, frameIndex, light);
}


//	クリア画面へ遷移時の処理
Base* Play::transitionClerar(double deltaTime){
	auto drawMgr = DrawManager::m_Instance;
	drawMgr->setFilter(DrawManager::FilterType::SEPIA);
	auto level = m_LevelManager->currentLevel();

	if (!m_BEnableSlomo) {
		level->setSlomotion(0.1);
		m_BEnableSlomo = true;
	}

	countDelay += deltaTime;
	if (countDelay >= DELAY_TO_CLEAR) {
		drawMgr->setFilter(DrawManager::FilterType::NONE);
		level->resetSlomotion();
		m_BEnableSlomo = false;
		countDelay = 0.0;
		return new Clear();
	}

	return this;
}

Base* Play::transitionGameOver(double deltaTime)
{
	auto drawMgr = DrawManager::m_Instance;
	drawMgr->setFilter(DrawManager::FilterType::SEPIA);
	auto level = m_LevelManager->currentLevel();

	if (!m_BEnableSlomo) {
		level->setSlomotion(0.1);
		m_BEnableSlomo = true;
	}

	countDelay += deltaTime;
	if (countDelay >= DELAY_TO_CLEAR) {
		drawMgr->setFilter(DrawManager::FilterType::NONE);
		level->resetSlomotion();
		m_BEnableSlomo = false;
		countDelay = 0.0;
		return new GameOver();
	}

	return this;
}

}// namespace Game
}// namespace Sequence
