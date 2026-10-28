#include "GameObject/Param/BlockWallParam.h"

void BlockWallParam::moveFromJson(const std::string& actorKey, const std::string& sceneKey)
{
	ObjectParam::moveFromJson(actorKey, sceneKey);

	auto scene = ISceneData::sceneData(sceneKey);
	const auto& p = scene.actorParam(actorKey);

	row = p.getIntSafe("row");
	column = p.getIntSafe("coulum");
	depth = p.getIntSafe("depth");

	using ActorParam = IJsonData::ActorParam;
	const auto& param = IActorData::actorParam("BlockWall");
}
