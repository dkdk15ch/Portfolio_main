#pragma once
#include "GameObject/GameBase.h"
#include "GameObject/UniqueName.h"

class RenderCompBase;

class SphereObject : public GameBase, public UniqueName
{
public:
	SphereObject(std::string name = "Sphere", uint32_t instanceNum = 1, const std::string jsonActorKey = "");
	virtual ~SphereObject();

private:
	RenderCompBase* m_Render;
};
