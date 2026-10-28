#pragma once
#include "PointerHelper.h"
#include <unordered_map>

class Camera;

class CameraManager
{
public:
	static CameraManager* m_Instance;

	CameraManager();
	~CameraManager() = default;

	void destroy();

	void addCamera(std::string key, LogPtr<Camera> camera);
	void removeCamera(std::string key);
	void removeAll();

	void setCurrentCamera(std::string key);
	Camera& currentCamera(){ return *m_CurrentCamera; }
private:
	Camera* m_CurrentCamera;

	std::unordered_map<std::string, LogPtr<Camera>> m_CameraList;
};

