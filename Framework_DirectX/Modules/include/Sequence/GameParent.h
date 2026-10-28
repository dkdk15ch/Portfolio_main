#pragma once
#include "Sequence/Child.h"
#include <memory>

class Player;

namespace Sequence {
class Parent;

namespace Game{
class Child;
	namespace Stage {
		enum {
			STAGE1_1,
			STAGE1_BOSS,
			Count,
		};
	}

	class Parent : public Sequence::Child
	{
	public:
		using GrandParent = Sequence::Parent;

		Parent(Game::Child*);
		~Parent();
		virtual void term() override;

		//	基本更新
		Base* update(GrandParent* grandpa, const uint32_t frameIndex, double deltaTime);
		void render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex);
		void renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont);

		bool hasLastStageCleared() { return (m_StageID == Stage::Count); }
		int lifeNumber() const { return m_Life; }
		void startLoading();
		void goNextStage();
		void reduceLife();


	private:
		// State シーン全体を管理するクラス

		int m_StageID;
		int m_Life;

		static const int INITIAL_LIFE = 3;

		Game::Child* m_Child;
	};

	
}// namespace Game
}// namespace Sequence


