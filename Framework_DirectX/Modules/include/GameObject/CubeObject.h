#pragma once
#include "GameObject/GameBase.h"
#include "GameObject/UniqueName.h"

class RenderCompBase;

class CubeObject : public GameBase, public UniqueName
{
public:
	CubeObject(std::string uniqueName = "Cube", uint32_t instanceNum = 1, const std::string jsonActorKey = "");
	virtual ~CubeObject() = default;

private:
	RenderCompBase* m_Render;
};
