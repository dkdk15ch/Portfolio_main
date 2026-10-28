#include "GameObject/BlockWall.h"
#include "GameObject/Block.h"
#include "GameObject/ObjectParam.h"
#include "GameObject/Param/BlockWallParam.h"
#include "GameObject/PrimitiveMesh.h"
#include "GameObject/Enemy/Bat.h"
#include "Manager/ObjectParamManager.h"
#include "Manager/LevelManager.h"
#include "Level/Level.h"
#include "Component/MovementComp.h"
#include "Component/Render/RenderCompBase.h"
#include "Component/Render/InstancingRenderComp.h"
#include "Component/Collider/BoxColliderComp.h"
#include "Component/TransformComp.h"
#include "Strategy/IHitResponse.h"
#include "Util/Loader.h"
#include "SafeDelete.h"
#include <format>
#include <utility>
#include "GameObject/Spawner.h"
#include "GameObject/Item/ClearItem.h"
#include "MyRand.h"
#include "GameObject/Param/CollisionLayerParam.h"


namespace {
    const std::string baseName = "BlockWall";

    struct BallHitResponse : public IHitResponse {
        BallHitResponse(BlockWall& caller) : m_Wall(caller)
        {
        };

        void onHit(GameBase& owner, const HitInfo& info) override
        {
            const auto hitIdx = info.instanceIndex;
            const auto lastIdx = m_Wall.instanceNum() - 1;
            auto block = m_Wall.m_BlockList[hitIdx].get();
            auto& data = m_Wall.m_InstanceData;
            std::swap(data[hitIdx], data[lastIdx]);
            std::swap(m_Wall.m_BlockList[hitIdx], m_Wall.m_BlockList[lastIdx]);

            m_Wall.m_Render->setMovable(true);
            m_Wall.setInstanceNum(lastIdx);
            m_Wall.m_ResetMovableTimer.startWait(2);

            if (block) {
                block->destroy();
            }
        };

    private:
        BlockWall& m_Wall;
    };


    struct HitResponseFactory {
        static IHitResponse* create(const CollisionTag& otherLayer, BlockWall& caller) {
            switch (otherLayer) {
            case CollisionTag::EnemyBall:
            case CollisionTag::Ball: {
                return new BallHitResponse(caller);
            }
            }
            return nullptr;
        }
    };
}


BlockWall::BlockWall(std::string uniqueName)
    : GameBase(std::move(makeUniqueName(uniqueName)))
    , m_EnemySpawner(nullptr)
{
    std::wstring nameW = StringToWString(name());
    std::string key = baseName;
    auto mgr = ObjectParamManager::m_Instance;
    auto param = mgr->getParam(key);
    if (!param) {
        mgr->addParam(key, key, MAKE_UNIQUE_LOG<BlockWallParam>());
        param = mgr->getParam(key);
    }
    auto p = dynamic_cast<const BlockWallParam*>(param);
    assert(p);
    setParam(p);
    if (!p->m_BIsInitSceneBoundingData) {
        mgr->updateSceneBindingData(key);
    }
    initStaticTransform(p->START_POS, p->START_ROT, p->START_SCALE);
    //  インスタンス数セット(renderより先に設定)
    uint32_t instNum = static_cast<uint32_t>(p->totalBlock());
    setInstanceNum(instNum);
    //  ブロック初期化
    initBlock();
    // render
    addComponent(MAKE_UNIQUE_LOG<InstancingRenderComp>(this));
    m_Render = static_cast<InstancingRenderComp*>(getComponent(CompType::RenderComp));
    auto priMesh = PrimitiveMesh::m_Instance;
    m_Render->setMesh(priMesh->cubeMesh());
    Loader::texture(
        p->TEXT_PATH.c_str(),
        m_Render->material(),
        nameW,
        DirectX::XMFLOAT2(
            p->TILING_X, 
            p->TILING_Y)
    );
    m_Render->setMovable(false);
    // collider
    addComponent(MAKE_UNIQUE_LOG<BoxColliderComp>(this, ColliderCompBase::Shape::AABB));
    m_Collider = static_cast<BoxColliderComp*>(getComponent(CompType::ColliderComp));
    m_Collider->setLayer(CollisionTag::Block);
    auto layerParam = CollisionLayerParam::layerParam();
    m_Collider->targetLayer().setTag(layerParam->blockTarget());
}

BlockWall::~BlockWall()
{
}

void BlockWall::term()
{
}

void BlockWall::implPreupdate(uint32_t frameIndex, double deltaTime)
{
    if (m_ResetMovableTimer.update()) return;

    m_Render->setMovable(false);
}

void BlockWall::implStart()
{
    m_Render->setMovable(true);

    auto p = dynamic_cast<const BlockWallParam*>(m_Param);
    const int row =  p->row;
    const int col = p->column;

    int clearIdx = MyRand::range(0, (row * col) -1);
    m_BlockList[clearIdx]->bind(BlockIn::ITEM_CLEAR);

    const int maxNum = m_BlockList.size();
    int MaxEnemy = static_cast<int>(maxNum * 0.4f);
    int count = 0;
    int safeC = 0;
    while (count < MaxEnemy && safeC < maxNum) {
        safeC++;
        int idx = MyRand::range(0, maxNum -1);
        if (m_BlockList[idx]->in() != BlockIn::NONE) { continue; }

        m_BlockList[idx]->bind(BlockIn::ENEMY_BAT);
        count++;
    }
}

void BlockWall::implPostupdate(uint32_t frameIndex, double deltaTime)
{
    for (auto& b : m_BlockList) {
        b->postUpdate(frameIndex, deltaTime);
    }
}

void BlockWall::implOnHit(const HitInfo& info)
{
    const auto& layer = info.other->layer();

    //	レイヤーごとの応答
    auto hitResponse = HitResponseFactory::create(layer, *this);
    if (hitResponse) {
        hitResponse->onHit(*this, info);
        SafeDelete(hitResponse);
    }
}

void BlockWall::initBlock()
{
    using namespace DirectX::SimpleMath;
    using namespace DirectX;

    const auto& t = transform();
    const float blockSizeX = t->scale().x * 2;
    const float blockSizeY = t->scale().y * 2;
    const float blockSizeZ = t->scale().z * 2;
    auto p = dynamic_cast<const BlockWallParam*>(m_Param);
    m_InstanceData.reserve(instanceNum());
    m_InstanceData.clear();

    for (uint32_t i = 0; i < instanceNum(); i++) {
        std::string name = std::format("Block{:02}", i);
        auto block = MAKE_UNIQUE_LOG<Block>(name);
        //  transform
        const int row = i % p->row;
        const int col = (i / p->row) % p->column;
        const int depth = i / (p->column * p->row);
        const float offsetX = row * blockSizeX;
        const float offsetY = col * blockSizeY;
        const float offsetZ = depth * blockSizeZ;
        Vector3 pos = Vector3(
            t->position().x + offsetX,
            t->position().y + offsetY,
            t->position().z - offsetZ);
        Vector3 rot = t->rotation();
        Vector3 scale = t->scale();
        // color
        const float colorR = (p->row > 1) ? (float)row / (p->row - 1) : 0.0f;
        const float colorG = (p->column > 1) ? (float)col / (p->column - 1) : 0.0f;
        const float colorB = (p->depth > 1) ? (float)depth / (p->depth - 1) : 0.0f;
        XMVECTOR color = { colorR,colorG,colorB,1.0f };
        //  初期状態セット
        block->initStaticTransform(pos, rot, scale);
        //block->setColor(color);
        m_BlockList.push_back(std::move(block));
        m_InstanceData.push_back(m_BlockList[i]->instanceDataList()[0]);
    } 
}

void BlockWall::allDestory()
{
#ifdef _DEBUG
    for (auto& b : m_BlockList) {
        b->destroy();
        b->disableNextFrame();
    }

    disableNextFrame();
#endif // _DEBUG
}


