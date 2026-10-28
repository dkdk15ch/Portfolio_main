#pragma once
#include "GameObject/GameBase.h"
#include "Util/IJsonData.h"
#include <SimpleMath.h>
#include <map>

#include "PointerHelper.h"

class Model;
class DebugMoveComp;

class TestAnimation : public GameBase
{
public:
	TestAnimation(const std::string& name);
	virtual ~TestAnimation() ;
	void implStart()override final;

	std::string m_ModelName;
	std::string m_AnimationName;

	LogPtr<Model> m_TestModel;
	DebugMoveComp* m_Mover;

protected:
	virtual void implPreupdate(uint32_t frameIndex, double deltaTime) override final;
	virtual void implupdate(uint32_t frameIndex, double deltaTime) override final;

	virtual void implOnHit(const HitInfo& other) override final;

private:
	bool m_BToggleTimeScale = false;

	GameBase* m_RacketObj;
	class MovementComp* m_RacketMover;
	MovementComp* m_RacketStringMover;
};

