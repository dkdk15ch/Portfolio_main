#include "Sequence/GameParent.h"
#include "Sequence/GameChild.h"
#include "Sequence/Parent.h"
#include "Sequence/Base.h"
#include "SafeDelete.h"

namespace Sequence {
namespace Game {

Parent::Parent(Game::Child* gameFirstSeq):
	m_StageID(0),
	m_Life(INITIAL_LIFE),
	m_Child(nullptr)
{
	//	最初の遷移先を設定
	m_Child = gameFirstSeq;
	if (m_Child) {
		m_Child->init();
	}
}


Parent::~Parent() {
	SafeDelete(m_Child);
}


void Parent::term()
{
	m_Child->term();
}


Base* Parent::update(GrandParent* grandpa, const uint32_t frameIndex, double deltaTime) {
	Base* next = this;
	Base* nextChild = m_Child->update(this,frameIndex,deltaTime);
	//	遷移判定
	if (nextChild != m_Child)
	{
		Game::Child* casted = dynamic_cast<Game::Child*>(nextChild);
		if(casted)
		{
			SafeDelete(m_Child);
			m_Child = casted;
			m_Child->init();
			m_Child->update(this, frameIndex, deltaTime);
		}
		else {
			next = nextChild;
		}
	}
	return next;
}


void Parent::render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex) {
	m_Child->render(pCmdList,frameIndex);
}


void Parent::renderSprite(DirectX::SpriteBatch& batch, DirectX::SpriteFont& spriteFont) {
	m_Child->renderSprite(batch,spriteFont);
}


void Parent::startLoading() {
}


void Parent::goNextStage(){	
	++m_StageID;
}


void reduceLife() {
}

}//	namespcace Game
}//	namespace Sequence
