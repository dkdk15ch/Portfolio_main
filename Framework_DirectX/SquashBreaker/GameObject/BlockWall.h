#pragma once
#include "GameObject/GameBase.h"
#include "GameObject/UniqueName.h"
#include <memory>
#include <vector>
#include "ConstantBuffer.h"
#include "Util/FrameDelay.h"

class BoxColliderComp;
class InstancingRenderComp;
class ObjectParam;
class Block;
class FrameDelay;
class Spawner;

namespace {
	struct BlockHitResponse;
	struct BallHitResponse;
	struct PlayerHitResponse;
}

class BlockWall : public GameBase, public UniqueName
{
	friend struct BallHitResponse;

public:
	BlockWall(std::string name = "BlockWall");
	~BlockWall();

	void term() override;

protected:
	void implPreupdate(uint32_t frameIndex, double deltaTime)override final;
	void implStart() override final;
	void implPostupdate(uint32_t frameIndex, double deltaTime)override final;

	void implOnHit(const HitInfo& info)override final;

private:
	void initInstanceParam();
	void initBlock();
	BoxColliderComp* m_Collider;
	InstancingRenderComp* m_Render;
	std::vector<LogPtr<Block>> m_BlockList;
	FrameDelay m_ResetMovableTimer;
	Spawner* m_EnemySpawner;
	Spawner* m_ClearItemSpawner;
	uint32_t m_SpawnIndex;

public:
	void allDestory();
};

