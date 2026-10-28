#include "GameObject/Spawner.h"
#include "Component/MovementComp.h"
#include <algorithm>


Spawner::Spawner(std::string name , uint32_t maxNum)
	: GameBase(name, 1, "")
	, m_MaxNum(maxNum)
{

}

void Spawner::spawn(
	DirectX::SimpleMath::Vector3 pos,
	DirectX::SimpleMath::Vector3 rot ,
	DirectX::SimpleMath::Vector3 scale )
{
	if (m_NextUseIndex >= m_MaxNum) { return; }
	auto& e = m_List[m_NextUseIndex];
	e->enableNextFrame();
	auto mover = dynamic_cast<MovementComp*>(e->getComponent(CompType::MovementComp));
	if (mover) {
		mover->setPos(pos);
	}
	else {
		e->initStaticTransform(pos, rot, scale);
	}
	m_NextUseIndex++;
}

void Spawner::spawn(XMathHelper::EulerTRS trs)
{
	if (m_NextUseIndex >= m_MaxNum) { return; }
	auto& e = m_List[m_NextUseIndex];
	e->enableNextFrame();
	auto mover = dynamic_cast<MovementComp*>(e->getComponent(CompType::MovementComp));
	if (mover) {
		mover->setPos(trs.translation);
	}
	else {
		e->initStaticTransform(trs.translation, trs.rot, trs.scale);
	}
	m_NextUseIndex++;
}

void Spawner::despawn(GameBase* target)
{
	if (!target) { return; }
	if (m_NextUseIndex == 0) { return; }

	for (uint32_t i = 0; i < m_NextUseIndex; ++i) {
		if (m_List[i] == target) {
			m_List[i]->disableNextFrame();
			std::swap(m_List[i], m_List[m_NextUseIndex - 1]);
			m_NextUseIndex--;
			return;
		}
	}
}