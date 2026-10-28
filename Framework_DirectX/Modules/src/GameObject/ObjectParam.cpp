#include "GameObject/ObjectParam.h"

ObjectParam::ObjectParam() 
	: START_POS{}
	, START_ROT{}
	, START_SCALE{}
	, MIN_POS{}
	, MAX_POS{}
{}
	 
ObjectParam::~ObjectParam()
{
}

void ObjectParam::moveFromJson(const std::string& actorKey, const std::string& sceneKey)
{
	using namespace DirectX::SimpleMath;
	auto scene = ISceneData::sceneData(sceneKey);
	const auto& p = scene.actorParam(actorKey);

	if (!m_BIsInitBaseData) {
		MESH_PATH = p.getWStringSafe("MESH_PATH");
		TEXT_PATH = p.getWStringSafe("TEXT_PATH");
		TILING_X = p.getFloatSafe("TILING_X");
		TILING_Y = p.getFloatSafe("TILING_Y");
		m_BIsInitBaseData = true;
	}

	if (!m_BIsInitSceneBoundingData) {
		START_POS = p.getVector3Safe("START_POS");
		START_ROT = p.getVector3Safe("START_ROT");
		START_SCALE = p.getVector3Safe("START_SCALE");
		MIN_POS = p.getVector3Safe("MIN_POS");
		MAX_POS = p.getVector3Safe("MAX_POS");
		MASS = p.getFloatSafe("MASS");
		FLOOR_FRICTION = p.getFloatSafe("FLOOR_FRICTION");
		AIR_DRAG = p.getFloatSafe("AIR_DRAG");
	}

}
