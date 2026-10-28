#pragma once
#include "GameObject/ObjectParam.h"
#include <string>

class ClearItemParam : public ObjectParam {
public:
	const std::string& sE_Spawn()const { return SE_SPAWN; }
	float volumeSpawn()const { return VOL_SPAWN; }
	const std::string& sE_Gain()const { return SE_GAIN; }
	float volumeGain()const { return VOL_GAIN; }
	const DirectX::SimpleMath::Vector3& colliderOffsetPos()const { return COL_OFFSET_POS; }
	const DirectX::SimpleMath::Vector3& colliderOffsetScale()const { return COL_OFFSET_SCALE; }

	virtual void moveFromJson(const std::string& actorKey, const std::string& sceneKey = "") override final;

private:
	std::string SE_SPAWN;
	float VOL_SPAWN;
	std::string SE_GAIN;
	float VOL_GAIN;
	DirectX::SimpleMath::Vector3 COL_OFFSET_POS;
	DirectX::SimpleMath::Vector3 COL_OFFSET_SCALE;
};