#pragma once
#include "GameObject/GameBase.h"
#include "GameObject/UniqueName.h"

class RenderCompBase;

class PlaneObject : public GameBase , public UniqueName
{
public:
	PlaneObject(const PrimitiveObjDesc& desc, std::string name = "Plane");
	virtual ~PlaneObject();

private:
	RenderCompBase* m_Render;
};