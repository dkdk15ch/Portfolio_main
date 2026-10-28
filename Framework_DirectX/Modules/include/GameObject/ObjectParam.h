#pragma once
#include "Util/IJsonData.h" 
#include <SimpleMath.h>

class MovementComp;
class ObjectParamManager;

class ObjectParam : public IActorData
{
	friend ObjectParamManager;
public:
	ObjectParam();
	virtual ~ObjectParam();

#pragma region getter
	bool bIsInitBaseData() const { return m_BIsInitBaseData; }
	bool bIsInitSceneBoundingData() const { return m_BIsInitSceneBoundingData; }
	const DirectX::SimpleMath::Vector3& startPos() const { return START_POS; }
	const DirectX::SimpleMath::Vector3& startRot() const { return START_ROT; }
	const DirectX::SimpleMath::Vector3& startScale() const { return START_SCALE; }
	const std::wstring& meshPath() const { return MESH_PATH; }
	const std::wstring& textPath() const { return TEXT_PATH; }
	float tilingX() const { return TILING_X; }
	float tilingY() const { return TILING_Y; }
	float mass()const { return MASS; }
	float floorFriction()const { return FLOOR_FRICTION; }
	float airDrag()const { return AIR_DRAG; }
	const DirectX::SimpleMath::Vector3& minPos()const { return MIN_POS; }
	const DirectX::SimpleMath::Vector3& maxPos()const 
	 { return MAX_POS; }
#pragma endregion

protected:
	virtual void moveFromJson(const std::string& actorKey, const std::string& sceneKey) override;

	bool m_BIsInitBaseData = false;
	bool m_BIsInitSceneBoundingData = false;
	
	//	init transform
	DirectX::SimpleMath::Vector3 START_POS;
	DirectX::SimpleMath::Vector3 START_ROT;
	DirectX::SimpleMath::Vector3 START_SCALE;
	//	mesh・texture
	std::wstring MESH_PATH;
	std::wstring TEXT_PATH;
	float TILING_X;
	float TILING_Y;
	//	Physics
	float MASS;
	float FLOOR_FRICTION;
	float AIR_DRAG;
	//	移動制限
	DirectX::SimpleMath::Vector3 MIN_POS;
	DirectX::SimpleMath::Vector3 MAX_POS;

private:
};

