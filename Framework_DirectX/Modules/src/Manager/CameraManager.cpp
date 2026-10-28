#include "Manager/CameraManager.h"
#include "GameObject/Camera.h"
#include "SafeDelete.h"
#include "Logger.h"

CameraManager* CameraManager::m_Instance = nullptr;

CameraManager::CameraManager()
	: m_CurrentCamera(nullptr)
	, m_CameraList{}
{
	if (m_Instance) { return; }
	m_Instance = this;
}

void CameraManager::destroy() {
	m_CurrentCamera = nullptr;
	m_CameraList.clear();
}

void CameraManager::addCamera(std::string key, LogPtr<Camera> camera)
{
	if (m_CameraList.contains(key)) {
		ELOG("Error : CameraManager::addCamera - key is Already registered");
		return;
	}

	m_CameraList[key] = std::move(camera);
}

void CameraManager::removeCamera(std::string key)
{
	auto it = m_CameraList.find(key);
	if (it == m_CameraList.end()) {
		return;
	}
	m_CameraList.erase(it);
}

void CameraManager::removeAll()
{
	m_CameraList.clear(); 
}

void CameraManager::setCurrentCamera(std::string key)
{
	if (!m_CameraList.contains(key)) {
		ELOG("Error : CameraManager::setCamera - key is not registered");
		return;
	}

	m_CurrentCamera = m_CameraList[key].get();
}
