#include "Level/GameOverLevel.h"
#include "GameObject/Player.h"
#include "GameObject/Camera.h"
#include "Component/Animator.h"
#include "Animation/Model.h"
#include "Util/Loader.h"
#include "Manager/CameraManager.h"
#include "Component/MovementComp.h"


GameOverLevel::GameOverLevel()
{


}

GameOverLevel::~GameOverLevel()
{

}

void GameOverLevel::init()
{
 	auto player = MAKE_UNIQUE_LOG<Player>("ResultPlayer", true);
	player->removeComponent(CompType::ColliderComp);

	auto mover = static_cast<MovementComp*>(player->getComponent(CompType::MovementComp));
	mover->setRotY(DirectX::XM_PIDIV2);
	mover->setRotX(0.f);
	mover->setRotZ(0.f);
	player->removeComponent(CompType::MovementComp);

	auto animator = static_cast<Animator*>(player->getComponent(CompType::Animator));
	auto model = animator->targetModel();
	auto p = dynamic_cast<const PlayerParam*>(player->param());
	if (p) {
		Loader::animation(p->animaPathGameOver(), model, p->animaLoopGameOver(), p->animaScaleGameOver());
	}	//	cameraセット
	auto cameraMgr = CameraManager::m_Instance;
	auto cameraDesc = Camera::Desc();
	cameraMgr->addCamera("ResultCamera", MAKE_UNIQUE_LOG<Camera>(cameraDesc));
	cameraMgr->setCurrentCamera("ResultCamera");
	auto& cam = CameraManager::m_Instance->currentCamera();
	auto relativePos =
		DirectX::SimpleMath::Vector3(0.0f, 150.f, -400.f);
	cam.setFollowOffset(relativePos);
	cam.follow(*player);

	animator->setNextAnimation(model->name() + "|GameOver");
	addObject(std::move(player));
}

void GameOverLevel::update(const uint32_t frameIndex, double deltaTime)
{
	Level::update(frameIndex, deltaTime);
	//	カメラを更新

}

void GameOverLevel::term()
{
}
