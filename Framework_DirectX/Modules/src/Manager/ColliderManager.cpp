#include "Manager/ColliderManager.h"
#include "Component/Collider/ColliderCompBase.h"
#include "GameObject/GameBase.h"

ColliderManager* ColliderManager::m_Instance = nullptr;

namespace {
    static uint32_t MAX_LIST_SIZE = 50;

    bool CheckLayer(const ColliderCompBase& c1, const ColliderCompBase& c2) {
        //  レイヤーチェック
        const auto& layer_c1 = c1.layer();
        const auto& layer_c2 = c2.layer();
        //  無効かチェック
        const bool isDisable_c1 = (static_cast<uint32_t>(layer_c1) & static_cast<uint32_t>(CollisionTag::disable)) != 0;
        const bool isDisable_c2 = (static_cast<uint32_t>(layer_c2) & static_cast<uint32_t>
           (CollisionTag::disable)) != 0;
        if (isDisable_c1 || isDisable_c2){
            return false;
        }
        
        const auto& targets_c2 = c2.targetLayer();
        const auto& targets_c1 = c1.targetLayer();
        if (targets_c1.hasTag(layer_c2) &&
            targets_c2.hasTag(layer_c1)) {
            return true;
        }
        return false;
    }

    void ProcessCollisionDetect(
        ColliderCompBase* checker,
        ColliderCompBase* target,
        uint32_t frameIndex,
        uint32_t instIndex
        )
    {
        HitInfo info_c1(checker,target);
        if (checker->detect(*target, frameIndex, 
            info_c1, instIndex)) 
        {
            HitInfo info_c2 = info_c1;
            info_c2.other = checker;
            info_c2.normal *= -1.f;

            checker->owner().onHit(info_c1);
            target->owner().onHit(info_c2);
        }
    }
}

ColliderManager::ColliderManager()
    : m_ColliderList(MAX_LIST_SIZE)
{
     if (m_Instance) return;
    m_Instance = this;
}

void ColliderManager::term() {

}

void ColliderManager::update(uint32_t frameIndex)
{
    allCheck(frameIndex);
}

void ColliderManager::add(const std::string& key, ColliderCompBase* adder)
{
    m_ColliderList.add(key, adder);
}

void ColliderManager::remove(const std::string& key) {
    m_ColliderList.remove(key);
}

void ColliderManager::clear() {
    m_ColliderList.clear();
}

bool ColliderManager::raycast(
    const RayDesc& ray,
    HitInfo& info,
    uint32_t frameIndex,
    bool bIsSkippedOnHit)
{
    bool isHit = false;
    float minDist = ray.maxDistance;
    auto selfCollider = info.self;
    HitInfo closesttInfo; 


    for (auto* collider : m_ColliderList.list()) {
        if (!collider) continue;
        if (!ray.targetTag.hasTag(collider->layer())) { continue; }

        uint32_t instNum = collider->owner().instanceNum();
        for (uint32_t idx = 0; idx < instNum; ++idx) {
            HitInfo tempInfo;
            if (collider->detectRay(ray, tempInfo, idx, frameIndex)) {
                if (tempInfo.distance < minDist) {
                    minDist = tempInfo.distance;
                    closesttInfo = tempInfo;
                    closesttInfo.instanceIndex = idx;
                    isHit = true;
                }
            }
        }
    }

    if (isHit) {
        info = closesttInfo; // 最も手前のヒット情報
        info.self = selfCollider;

        auto otherInfo = info;
        otherInfo.self = info.other;
        otherInfo.other = info.self;
        if (bIsSkippedOnHit || !info.self || !info.other) {
            return isHit;
        }

        info.self->owner().onHit(info);
        info.other->owner().onHit(otherInfo);
    }

    return isHit;
}
//  全走査
//  インスタンシング描画用のバッファから座標を取得
//  インスタンシング描画同士は判定をとらない
void ColliderManager::allCheck(uint32_t frameIndex)
{
    const auto& list = m_ColliderList.list();
    for (size_t i = 0; i < list.size(); i++) {
        for (size_t j = i + 1; j < list.size(); j++) {
            const auto c1 = list[i];
            const auto c2 = list[j];
            assert((c1 && c2) && "ColliderManager::allCheck - colliderNull");
            //  レイヤーチェック
            if (!CheckLayer(*c1, *c2)) {
                continue;
            }

            //if (c1->layer() == CollisionTag::Block || c2->layer() == CollisionTag::Block) {
            //    printf("");
            //}



            auto instNum1 = c1->owner().instanceNum();
            auto instNum2 = c2->owner().instanceNum();
            if (instNum1 == 1 && instNum2 == 1) {
                ProcessCollisionDetect(c1, c2, frameIndex, 0);
            }
            else if (instNum2 > 1) {
                for (uint32_t idx = 0; idx < instNum2; ++idx) {
                    ProcessCollisionDetect(c1, c2, frameIndex, idx);
                }
            }
            else if (instNum1 > 1) {
                for (uint32_t idx = 0; idx < instNum1; ++idx) {
                    ProcessCollisionDetect(c2, c1, frameIndex, idx);
                }
            }
        }
    }

}
