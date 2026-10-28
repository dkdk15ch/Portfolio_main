#include "GameObject/Item/ClearItem.h"
#include "Component/MovementComp.h"
#include "Component/Render/RenderCompBase.h"
#include "Manager/ObjectParamManager.h"
#include "Manager/LevelManager.h"
#include "Component/Collider/SphereColliderComp.h"
#include "Component/Collider/BoxColliderComp.h"
#include "GameObject/Param/ClearItemParam.h"
#include "InOutput/Audio.h"

namespace {
	const std::string baseName = "ClearItem";
}

ClearItem::ClearItem()
{
	std::string key = baseName;
	auto mgr = ObjectParamManager::m_Instance;
	auto param = mgr->getParam(key);
	if (!param) {
		mgr->addParam(key, key, MAKE_UNIQUE_LOG<ClearItemParam>());
		param = mgr->getParam(key);
	}
	auto itemParam = dynamic_cast<const ClearItemParam*>(param);
	setParam(itemParam);
	auto& p = itemParam;

	auto render = static_cast<RenderCompBase*>(getComponent(CompType::RenderComp));
	render->setMeshFromPath(p->meshPath());

	addComponent(MAKE_UNIQUE_LOG<MovementComp>(this));
	auto mover = static_cast<MovementComp*>(getComponent(CompType::MovementComp));
	mover->setGrabity(false);
	mover->setUniformScale(1.0f);
	transform()->unLock(TransformLock::LocalAll);

	auto collider = static_cast<ColliderCompBase*>(getComponent(CompType::ColliderComp));
	collider->setOffset(
		p->colliderOffsetPos(),
		DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f),
		p->colliderOffsetScale()
	);
	collider->show();

}

void ClearItem::enable()
{
	GameBase::enable();

	auto p = dynamic_cast<const ClearItemParam*>(param());
	Audio::playSE(p->sE_Spawn(), p->volumeSpawn());

}

void ClearItem::implOnHit(const HitInfo& info)
{
	gain();
	disableNextFrame();
}

void ClearItem::gain()
{
	if (m_BIsGained) { return; }
	disableNextFrame();
	m_BIsGained = true;

	auto p = dynamic_cast<const ClearItemParam*>(param());
	Audio::playSE(p->sE_Gain(), p->volumeGain());

	OnGain.execute();
}
