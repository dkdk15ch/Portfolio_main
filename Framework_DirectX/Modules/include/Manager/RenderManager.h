#pragma once
#include <d3d12.h>
#include <unordered_map>
#include <string>
#include <vector>
#include "Util/MappedVector.h"

class RenderCompBase;

class RenderManager {
public:
	static RenderManager* m_Instance;

	RenderManager();
	~RenderManager();
	void term();
	void draw(ID3D12GraphicsCommandList* pCmdList,
		const uint32_t frameIndex,
		D3D12_GPU_VIRTUAL_ADDRESS lightBuffer);

	bool add(const std::string& psoName, RenderCompBase* adder);
	bool remove(const std::string& psoName, const std::string& key); 
	void clear(const std::string& psoName);
	void allClear();

private:
	std::unordered_map<std::string, MappedVector<RenderCompBase>> m_RenderList;
};