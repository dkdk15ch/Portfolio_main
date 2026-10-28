#include "GameObject/Param/ClearItemParam.h"
#include "InOutput/Audio.h"

void ClearItemParam::moveFromJson(const std::string& actorKey, const std::string& sceneKey)
{
	ObjectParam::moveFromJson(actorKey, sceneKey);
	
	const auto& param = IActorData::actorParam(actorKey);
	MESH_PATH = param.getWStringSafe("CLEAR_ITEM_MESH");
	SE_SPAWN = param.getStringSafe("SE_SPAWN");
	Audio::loadSound(SE_SPAWN);
	VOL_SPAWN = param.getFloatSafe("VOL_SPAWN");
	SE_GAIN = param.getStringSafe("SE_GAIN");
	Audio::loadSound(SE_GAIN);
	VOL_GAIN = param.getFloatSafe("VOL_GAIN");
	COL_OFFSET_POS = param.getVector3Safe("COL_OFFSET_POS");
	COL_OFFSET_SCALE = param.getVector3Safe("COL_OFFSET_SCALE");
}
