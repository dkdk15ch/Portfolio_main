#include "Level/ClearLevel.h"
#include "GameObject/Player.h"
#include "GameObject/Camera.h"
#include "Component/Animator.h"
#include "Animation/Model.h"
#include "Util/Loader.h"
#include "Manager/CameraManager.h"
#include "Component/MovementComp.h"


ClearLevel::ClearLevel()
	: Level("")
{


}

ClearLevel::~ClearLevel()
{

}

void ClearLevel::init()
{
	auto player = MAKE_UNIQUE_LOG<Player>("ResultPlayer", true);

	auto mover = static_cast<MovementComp*>(player->getComponent(CompType::MovementComp));
	mover->setRotY(0.f);
	mover->setRotX(0.f);
	mover->setRotZ(0.f);
	player->removeComponent(CompType::MovementComp);

	auto animator = static_cast<Animator*>(player->getComponent(CompType::Animator));
	auto model = animator->targetModel();
	auto p = dynamic_cast<const PlayerParam*>(player->param());
	if (p) {
		Loader::animation(p->animaPathDance(), model, p->animaLoopDance(), p->animaScaleDance());
	}

	//	cameraセット
	auto cameraMgr = CameraManager::m_Instance;
	auto cameraDesc = Camera::Desc();
	cameraMgr->addCamera("ResultCamera", MAKE_UNIQUE_LOG<Camera>(cameraDesc));
	cameraMgr->setCurrentCamera("ResultCamera");
	auto& cam = CameraManager::m_Instance->currentCamera();
	auto relativePos =
		DirectX::SimpleMath::Vector3(0.0f, 200.f, -500.f);
	cam.setFollowOffset(relativePos);
	cam.follow(*player);

	animator->setNextAnimation(model->name() + "|DancingRunning");
	addObject(std::move(player));
}

void ClearLevel::update(const uint32_t frameIndex, double deltaTime)
{
	Level::update(frameIndex, deltaTime);
	//	カメラを更新

}

void ClearLevel::term()
{
}
