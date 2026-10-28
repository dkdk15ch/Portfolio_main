#include "GameObject/Param/PlayerParam.h"
#include "InOutput/Audio.h"
#include "StringHelper.h"

PlayerParam::PlayerParam()
	: ObjectParam()
	, RACKET_SCALE{}
	, RACKET_RELATIVEPOS_RIGHT{}
	, RACKET_RELATIVEPOS_LEFT{}
	, RACKET_RELATIVEPOS_TOP{}
	, LAUNCH_FORCE{}
	, MOVE_SPEED_X{}
	, MOVE_SPEED_Z{}
	, JUMP_SPEED_Y{}
	, INIT_TARGET_POS{}
	, AIM_SPEED{}
	, AIM_LIMIT_RIGHT{}
	, AIM_LIMIT_LEFT{}
	, AIM_LIMIT_TOP{}
	, AIM_LIMIT_DOWN{}
	, SE_SHOT{}
	, JUMP_CANCEL_FRAME{}
	, JUMP_CANCEL_BONUS{}
	, TAKE_BACK_TIME{}
	, SWING_CANCEL_FRAME{}
	, HIT_RESPONSE_TIME_SCALE{}
{
}

void PlayerParam::moveFromJson(const std::string& actorKey, const std::string& sceneKey)
{
	ObjectParam::moveFromJson(actorKey, sceneKey);

	using ActorParam = IJsonData::ActorParam;
	const auto& param = IActorData::actorParam(actorKey);
	//	racket
	RACKET_SCALE = param.getVector3Safe("RACKET_SCALE");
	RACKET_RELATIVEPOS_RIGHT = param.getVector3Safe("RACKET_RELATIVEPOS_RIGHT");
	RACKET_RELATIVEPOS_LEFT = param.getVector3Safe("RACKET_RELATIVEPOS_LEFT");
	RACKET_RELATIVEPOS_TOP = param.getVector3Safe("RACKET_RELATIVEPOS_TOP");
	LAUNCH_FORCE = param.getFloatSafe("LAUNCH_FORCE");
	//	move
	MOVE_SPEED_X = param.getFloatSafe("MOVE_SPEED_X");
	MOVE_SPEED_Z = param.getFloatSafe("MOVE_SPEED_Z");
	JUMP_SPEED_Y = param.getFloatSafe("JUMP_SPEED_Y");
	//	aim
	INIT_TARGET_POS = param.getVector3Safe("INIT_TARGET_POS");
	AIM_SPEED = param.getFloatSafe("AIM_SPEED");
	AIM_LIMIT_RIGHT = param.getFloatSafe("AIM_LIMIT_RIGHT");
	AIM_LIMIT_LEFT = param.getFloatSafe("AIM_LIMIT_LEFT");
	AIM_LIMIT_TOP = param.getFloatSafe("AIM_LIMIT_TOP");
	AIM_LIMIT_DOWN = param.getFloatSafe("AIM_LIMIT_DOWN");
	//	sound 
	SE_SHOT = param.getStringSafe("SE_SHOT");
	VOL_SHOT = param.getFloatSafe("VOL_SHOT");
	Audio::loadSound(SE_SHOT);
	SE_JUMP= param.getStringSafe("SE_JUMP");
	VOL_JUMP = param.getFloatSafe("VOL_JUMP");
	Audio::loadSound(SE_JUMP);
	SE_SPLIT_STEP = param.getStringSafe("SE_SPLIT_STEP");
	VOL_SPLIT_STEP = param.getFloatSafe("VOL_SPLIT_STEP");
	Audio::loadSound(SE_SPLIT_STEP);
	SE_DAMAGE = param.getStringSafe("SE_DAMAGE");
	VOL_DAMAGE = param.getFloatSafe("VOL_DAMAGE");
	Audio::loadSound(SE_DAMAGE);
	//	state
	JUMP_CANCEL_FRAME = param.getUInt32Safe("JUMP_CANCEL_FRAME");
	JUMP_CANCEL_BONUS = param.getFloatSafe("JUMP_CANCEL_BONUS");
	TAKE_BACK_TIME = param.getDoubleSafe("TAKE_BACK_TIME");
	SWING_CANCEL_FRAME = param.getUInt32Safe("SWING_CANCEL_FRAME");
	HIT_RESPONSE_TIME_SCALE = param.getFloatSafe("HIT_RESPONSE_TIME_SCALE");
	CAN_AIM_DISTANCE = param.getFloatSafe("CAN_AIM_DISTANCE");
	//	mesh
	PLAYER_MESH = param.getWStringSafe("PLAYER_MESH");
	//	animation
	ANIMA_PATH_IDLE = param.getWStringSafe("ANIMA_PATH_IDLE");
	ANIMA_LOOP_IDLE = param.getBoolSafe("ANIMA_LOOP_IDLE");
	ANIMA_SCALE_IDLE = param.getDoubleSafe("ANIMA_SCALE_IDLE");
	ANIMA_PATH_RUN = param.getWStringSafe("ANIMA_PATH_RUN");
	ANIMA_LOOP_RUN = param.getBoolSafe("ANIMA_LOOP_RUN");
	ANIMA_SCALE_RUN = param.getDoubleSafe("ANIMA_SCALE_RUN");
	ANIMA_PATH_SWING = param.getWStringSafe("ANIMA_PATH_SWING");
	ANIMA_LOOP_SWING = param.getBoolSafe("ANIMA_LOOP_SWING");
	ANIMA_SCALE_SWING = param.getDoubleSafe("ANIMA_SCALE_SWING");
	ANIMA_PATH_SMASH = param.getWStringSafe("ANIMA_PATH_SMASH");
	ANIMA_LOOP_SMASH = param.getBoolSafe("ANIMA_LOOP_SMASH");
	ANIMA_SCALE_SMASH = param.getDoubleSafe("ANIMA_SCALE_SMASH");
	ANIMA_PATH_JUMP = param.getWStringSafe("ANIMA_PATH_JUMP");
	ANIMA_LOOP_JUMP = param.getBoolSafe("ANIMA_LOOP_JUMP");
	ANIMA_SCALE_JUMP = param.getDoubleSafe("ANIMA_SCALE_JUMP");
	ANIMA_PATH_HIT_RES = param.getWStringSafe("ANIMA_PATH_HIT_RES");
	ANIMA_LOOP_HIT_RES = param.getBoolSafe("ANIMA_LOOP_HIT_RES");
	ANIMA_SCALE_HIT_RES = param.getDoubleSafe("ANIMA_SCALE_HIT_RES");
	ANIMA_PATH_DANCE = param.getWStringSafe("ANIMA_PATH_DANCE");
	ANIMA_LOOP_DANCE = param.getBoolSafe("ANIMA_LOOP_DANCE");
	ANIMA_SCALE_DANCE = param.getDoubleSafe("ANIMA_SCALE_DANCE");
	ANIMA_PATH_GAME_OVER = param.getWStringSafe("ANIMA_PATH_GAME_OVER");
	ANIMA_LOOP_GAME_OVER = param.getBoolSafe("ANIMA_LOOP_GAME_OVER");
	ANIMA_SCALE_GAME_OVER = param.getDoubleSafe("ANIMA_SCALE_GAME_OVER");

#pragma region PlayersObj
	const auto& objParam = IActorData::actorParam("PlayersObj");

	//	racket mesh & offset
	RACKET_MESH_PATH = objParam.getWStringSafe("RACKET_MESH_PATH");
	RACKET_TEXT_PATH = objParam.getWStringSafe("RACKET_TEXT_PATH");
	RACKET_OFFSET_SCALE = objParam.getFloatSafe("RACKET_OFFSET_SCALE");
	RACKET_OFFSET_POS = objParam.getVector3Safe("RACKET_OFFSET_POS");
	RACKET_OFFSET_ROT = objParam.getVector3Safe("RACKET_OFFSET_ROT");
	//	gut
	GUT_PATH = objParam.getWStringSafe("GUT_PATH");
	GUT_TILING_X = objParam.getFloatSafe("GUT_TILING_X");
	GUT_TILING_Y = objParam.getFloatSafe("GUT_TILING_Y");
	GUT_POS = objParam.getVector3Safe("GUT_POS");
	GUT_ROT = objParam.getVector3Safe("GUT_ROT");
	GUT_SCALE = objParam.getVector3Safe("GUT_SCALE");
	//	effect
	EFFECT_PATH = objParam.getWStringSafe("EFFECT_PATH");
	EFFECT_TEXT_PATH = objParam.getWStringSafe("EFFECT_TEXT_PATH");
	SHOW_START = objParam.getFloatSafe("SHOW_START");
	SHOW_END = objParam.getFloatSafe("SHOW_END");
	HIT_EFFECT_TIME = objParam.getDoubleSafe("HIT_EFFECT_TIME");
	//	body collider
	BODY_POS = objParam.getVector3Safe("BODY_POS");
	BODY_SCALE = objParam.getVector3Safe("BODY_SCALE");
	//	ball case
	BALL_CASE_MESH = objParam.getWStringSafe("BALL_CASE_MESH");
	BALL_CASE_TEXT = objParam.getWStringSafe("BALL_CASE_TEXT");
	BALL_CASE_POS = objParam.getVector3Safe("BALL_CASE_POS");
	BALL_CASE_SCALE = objParam.getVector3Safe("BALL_CASE_SCALE");
	BALL_STOCK_SCALE = objParam.getVector3Safe("BALL_STOCK_SCALE");
	//	launch correction
	LAUNCH_CORRECTION_START_Z = objParam.getFloatSafe("LAUNCH_CORRECTION_START_Z");
	LAUNCH_MIN_RATIO = objParam.getFloatSafe("LAUNCH_MIN_RATIO");
	//	sound
	SE_BALL_CASE = objParam.getStringSafe("SE_BALL_CASE");
	Audio::loadSound(SE_BALL_CASE);
	VOL_BALL_CASE = objParam.getFloatSafe("VOL_BALL_CASE");
#pragma endregion

 }
