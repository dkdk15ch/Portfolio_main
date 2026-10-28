#pragma once
#include "GameObject/GameBase.h"
#include "GameObject/UniqueName.h"
#include <SimpleMath.h>
#include "GameObject/Param/PlayerParam.h"

class PlayerMovementComp;
class MovementComp;
class ModelRenderComp;
class BoxColliderComp;
class IHitResponseFactory;
namespace PlayersObjct {
	class Racket;
}


class Player : public GameBase, public UniqueName
{
	friend PlayersObjct::Racket;
	friend PlayerMovementComp;
public:
	Player(std::string name = "Player", bool bIsPlayerOnly = false);
	~Player();
	virtual void term() override;

protected:
	virtual void implupdate(uint32_t frameIndex, double deltaTime) override;

	//	ref Components
	PlayerMovementComp* m_Mover;
	MovementComp* m_RacketMover;
	MovementComp* m_ShadowMover;
	BoxColliderComp* m_RacketCollider;
	GameBase* m_ColliderObj;
	GameBase* m_CrossHair;
};


