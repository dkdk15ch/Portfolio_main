#include "GameObject/Param/BallParam.h"
#include "InOutput/Audio.h"


void BallParam::moveFromJson(const std::string& actorKey, const std::string& sceneKey)
{
	ObjectParam::moveFromJson(actorKey, sceneKey);

	using ActorParam = IJsonData::ActorParam;
	const auto& param = IActorData::actorParam("Ball");

	//	physic
	DEFAULT_TIME_SCALE = param.getDoubleSafe("DEFAULT_TIME_SCALE");
	SPEED_MIN = param.getFloatSafe("SPEED_MIN");
	GRAVITY = param.getFloatSafe("GRAVITY");
	IGNORE_GRAVITY_HEIGHT = param.getFloatSafe("IGNORE_GRAVITY_HEIGHT");
	COR_WALL = param.getFloatSafe("COR_WALL");
	COR_BLOCK = param.getFloatSafe("COR_BLOCK");
	FRONT_HIT_VERTICAL_DAMPING = param.getFloatSafe("FRONT_HIT_VERTICAL_DAMPING");
	//	sound
	SE_BOUND = param.getStringSafe("SE_BOUND");
	Audio::loadSound(SE_BOUND);
	VOL_BOUND = param.getFloatSafe("VOL_BOUND");
}
