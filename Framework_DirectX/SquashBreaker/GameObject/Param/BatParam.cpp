#include "GameObject/Param/BatParam.h"
#include "InOutput/Audio.h"

void BatParam::moveFromJson(const std::string& actorKey, const std::string& sceneKey)
{
	ObjectParam::moveFromJson(actorKey, sceneKey);

	const auto& param = IActorData::actorParam(actorKey);

	BAT_SCALE = param.getFloatSafe("BAT_SCALE");
	BAT_ROT = param.getVector3Safe("BAT_ROT");
	BAT_MESH = param.getWStringSafe("BAT_MESH");
	BAT_COLLIDER_POS = param.getVector3Safe("BAT_COLLIDER_POS");
	BAT_COLLIDER_ROT = param.getVector3Safe("BAT_COLLIDER_ROT");
	BAT_COLLIDER_SCALE = param.getVector3Safe("BAT_COLLIDER_SCALE");
	//	animation
	ANIMA_PATH_BAT_FLYING = param.getWStringSafe("ANIMA_PATH_BAT_FLYING");
	ANIMA_LOOP_BAT_FLYING = param.getBoolSafe("ANIMA_LOOP_BAT_FLYING");
	ANIMA_SCALE_BAT_FLYING = param.getDoubleSafe("ANIMA_SCALE_BAT_FLYING");
	ANIMA_PATH_BAT_DEATH = param.getWStringSafe("ANIMA_PATH_BAT_DEATH");
	ANIMA_LOOP_BAT_DEATH = param.getBoolSafe("ANIMA_LOOP_BAT_DEATH");
	ANIMA_SCALE_BAT_DEATH = param.getDoubleSafe("ANIMA_SCALE_BAT_DEATH");
	ANIMA_PATH_BAT_ATTACK = param.getWStringSafe("ANIMA_PATH_BAT_ATTACK");
	ANIMA_LOOP_BAT_ATTACK = param.getBoolSafe("ANIMA_LOOP_BAT_ATTACK");
	ANIMA_SCALE_BAT_ATTACK = param.getDoubleSafe("ANIMA_SCALE_BAT_ATTACK");
	//	sound
	SE_ATTACK = param.getStringSafe("SE_ATTACK");
	VOL_ATTACK = param.getFloatSafe("VOL_ATTACK");
	Audio::loadSound(SE_ATTACK);
	SE_DEAD = param.getStringSafe("SE_DEAD");
	VOL_DEAD = param.getFloatSafe("VOL_DEAD");
	Audio::loadSound(SE_DEAD);
	//	move
	MOVE_SPEED = param.getFloatSafe("MOVE_SPEED");
	SPAWN_TIME = param.getDoubleSafe("SPAWN_TIME");
	ATTACK_INTRVAL = param.getDoubleSafe("ATTACK_INTRVAL");
	DESPAWN_TIME = param.getDoubleSafe("DESPAWN_TIME");
	BORN_OFFSET_POSY = param.getFloatSafe("BORN_OFFSET_POSY");
	BORN_OFFSET_ROTX = param.getFloatSafe("BORN_OFFSET_ROTX");
	LUNCH_FORCE = param.getFloatSafe("LUNCH_FORCE");
}