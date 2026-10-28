#include "GameObject/Item/GameItem.h"
#include "GameObject/Param/CollisionLayerParam.h"
#include "Component/Collider/SphereColliderComp.h"
#include "Component/Render/RenderCompBase.h"
#include "Strategy/IHitResponse.h"


GameItem::GameItem(std::string uniqueName)
	: GameBase(std::move(makeUniqueName(uniqueName)))
{
	addComponent(MAKE_UNIQUE_LOG<SphereColliderComp>(this));
	auto collider = static_cast<SphereColliderComp*>(getComponent(CompType::ColliderComp));
	collider->setLayer(CollisionTag::Item);
	auto layerParam = CollisionLayerParam::layerParam();
	collider->setTargetLayer(layerParam->itemTarget());
	addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(this, GPSOkey::LAMBERT));
}

GameItem::~GameItem()
{
}

void GameItem::implOnHit(const HitInfo& info)
{
	gain();
}


