#pragma once
#include "GameObject/GameBase.h"
#include "GameObject/UniqueName.h"

class SphereColliderComp;
class GameItem : public GameBase, public UniqueName
{
public:
	GameItem(std::string uniqueName = "Item");
	virtual ~GameItem();

	virtual void implOnHit(const HitInfo& info) override;

protected:
	virtual void gain() = 0;

private:

};