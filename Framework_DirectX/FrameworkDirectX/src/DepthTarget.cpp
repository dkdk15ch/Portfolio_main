#include "DepthTarget.h"
#include "DescriptorPool.h"
#include "Logger.h"

DepthTarget::DepthTarget()
    : m_PTarget(nullptr)
    , m_PHandleDSV(nullptr)
    , m_PPoolDSV(nullptr)
    , m_ViewDesc{}
{ 
}

DepthTarget::~DepthTarget(){
    term();
}


bool DepthTarget::init(
    ID3D12Device* pDevice,
    DescriptorPool* pPoolRTV,
    uint32_t        width,
    uint32_t        height,
    DXGI_FORMAT     format)
{
    if (pDevice == nullptr || pPoolRTV == nullptr || width == 0 || height == 0)
    {
        return false;
    }

    assert(m_PHandleDSV == nullptr && "DepthTarget::init() m_pHandleDSV != nullptr");
    assert(m_PPoolDSV == nullptr && "DepthTarget::init() m_pPoolDSV != nullptr");

    m_PPoolDSV = pPoolRTV;
    m_PPoolDSV->addRef();

    m_PHandleDSV = m_PPoolDSV->allocHandle();
    if (m_PHandleDSV == nullptr)
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
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = format;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    //  リソース生成
    auto hr = pDevice->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(m_PTarget.GetAddressOf()));
    if (FAILED(hr))  {
        ELOG("Error DepthTarget::init - failde create resource");
        return false;
    }
    m_PTarget->SetName(L"DepthTarget");

    m_ViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    m_ViewDesc.Format = format;
    m_ViewDesc.Texture2D.MipSlice = 0;
    m_ViewDesc.Flags = D3D12_DSV_FLAG_NONE;
    //  ビュー生成
    pDevice->CreateDepthStencilView(
        m_PTarget.Get(),
        &m_ViewDesc,
        m_PHandleDSV->HandleCPU);

    return true;
}


void DepthTarget::term()
{
    m_PTarget.Reset();

    if (m_PPoolDSV != nullptr && m_PHandleDSV != nullptr)
    {
        m_PPoolDSV->freeHandle(m_PHandleDSV);
        m_PHandleDSV = nullptr;
    }

    if (m_PPoolDSV != nullptr)
    {
        m_PPoolDSV->release();
        m_PPoolDSV = nullptr;
    }
}


#pragma region getter

DescriptorHandle* DepthTarget::getHandleDSV() const
{
    return m_PHandleDSV;
}

ID3D12Resource* DepthTarget::getResource() const
{
    return m_PTarget.Get();
}

D3D12_RESOURCE_DESC DepthTarget::getDesc() const
{
    if (m_PTarget == nullptr)
    {
        return D3D12_RESOURCE_DESC();
    }

    return m_PTarget->GetDesc();
}

D3D12_DEPTH_STENCIL_VIEW_DESC DepthTarget::getViewDesc() const
{
    return m_ViewDesc;
}

#pragma endregion
