#pragma once
#include "GameObject/Item/GameItem.h"
#include "Delegate.h"

class RenderCompBase;
class ClearItem : public GameItem {
public:
	ClearItem();
	virtual ~ClearItem() = default;

	Delegate::Singlecast<void(void)> OnGain;

	void enable()override final;

	bool isGained() const { return m_BIsGained; }
	void implOnHit(const HitInfo& info) override final;

protected:
	void gain() override final;
private:
	bool m_BIsGained = false;
};