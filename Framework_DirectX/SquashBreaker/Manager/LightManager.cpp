#include "Manager/LightManager.h"
#include "ConstantBuffer.h"
#include "Logger.h"
#include "GraphicContext.h"

LightManager* LightManager::m_Instance = nullptr;


LightManager::LightManager()
{
	if (m_Instance) return;
	m_Instance = this;

    //  ライトバッファの設定
    auto graphicContext = GraphicContext::m_Instance;
    {
        auto pCB = std::make_unique<ConstantBuffer>();
        if (pCB == nullptr)
        {
            ELOG("Error : Out of memory.");
            return;
        }
        auto name = std::wstring(L"Light");
        if (!pCB->init(graphicContext->device(), graphicContext->poolRes(), sizeof(CB::LightBuffer),name))
        {
            ELOG("Error : ConstantBuffer::init() Failed.");
            return;
        }
	    auto ptr = pCB->getPtr<CB::LightBuffer>();
	    ptr->LightPosition = DirectX::SimpleMath::Vector4(0.0f, 100.0f, -100.0f, 0.0);

	    ptr->LightColor = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 0.0f);

	    m_PLight = std::move(pCB);
    }
}


LightManager::~LightManager()
{  
}


void LightManager::term() {
    m_PLight->term();
    m_PLight.reset();

    m_Instance = nullptr;
}


D3D12_GPU_VIRTUAL_ADDRESS LightManager::getLightAddress()
{
	return m_PLight->getAddress();
}
