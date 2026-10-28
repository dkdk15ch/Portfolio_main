#include "GameObject/Enemy/Enemy.h"
#include "GameObject/Spawner.h"
#include "GameObject/Param/CollisionLayerParam.h"
#include "Component/MovementComp.h"
#include "Component/Collider/SphereColliderComp.h"
#include "Component/Collider/ColliderCompBase.h"
#include "Manager/LevelManager.h"
#include "GameObject/Strategy/EnemyHitResponse.h"
#include "SafeDelete.h"


namespace EnemyObjects {
	class CollisionObject : public GameBase {
	public:
		CollisionObject(std::string name, Enemy* parent) 
			: GameBase(name)
			, m_CastedParent(parent)
		{
			setParent(parent);
			addComponent(MAKE_UNIQUE_LOG<SphereColliderComp>(this));
			auto collider = getComponent<SphereColliderComp>(CompType::ColliderComp);
			assert(collider);
			collider->setLayer(CollisionTag::Enemy);
			auto layerParam = CollisionLayerParam::layerParam();
			collider->targetLayer().setTag(layerParam->enemyTarget());
			collider->show();
		}

		void implOnHit(const HitInfo& info) override final {
			const auto& layer = info.other->layer();
			auto hitRes = m_CastedParent->m_ResponseFactory->create(layer);

			if (hitRes) {
				hitRes->onHit(*m_CastedParent, info);
				SafeDelete(hitRes);
			}
		}
		Enemy* m_CastedParent;

	};
}

Enemy::Enemy(std::string uniqueName)
	: GameBase(std::move(makeUniqueName(uniqueName)))
	, m_ResponseFactory(nullptr)
{
	//	mover
	addComponent(MAKE_UNIQUE_LOG<MovementComp>(this));
	m_Mover = static_cast<MovementComp*>(getComponent(CompType::MovementComp));
	//	collider
	auto colliderObj = MAKE_UNIQUE_LOG<EnemyObjects::CollisionObject>(m_Name + "_Collider", this);
	m_ColliderObj = colliderObj.get();
	m_Collider = static_cast<SphereColliderComp*>(m_ColliderObj->getComponent(CompType::ColliderComp));

	auto mgr = LevelManager::m_Instance;
	mgr->requestAddObject(std::move(colliderObj));
}

Enemy::~Enemy()
{
}

void Enemy::term()
{
}

SphereColliderComp* Enemy::collider()const { 
	return static_cast<SphereColliderComp*>(m_Collider); 
}

void Enemy::showLifeTime()
{
#ifdef _DEBUG
	printf("LifeTime %f\n", m_LifeTime);
	printf("%s\n", m_Name.c_str());
	
#endif // _DEBUG
}


void Enemy::implPreupdate(uint32_t frameIndex, double deltaTime)
{
}

void Enemy::implupdate(uint32_t frameIndex, double deltaTime)
{
	m_LifeTime += deltaTime;
}

void Enemy::implOnHit(const HitInfo& info)
{

}

