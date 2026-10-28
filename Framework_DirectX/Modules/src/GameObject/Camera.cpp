#include "GameObject/Camera.h"
#include "InOutput/Pad.h"
#include "App.h"
#include "UI/HUD.h"
#include <algorithm>
#include "GameObject/GameBase.h"


Camera::Camera(Desc desc) :
	m_Position	(desc.position),
	m_Target	(desc.target),
	m_Up		(desc.up),
	m_Fov		(desc.fov),
	m_Aspect	(desc.aspect),
	m_NearZ		(desc.nearZ),
	m_FarZ		(desc.farZ),
	m_Yaw(0.0f),
	m_Pitch(-10.f)
{
	m_Position = DirectX::SimpleMath::Vector3(0.0f, 300.0f, 670.0f);

	auto app = App::m_Instance;
	m_Fov = DirectX::XMConvertToRadians(desc.fov);
	m_Aspect = static_cast<float>(app->width()) / static_cast<float>(app->height());
	calcTarget();
}


Camera::~Camera() 
{
}


void Camera::update(uint32_t frameIndex, double deltaTime) 
{
	auto pad = Pad::m_Instance;

#ifdef _DEBUG
	const auto speed = static_cast<float>(5.0 * deltaTime);

	if (pad->m_KeyboardState.LeftShift && pad->m_KeyboardState.Up) {
		m_Position.z += speed;
	}
	else if (pad->m_KeyboardState.LeftShift && pad->m_KeyboardState.Down) {
		m_Position.z -= speed;
	}
	else if (pad->m_KeyboardState.LeftShift && pad->m_KeyboardState.Left) {
		m_Pitch -= 0.5f * speed;
	}
	else if (pad->m_KeyboardState.LeftShift && pad->m_KeyboardState.Right) {
		m_Pitch += 0.5f * speed;
	}
	else if (pad->m_KeyboardState.Left) {
		m_Position.x -= speed;
	}
	else if (pad->m_KeyboardState.Right) {
		m_Position.x += speed;
	}
	else if (pad->m_KeyboardState.Up) {
		m_Position.y += speed;
	}
	else if (pad->m_KeyboardState.Down) {
		m_Position.y -= speed;
	}
	else {
		return;
	}

	//	Target更新
	calcTarget();

	//	HUD更新
	auto messagePos = std::format(L"CameraPos : x = {}, y = {}, z = {}", m_Position.x, m_Position.y, m_Position.z);
	HUD::sendDebugMessage(L"CameraPos", messagePos);
	auto messageRot = std::format(L"CameraRot(deg) : yaw = {} , pitch = {}",m_Yaw,m_Pitch);
	HUD::sendDebugMessage(L"CameraRot", messageRot);

#endif // _DEBUG
}


void Camera::follow(const GameBase& target)
{
	m_Position = target.transform()->position() + m_FollowOffset;
	calcTarget();
}


DirectX::SimpleMath::Matrix Camera::getView() const {
	//return DirectX::SimpleMath::Matrix::CreateLookAt(m_Position, m_Target, m_Up);
	return DirectX::XMMatrixLookAtLH(m_Position, m_Target, m_Up);
}


DirectX::SimpleMath::Matrix Camera::getProj() const{
	//return DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(m_Fov, m_Aspect, m_NearZ, m_FarZ);
	
	return DirectX::XMMatrixPerspectiveFovLH(m_Fov, m_Aspect, m_NearZ, m_FarZ);
}


void Camera::calcTarget(){
	auto forward = DirectX::SimpleMath::Vector3();
	m_Pitch = std::clamp(m_Pitch, -89.0f, 20.0f);
	auto pitch = DirectX::XMConvertToRadians(m_Pitch);
	auto yaw = DirectX::XMConvertToRadians(m_Yaw);

	forward.x = cos(pitch) * sin(yaw);
	forward.y = sin(pitch);
	forward.z = cos(pitch) * cos(yaw);
	m_Target = m_Position + forward;
}

