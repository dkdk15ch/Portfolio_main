#pragma once
#include <memory>
#include <Windows.h>
#include <d3d12.h>

class ConstantBuffer;

class LightManager
{
public:
	static LightManager* m_Instance;

	LightManager();
	~LightManager();
	void term();

	D3D12_GPU_VIRTUAL_ADDRESS getLightAddress();

private:
	std::unique_ptr<ConstantBuffer> m_PLight;

};
