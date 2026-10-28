#include "GameObject/Param/CollisionLayerParam.h"
#include "Manager/ObjectParamManager.h"

const CollisionLayerParam* CollisionLayerParam::layerParam()
{
	auto paramMgr = ObjectParamManager::m_Instance;
	auto layerParam = dynamic_cast<const CollisionLayerParam*>(paramMgr->getParam("CollisionLayer"));
	assert(layerParam);
	return layerParam;
}

void CollisionLayerParam::moveFromJson(const std::string& actorKey, const std::string& sceneKey)
{
	using ActorParam = IJsonData::ActorParam;
	const auto& param = IActorData::actorParam(actorKey);

	RACKET_TARGET = param.getIntSafe("RACKET_TARGET");
	PLAYER_TARGET = param.getIntSafe("PLAYER_TARGET");
	BALL_TARGET = param.getIntSafe("BALL_TARGET");
	ENEMY_BALL_TARGET = param.getIntSafe("ENEMY_BALL_TARGET");
	BLOCK_TARGET = param.getIntSafe("BLOCK_TARGET");
	ENEMY_TARGET = param.getIntSafe("ENEMY_TARGET");
	ITEM_TARGET = param.getIntSafe("ITEM_TARGET");
	TARGET_UI_RAY_TARGET = param.getIntSafe("TARGET_UI_RAY_TARGET");
	STATIC_TARGET = param.getIntSafe("STATIC_TARGET");
 }