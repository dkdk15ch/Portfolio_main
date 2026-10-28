#include "Sequence/Parent.h"
#include <cassert>
#include "SafeDelete.h"
#include "Sequence/Base.h"
#include "Sequence/Child.h"

namespace Sequence {

Parent* Parent::m_Instance = nullptr;

/**
 * @brief		コンストラクタで呼ばずにこの関数で作成
 * @param base	初期のシークエンスを渡す
 */
void Parent::create(Base* base)
{
 	assert(!m_Instance && "Parent::create() m_Instance");
	assert(base && "Parent::create() base");
	m_Instance = new Parent(base);
}


void Parent::destroy() 
{
	printf("Parent::destory\n");
	assert(m_Instance && "Parent::destroy() m_Instance");
	m_Instance->m_Child->term();
	SAFE_DELETE(m_Instance);
}


Parent* Parent::instance()
{
	return m_Instance;
}


Parent::Parent(Base* base) : m_Child(nullptr)
{
	m_Child = dynamic_cast<Child*>(base);
}


Parent::~Parent()
{
	SafeDelete(m_Child);
}


void Parent::update(const uint32_t frameIndex, double deltaTime)
{
	Base* nextChild = m_Child->update(this,frameIndex,deltaTime);
	if (nextChild != m_Child)
	{
		//	遷移判定
		Child* casted = dynamic_cast<Child*>(nextChild);
		assert(casted && "Parent::update() casted");
		SafeDelete(m_Child);
		m_Child = casted;
	}
}


void Parent::render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex) {
	m_Child->render(pCmdList,frameIndex);
}


void Parent::renderSprite(DirectX::SpriteBatch& batch,DirectX::SpriteFont& spriteFont)
{
	m_Child->renderSprite(batch,spriteFont);
}

}//	namespace Sequence
