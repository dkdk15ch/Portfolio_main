#pragma once
#include "GameObject/ObjectParam.h"

namespace DirectX { class SoundEffect; }
namespace PlayersObjct {
	class Racket;
}

class PlayerParam : public ObjectParam
{
	friend class Player;
	friend PlayersObjct::Racket;
	friend class PlayerMovementComp;
public:
	PlayerParam();
	~PlayerParam() = default;

	const DirectX::SimpleMath::Vector3& racketScale() const { return RACKET_SCALE; }
	const DirectX::SimpleMath::Vector3& racketRelativePosRight() const { return RACKET_RELATIVEPOS_RIGHT; }
	const DirectX::SimpleMath::Vector3& racketRelativePosLeft() const { return RACKET_RELATIVEPOS_LEFT; }
	const DirectX::SimpleMath::Vector3& racketRelativePosTop() const { return RACKET_RELATIVEPOS_TOP; }
	float launchForce() const { return LAUNCH_FORCE; }
	float moveSpeedX() const { return MOVE_SPEED_X; }
	float moveSpeedZ() const { return MOVE_SPEED_Z; }
	float jumpSpeedY() const { return JUMP_SPEED_Y; }
	const DirectX::SimpleMath::Vector3& initTargetPos() const { return INIT_TARGET_POS; }
	float aimSpeed() const { return AIM_SPEED; }
	float aimLimitRight() const { return AIM_LIMIT_RIGHT; }
	float aimLimitLeft() const { return AIM_LIMIT_LEFT; }
	float aimLimitTop() const { return AIM_LIMIT_TOP; }
	float aimLimitDown() const { return AIM_LIMIT_DOWN; }
	//	sound
	const std::string& pathSE_Shot() const { return SE_SHOT; }
	float volumeShot() const { return VOL_SHOT; }
	const std::string& pathSE_Jump() const { return SE_JUMP; }
	float volumeJump() const { return VOL_JUMP; }
	const std::string& pathSE_SplitStep() const { return SE_SPLIT_STEP; }
	float volumeSplitStep() const { return VOL_SPLIT_STEP; }
	const std::string& pathSE_Damage() const { return SE_DAMAGE; }
	float volumeDamage() const { return VOL_DAMAGE; }
	//	state
	uint32_t jumpCancelFrame() const { return JUMP_CANCEL_FRAME; }
	float jumpCancelBonus() const { return JUMP_CANCEL_BONUS; }
	double takeBackTime() const { return TAKE_BACK_TIME; }
	uint32_t swingCancelFrame() const { return SWING_CANCEL_FRAME; }
	float hitResponseTimeScale() const { return HIT_RESPONSE_TIME_SCALE; }
	float canAimDistance()const { return CAN_AIM_DISTANCE; }
	//	mesh
	const std::wstring& playerMesh()const { return PLAYER_MESH; }
	//	animation
	const std::wstring& animaPathIdle() const { return ANIMA_PATH_IDLE; }
	bool animaLoopIdle() const { return ANIMA_LOOP_IDLE; }
	double animaScaleIdle() const { return ANIMA_SCALE_IDLE; }
	const std::wstring& animaPathRun() const { return ANIMA_PATH_RUN; }
	bool animaLoopRun() const { return ANIMA_LOOP_RUN; }
	double animaScaleRun() const { return ANIMA_SCALE_RUN; }
	const std::wstring& animaPathSwing() const { return ANIMA_PATH_SWING; }
	bool animaLoopSwing() const { return ANIMA_LOOP_SWING; }
	double animaScaleSwing() const { return ANIMA_SCALE_SWING; }
	const std::wstring& animaPathSmash() const { return ANIMA_PATH_SMASH; }
	bool animaLoopSmash() const { return ANIMA_LOOP_SMASH; }
	double animaScaleSmash() const { return ANIMA_SCALE_SMASH; }
	const std::wstring& animaPathJump() const { return ANIMA_PATH_JUMP; }
	bool animaLoopJump() const { return ANIMA_LOOP_JUMP; }
	double animaScaleJump() const { return ANIMA_SCALE_JUMP; }
	const std::wstring& animaPathHitRes() const { return ANIMA_PATH_HIT_RES; }
	bool animaLoopHitRes() const { return ANIMA_LOOP_HIT_RES; }
	double animaScaleHitRes() const { return ANIMA_SCALE_HIT_RES; }
	const std::wstring& animaPathDance() const { return ANIMA_PATH_DANCE; }
	bool animaLoopDance() const { return ANIMA_LOOP_DANCE; }
	double animaScaleDance() const { return ANIMA_SCALE_DANCE; }
	const std::wstring& animaPathGameOver() const { return ANIMA_PATH_GAME_OVER; }
	bool animaLoopGameOver() const { return ANIMA_LOOP_GAME_OVER; }
	double animaScaleGameOver() const { return ANIMA_SCALE_GAME_OVER; }

#pragma region PlayersObj
	//	racket mesh & offset
	const std::wstring& racketMeshPath() const { return RACKET_MESH_PATH; }
	const std::wstring& racketTextPath() const { return RACKET_TEXT_PATH; }
	float racketOffsetScale() const { return RACKET_OFFSET_SCALE; }
	const DirectX::SimpleMath::Vector3& racketOffsetPos() const { return RACKET_OFFSET_POS; }
	const DirectX::SimpleMath::Vector3& racketOffsetRot() const { return RACKET_OFFSET_ROT; }
	//	gut
	const std::wstring& gutPath() const { return GUT_PATH; }
	float gutTilingX() const { return GUT_TILING_X; }
	float gutTilingY() const { return GUT_TILING_Y; }
	const DirectX::SimpleMath::Vector3& gutPos() const { return GUT_POS; }
	const DirectX::SimpleMath::Vector3& gutRot() const { return GUT_ROT; }
	const DirectX::SimpleMath::Vector3& gutScale() const { return GUT_SCALE; }
	//	effect & hit
	const std::wstring& effectPath() const { return EFFECT_PATH; }
	const std::wstring& effectTextPath() const { return EFFECT_TEXT_PATH; }
	float showStart() const { return SHOW_START; }
	float showEnd() const { return SHOW_END; }
	double hitEffectTime() const { return HIT_EFFECT_TIME; }
	//	body collider
	const DirectX::SimpleMath::Vector3& bodyPos() const { return BODY_POS; }
	const DirectX::SimpleMath::Vector3& bodyScale() const { return BODY_SCALE; }
	//	ball case
	const std::wstring& ballCaseMesh() const { return BALL_CASE_MESH; }
	const std::wstring& ballCaseText() const { return BALL_CASE_TEXT; }
	const DirectX::SimpleMath::Vector3& ballCasePos() const { return BALL_CASE_POS; }
	const DirectX::SimpleMath::Vector3& ballCaseScale() const { return BALL_CASE_SCALE; }
	const DirectX::SimpleMath::Vector3& ballStockScale() const { return BALL_STOCK_SCALE; }
	//	launch correction
	float launchCorrectionStartZ() const { return LAUNCH_CORRECTION_START_Z; }
	float launchMinRatio() const { return LAUNCH_MIN_RATIO; }
	//	sound
	const std::string& sE_BllCase()const { return SE_BALL_CASE; }
	float volumeBallCase()const { return VOL_BALL_CASE; }
#pragma endregion


protected:
	virtual void moveFromJson(const std::string& actorKey, const std::string& sceneKey = "") override final;

private:

	//	racket
	DirectX::SimpleMath::Vector3 RACKET_SCALE;
	DirectX::SimpleMath::Vector3 RACKET_RELATIVEPOS_RIGHT;
	DirectX::SimpleMath::Vector3 RACKET_RELATIVEPOS_LEFT;
	DirectX::SimpleMath::Vector3 RACKET_RELATIVEPOS_TOP;
	float LAUNCH_FORCE;
	//	move
	float MOVE_SPEED_X;
	float MOVE_SPEED_Z;
	float JUMP_SPEED_Y;
	//	aim
	DirectX::SimpleMath::Vector3 INIT_TARGET_POS;
	float AIM_SPEED;
	float AIM_LIMIT_RIGHT;
	float AIM_LIMIT_LEFT;
	float AIM_LIMIT_TOP;
	float AIM_LIMIT_DOWN;
	//	sound
	std::string SE_SHOT;
	float VOL_SHOT;
	std::string SE_JUMP;
	float VOL_JUMP;
	std::string SE_SPLIT_STEP;
	float VOL_SPLIT_STEP;
	std::string SE_DAMAGE;
	float VOL_DAMAGE;
	//	state
	uint32_t JUMP_CANCEL_FRAME;
	float JUMP_CANCEL_BONUS;
	double TAKE_BACK_TIME;
	uint32_t SWING_CANCEL_FRAME;
	float HIT_RESPONSE_TIME_SCALE;
	float CAN_AIM_DISTANCE;
	//	mesh
	std::wstring PLAYER_MESH;
	//	animation
	std::wstring ANIMA_PATH_IDLE;
	bool ANIMA_LOOP_IDLE;
	double ANIMA_SCALE_IDLE;
	std::wstring ANIMA_PATH_RUN;
	bool ANIMA_LOOP_RUN;
	double ANIMA_SCALE_RUN;
	std::wstring ANIMA_PATH_SWING;
	bool ANIMA_LOOP_SWING;
	double ANIMA_SCALE_SWING;
	std::wstring ANIMA_PATH_SMASH;
	bool ANIMA_LOOP_SMASH;
	double ANIMA_SCALE_SMASH;
	std::wstring ANIMA_PATH_JUMP;
	bool ANIMA_LOOP_JUMP;
	double ANIMA_SCALE_JUMP;
	std::wstring ANIMA_PATH_HIT_RES;
	bool ANIMA_LOOP_HIT_RES;
	double ANIMA_SCALE_HIT_RES;
	std::wstring ANIMA_PATH_DANCE;
	bool ANIMA_LOOP_DANCE;
	double ANIMA_SCALE_DANCE;
	std::wstring ANIMA_PATH_GAME_OVER;
	bool ANIMA_LOOP_GAME_OVER;
	double ANIMA_SCALE_GAME_OVER;

#pragma region PlayersObj
	//	racket mesh & offset
	std::wstring RACKET_MESH_PATH;
	std::wstring RACKET_TEXT_PATH;
	float RACKET_OFFSET_SCALE;
	DirectX::SimpleMath::Vector3 RACKET_OFFSET_POS;
	DirectX::SimpleMath::Vector3 RACKET_OFFSET_ROT;
	//	gut
	std::wstring GUT_PATH;
	float GUT_TILING_X;
	float GUT_TILING_Y;
	DirectX::SimpleMath::Vector3 GUT_POS;
	DirectX::SimpleMath::Vector3 GUT_ROT;
	DirectX::SimpleMath::Vector3 GUT_SCALE;
	//	effect
	std::wstring EFFECT_PATH;
	std::wstring EFFECT_TEXT_PATH;
	float SHOW_START;
	float SHOW_END;
	double HIT_EFFECT_TIME;
	//	body collider
	DirectX::SimpleMath::Vector3 BODY_POS;
	DirectX::SimpleMath::Vector3 BODY_SCALE;
	//	ball case
	std::wstring BALL_CASE_MESH;
	std::wstring BALL_CASE_TEXT;
	DirectX::SimpleMath::Vector3 BALL_CASE_POS;
	DirectX::SimpleMath::Vector3 BALL_CASE_SCALE;
	DirectX::SimpleMath::Vector3 BALL_STOCK_SCALE;
	//	launch correction
	float LAUNCH_CORRECTION_START_Z;
	float LAUNCH_MIN_RATIO;
	//	sound
	std::string SE_BALL_CASE;
	float VOL_BALL_CASE;
#pragma endregion
};