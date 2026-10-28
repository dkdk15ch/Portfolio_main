#include "Manager/RenderManager.h"
#include <utility>
#include "Component/Render/RenderCompBase.h"
#include "GameObject/GameBase.h"
#include "GraphicContext.h"
#include <string>

RenderManager* RenderManager::m_Instance = nullptr;

namespace {
	static uint32_t MAX_LIST_SIZE = 50;
}

RenderManager::RenderManager()
{
	if (m_Instance) return; 

	auto psoKeyList = GraphicContext::m_Instance->getGPSOKeys();
	for (const std::string key : psoKeyList) {
		m_RenderList.emplace(key, MAX_LIST_SIZE);
	}
	m_Instance = this;
}

RenderManager::~RenderManager()
{
}

void RenderManager::term() {
}

void RenderManager::draw(
	ID3D12GraphicsCommandList* pCmdList,
	const uint32_t frameIndex,
	D3D12_GPU_VIRTUAL_ADDRESS lightBuffer) 
{
	auto graphicContext = GraphicContext::m_Instance;
	
	for (const auto& [pso, renderList] : m_RenderList) {
		const auto& list = renderList.list();
		if (list.empty()) { continue;}

		pCmdList->SetPipelineState(graphicContext->getGPSO(pso));
		for (const auto& r : list) {
			
			if (r->owner().name() == "CrossHair") {
				printf("Hi");
			}

			r->render(pCmdList, frameIndex, lightBuffer);
		}
	}
}

bool RenderManager::add(const std::string& psoName, RenderCompBase* adder)
{
	if (!adder) return false;

	auto it = m_RenderList.find(psoName);
	if (it != m_RenderList.end()) {
		std::string key = adder->owner().name();
		return it->second.add(key, adder);
	}

	return true;
}

bool RenderManager::remove(const std::string& psoName, const std::string& key)
{		
	auto it = m_RenderList.find(psoName);
	if (it != m_RenderList.end()) {
		return it->second.remove(key);
	}

	return true;
}

void RenderManager::clear(const std::string& psoName) {
	auto it = m_RenderList.find(psoName);
	if (it != m_RenderList.end()) {
		it->second.clear();
	}
}

void RenderManager::allClear()
{
	m_RenderList.clear();

	auto psoKeyList = GraphicContext::m_Instance->getGPSOKeys();
	for (const std::string key : psoKeyList) {
		m_RenderList.emplace(key, MAX_LIST_SIZE);
	}
}




