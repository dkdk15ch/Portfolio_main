#include "Level/TestScene.h"
#include "GameObject/Player.h"
#include "GameObject/GameBase.h"
#include "GameObject/Ball.h"
#include "GameObject/BlockWall.h"
#include "GameObject/PlaneObject.h"
#include "GameObject/Camera.h"
#include "InOutput/Audio.h"
#include "Manager/CameraManager.h"
#include "Manager/ColliderManager.h"
#include "Manager/RenderManager.h"

#include "GameObject/SphereObject.h"
#include "GameObject/CubeObject.h"
#include "Manager/LevelManager.h"
#include "Component/Render/RenderCompBase.h"
#include "GameObject/Spawner.h"
#include "GameObject/Enemy/Bat.h"
#include "GameObject/Item/ClearItem.h"
#include "Level/ClearCondition/ItemClearCondition.h"
#include "GameObject/TestAnimation.h"
#include <DirectXColors.h>
#include "Component/Collider/SphereColliderComp.h"

namespace {
	const std::string sceneKey = "GameScene";

	class TestSphere : public SphereObject {
	public:

		TestSphere(std::string name)
			: SphereObject(name)
		{
			initStaticTransform(
				DirectX::SimpleMath::Vector3(50.f, 50.f, 200.f),
				DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f),
				DirectX::SimpleMath::Vector3(10.f, 10.f, 10.f)
			);


			m_Render = static_cast<RenderCompBase*>(getComponent(CompType::RenderComp));
			assert(m_Render);
			m_Render->setMovable(true);

			addComponent(MAKE_UNIQUE_LOG<SphereColliderComp>(this));
			auto collider = dynamic_cast<SphereColliderComp*>(getComponent(CompType::ColliderComp));
			assert(collider);
			collider->setLayer(CollisionTag::Ball);
			collider->targetLayer().setTag(static_cast<uint32_t>(400));
			//collider->show();


		}
	protected:
		void implPreupdate(uint32_t frameIndex, double deltaTime) override {
			setColor(DirectX::Colors::Green);
		}

		void implOnHit(const HitInfo& info) override {
			setColor(DirectX::Colors::Red);
		}
		RenderCompBase* m_Render;
	};

}

TestScene::TestScene(std::string sceneKey)
	: Level(std::move(sceneKey))
{


}

TestScene::~TestScene()
{
	m_ColliderMgr->clear();
	RenderManager::m_Instance->allClear();
}

void TestScene::init()
{
	std::vector<LogPtr<GameBase>> objects;
	objects.push_back(MAKE_UNIQUE_LOG<Player>("Player"));
	//objects.push_back(MAKE_UNIQUE_LOG<Ball>("Ball"));
	//objects.push_back(MAKE_UNIQUE_LOG<BlockWall>("BlockWall"));
	//objects.push_back(MAKE_UNIQUE_LOG<PlaneObject>
	//	("Floor", 1, "Floor"));
	//objects.push_back(MAKE_UNIQUE_LOG<PlaneObject>
	//	("FrontWall", 1, "FrontWall"));
	//objects.push_back(MAKE_UNIQUE_LOG<PlaneObject>
	//	("LeftWall", 1, "LeftWall"));
	//objects.push_back(MAKE_UNIQUE_LOG<PlaneObject>
	//	("RightWall", 1, "RightWall"));
	Spawner::create<Bat>("BatSpawner", 1);
	Spawner::create<ClearItem>("ClearItemSpawner", 1);

	auto test = MAKE_UNIQUE_LOG<TestSphere>("TestSphere");
	objects.push_back(std::move(test));

	addObjects(std::move(objects));

	m_ColliderMgr = ColliderManager::m_Instance;
	m_ClearCondition = MAKE_UNIQUE_LOG<ItemClearCondition>();
	m_Player = getObject<Player>();
}


void TestScene::update(const uint32_t frameIndex, double deltaTime)
{
	Level::update(frameIndex, deltaTime);
	//	カメラを更新
	auto& cam = CameraManager::m_Instance->currentCamera() ;
 	cam.follow(*m_Player);
	cam.update(frameIndex, deltaTime);
	//	当たり判定
	m_ColliderMgr->update(frameIndex);
}

void TestScene::term()
{
	Level::term();
}


