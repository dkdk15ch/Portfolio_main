#include "ColorTarget.h"
#include "DescriptorPool.h"
#include "UnorderAccessTarget.h"

ColorTarget::ColorTarget()
	: m_PTarget(nullptr)
	, m_PHandleRTV(nullptr)
	, m_PPoolRTV(nullptr)
    , m_ViewDesc{}
{ /* DO_NOTHING */
}


ColorTarget::~ColorTarget()
{
	term();
}


bool ColorTarget::init(
	ID3D12Device* pDevice, 
	DescriptorPool* pPoolRTV, 
	uint32_t width, 
	uint32_t height, 
	DXGI_FORMAT format)
{
    if (pDevice == nullptr || pPoolRTV == nullptr || width == 0 || height == 0)
    {
        return false;
    }

    assert(m_PHandleRTV == nullptr && "ColorTarget::init() m_pHandleRTV != nullptr");
    assert(m_PPoolRTV == nullptr && "ColorTarget::init() m_pPoolRTV != nullptr");

    m_PPoolRTV = pPoolRTV;
    m_PPoolRTV->addRef();

    m_PHandleRTV = m_PPoolRTV->allocHandle();
    if (m_PHandleRTV == nullptr)
    {
        return false;
    }

    D3D12_HEAP_PROPERTIES prop = {};
    prop.Type = D3D12_HEAP_TYPE_DEFAULT;
    prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    prop.CreationNodeMask = 1;
    prop.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Alignment = 0;
    desc.Width = UINT64(width);
    desc.Height = height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = format;
    clearValue.Color[0] = 1.0f;
    clearValue.Color[1] = 1.0f;
    clearValue.Color[2] = 1.0f;
    clearValue.Color[3] = 1.0f;

    //  リソース生成
    auto hr = pDevice->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue,
        IID_PPV_ARGS(m_PTarget.GetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }

    m_PTarget->SetName(L"ColorTarget");

    m_ViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    m_ViewDesc.Format = format;
    m_ViewDesc.Texture2D.MipSlice = 0;
    m_ViewDesc.Texture2D.PlaneSlice = 0;

    //  ビュー作成
    pDevice->CreateRenderTargetView(
        m_PTarget.Get(),
        &m_ViewDesc,
        m_PHandleRTV->HandleCPU);

    return true;
}


void ColorTarget::term()
{
	m_PTarget.Reset();

	if (m_PPoolRTV != nullptr && m_PHandleRTV != nullptr)
	{
		m_PPoolRTV->freeHandle(m_PHandleRTV);
		m_PHandleRTV = nullptr;
	}

	if (m_PPoolRTV != nullptr)
	{
		m_PPoolRTV->release();
		m_PPoolRTV = nullptr;
	}

    if (m_PHandleSRV != nullptr) {
        m_PHandleRTV = nullptr;
    }
}


bool ColorTarget::initFromBackBuffer(
    ID3D12Device* pDevice, 
    DescriptorPool* pPoolRTV, 
    DescriptorPool* pPoolRes, 
    uint32_t index, 
    IDXGISwapChain* pSwapChain)
{
    if (pDevice == nullptr || pPoolRTV == nullptr || pSwapChain == nullptr)
    {
        return false;
    }

    assert(m_PHandleRTV == nullptr && "ColorTarget::initFromBackBuffer() m_pHandleRTV != nullptr");
    assert(m_PPoolRTV == nullptr && "ColorTarget::initFromBackBuffer() m_pPoolRTV != nullptr");

    m_PPoolRTV = pPoolRTV;
    m_PPoolRTV->addRef();

    m_PHandleRTV = m_PPoolRTV->allocHandle();
    if (m_PHandleRTV == nullptr)
    {
        return false;
    }

    auto hr = pSwapChain->GetBuffer(index, IID_PPV_ARGS(m_PTarget.GetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }

    DXGI_SWAP_CHAIN_DESC desc;
    pSwapChain->GetDesc(&desc);

    m_ViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    m_ViewDesc.Format = desc.BufferDesc.Format;
    m_ViewDesc.Texture2D.MipSlice = 0;
    m_ViewDesc.Texture2D.PlaneSlice = 0;

    pDevice->CreateRenderTargetView(
        m_PTarget.Get(),
        &m_ViewDesc,
        m_PHandleRTV->HandleCPU);

    //  SRVの作成
    m_PHandleSRV = pPoolRes->allocHandle();
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = desc.BufferDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = 1;

    pDevice->CreateShaderResourceView(
        m_PTarget.Get(), 
        &srvDesc, 
        m_PHandleSRV->HandleCPU);    
    
    return true;
}


#pragma region Getter

DescriptorHandle* ColorTarget::getHandleRTV() const
{
    return m_PHandleRTV;
}

DescriptorHandle* ColorTarget::getHandleSRV() const
{
    return m_PHandleSRV;
}

ID3D12Resource* ColorTarget::getResource() const
{
    return m_PTarget.Get();
}

D3D12_RESOURCE_DESC ColorTarget::getDesc() const
{
    if (m_PTarget == nullptr)
    {
        return D3D12_RESOURCE_DESC();
    }

    return m_PTarget->GetDesc();
}

D3D12_RENDER_TARGET_VIEW_DESC ColorTarget::getViewDesc() const
{
    return m_ViewDesc;
}

#pragma endregion