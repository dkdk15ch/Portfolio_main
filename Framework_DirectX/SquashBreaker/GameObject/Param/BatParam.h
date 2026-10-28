#pragma once
#include "GameObject/ObjectParam.h"
#include <string>

class BatParam : public ObjectParam {
	friend class Bat;
	friend class BatMovementComp;
public:
	float batScale() const { return BAT_SCALE; }
	DirectX::SimpleMath::Vector3 batRot() const { return BAT_ROT; }
	std::wstring batMesh() const { return BAT_MESH; }
	DirectX::SimpleMath::Vector3 batColliderPos() const { return BAT_COLLIDER_POS; }
	DirectX::SimpleMath::Vector3 batColliderRot() const { return BAT_COLLIDER_ROT; }
	DirectX::SimpleMath::Vector3 batColliderScale() const { return BAT_COLLIDER_SCALE; }
	//	animation
	const std::wstring& pathFlying() const { return ANIMA_PATH_BAT_FLYING; }
	bool loopFlying() const { return ANIMA_LOOP_BAT_FLYING; }
	double scaleFlying() const { return ANIMA_SCALE_BAT_FLYING; }
	const std::wstring& pathDeath() const { return ANIMA_PATH_BAT_DEATH; }
	bool loopDeath() const { return ANIMA_LOOP_BAT_DEATH; }
	double scaleDeath() const { return ANIMA_SCALE_BAT_DEATH; }
	const std::wstring& pathAttack() const { return ANIMA_PATH_BAT_ATTACK; }
	bool loopAttack() const { return ANIMA_LOOP_BAT_ATTACK; }
	double scaleAttack() const { return ANIMA_SCALE_BAT_ATTACK; }
	//	sound
	const std::string& sE_Attack()const { return SE_ATTACK; }
	float volumeAttack()const { return VOL_ATTACK; }
	const std::string& sE_Dead()const { return SE_DEAD; }
	float volumeDead()const { return VOL_DEAD; }
	//	move
	float moveSpeed() const { return MOVE_SPEED; }
	double spawnTime() const { return SPAWN_TIME; }
	double attackIntrval() const { return ATTACK_INTRVAL; }
	double despawnTime() const { return DESPAWN_TIME; }
	float bornOffsetPosY() const { return BORN_OFFSET_POSY; }
	float bornOffsetRotX() const { return BORN_OFFSET_ROTX; }
	float lunchForce()const { return LUNCH_FORCE; }

protected:
	virtual void moveFromJson(const std::string& actorKey, const std::string& sceneKey = "") override final;

private:
	float BAT_SCALE;
	DirectX::SimpleMath::Vector3 BAT_ROT;
	std::wstring BAT_MESH;
	DirectX::SimpleMath::Vector3 BAT_COLLIDER_POS;
	DirectX::SimpleMath::Vector3 BAT_COLLIDER_ROT;
	DirectX::SimpleMath::Vector3 BAT_COLLIDER_SCALE;
	//	animation
	std::wstring ANIMA_PATH_BAT_FLYING;
	bool ANIMA_LOOP_BAT_FLYING;
	double ANIMA_SCALE_BAT_FLYING;
	std::wstring ANIMA_PATH_BAT_DEATH;
	bool ANIMA_LOOP_BAT_DEATH;
	double ANIMA_SCALE_BAT_DEATH;
	std::wstring ANIMA_PATH_BAT_ATTACK;
	bool ANIMA_LOOP_BAT_ATTACK;
	double ANIMA_SCALE_BAT_ATTACK;
	//	sound
	std::string SE_ATTACK;
	float VOL_ATTACK;
	std::string SE_DEAD;
	float VOL_DEAD;
	//	move
	float MOVE_SPEED;
	double SPAWN_TIME;
	double ATTACK_INTRVAL;
	double DESPAWN_TIME;
	float BORN_OFFSET_POSY;
	float BORN_OFFSET_ROTX;
	float LUNCH_FORCE;
};