#include "GraphicContext.h"
#include "Texture.h"
#include "FileUtil.h"
#include "Logger.h"
#include <StringHelper.h>
#include <format>
#include <CommonStates.h>
#include "resMesh.h"
#include <iostream>

//  static
GraphicContext* GraphicContext::m_Instance = nullptr;


GraphicContext::GraphicContext() : 
    m_FrameIndex(0),
    m_PPool{},
    m_Scissor(0,0,0,0),
    m_Viewport(0)
{
    if (m_Instance) return;

    m_Instance = this;
}


GraphicContext::~GraphicContext()
{
    m_Instance = nullptr;
}


void GraphicContext::addGPSO(const std::string& key, ComPtr<ID3D12PipelineState> pso)
{
    m_PGraphicPSOMap[key] = pso;
}

void GraphicContext::addCPSO(const std::string& key, ComPtr<ID3D12PipelineState> pso)
{
    m_PComputePSOMap[key] = pso;
}


void GraphicContext::copyRenderTarget(ID3D12Resource* srcRes, ID3D12Resource* dstRes)
{
    auto cmd = m_CommandList.get();
    D3D12_RESOURCE_BARRIER barriers[2] = {};

    // srcRes(UAV)をコピー元へ
    barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barriers[0].Transition.pResource = srcRes;
    barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
    barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    // dstRes(バックバッファ)をコピー先へ
    barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barriers[1].Transition.pResource = dstRes;
    barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    cmd->ResourceBarrier(2, barriers);
    cmd->CopyResource(dstRes, srcRes);

    // 元にも戻す
    barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 表示用に戻す

    cmd->ResourceBarrier(2, barriers);
}

ColorTarget* GraphicContext::getBackBuffer()
{
    uint32_t idx = m_PSwapChain->GetCurrentBackBufferIndex();
    return &m_ColorTarget[idx];
}


bool GraphicContext::init() {
#if defined(DEBUG) || defined(_DEBUG)
    {
        ComPtr<ID3D12Debug> pDebug;
        auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(pDebug.GetAddressOf()));
        if (SUCCEEDED(hr))
        {
            pDebug->EnableDebugLayer();
        }
    }
#endif

// デバイスの生成.
auto hr = D3D12CreateDevice(
    nullptr,
    D3D_FEATURE_LEVEL_11_0,
    IID_PPV_ARGS(m_PDevice.GetAddressOf()));
if (FAILED(hr))
{
    ELOG("Error : GraphicContext::init() failed create device");
    return false;
}

m_PDevice->SetName(L"App::m_pDevice");

// コマンドキューの生成.
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    hr = m_PDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_PQueue.GetAddressOf()));
    if (FAILED(hr))
    {
        ELOG("Error : GraphicContext::init() failed create queue");
        return false;
    }

    m_PQueue->SetName(L"App::m_pQueue");
}

auto app = App::m_Instance;
auto width = app->width();
auto height = app->height();
auto wnd = app->hWnd();

// スワップチェインの生成.
{
    // DXGIファクトリーの生成.
    ComPtr<IDXGIFactory4> pFactory;
    hr = CreateDXGIFactory1(IID_PPV_ARGS(pFactory.GetAddressOf()));
    if (FAILED(hr))
    {
        ELOG("Error : GraphicContext::init() failed create DXGIFactory");
        return false;
    }

    // スワップチェインの設定.
    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferDesc.Width = width;
    desc.BufferDesc.Height = height;
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = App::m_FrameCount;
    desc.OutputWindow = wnd;
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // スワップチェインの生成.
    ComPtr<IDXGISwapChain> pSwapChain;
    hr = pFactory->CreateSwapChain(m_PQueue.Get(), &desc, pSwapChain.GetAddressOf());
    if (FAILED(hr))
    {
        ELOG("Error : GraphicContext::init() failed create swapchain");
        return false;
    }
    //  alt + enter 無効
    pFactory->MakeWindowAssociation(wnd, DXGI_MWA_NO_ALT_ENTER);

    // IDXGISwapChain3 を取得.
    hr = pSwapChain.As(&m_PSwapChain);
    if (FAILED(hr))
    {
        ELOG("Error : GraphicContext::init() failed create IDXGISwapChain3");
        return false;
    }

    // バックバッファ番号を取得.
    m_FrameIndex = m_PSwapChain->GetCurrentBackBufferIndex();

    // 不要になったので解放.
    pFactory.Reset();
    pSwapChain.Reset();
}

// ディスクリプタプールの生成.
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};

    desc.NodeMask = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = 512;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (!DescriptorPool::create(m_PDevice.Get(), &desc, &m_PPool[POOL_TYPE_RES]))
    {
        ELOG("Error : GraphicContext::init() failed create POOL_TYPE_RES");
        return false;
    }

    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    desc.NumDescriptors = 256;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (!DescriptorPool::create(m_PDevice.Get(), &desc, &m_PPool[POOL_TYPE_SMP]))
    {
        ELOG("Error : GraphicContext::init() failed create POOL_TYPE_SMP");
        return false;
    }

    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.NumDescriptors = 512;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    if (!DescriptorPool::create(m_PDevice.Get(), &desc, &m_PPool[POOL_TYPE_RTV]))
    {
        ELOG("Error : GraphicContext::init() failed create POOL_TYPE_RTV");
        return false;
    }

    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    desc.NumDescriptors = 512;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    if (!DescriptorPool::create(m_PDevice.Get(), &desc, &m_PPool[POOL_TYPE_DSV]))
    {
        ELOG("Error : GraphicContext::init() failed create POOL_TYPE_DSV");
        return false;
    }
}

// コマンドリストの生成.
{
    if (!m_CommandList.init(
        m_PDevice.Get(),
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        App::m_FrameCount))
    {
        ELOG("Error : GraphicContext::init() failed create commandlist");
        return false;
    }

}

{// レンダーターゲットビューの生成.

    for (auto i = 0u; i < App::m_FrameCount; ++i)
    {
        if (!m_ColorTarget[i].initFromBackBuffer(
            m_PDevice.Get(),
            m_PPool[POOL_TYPE_RTV],
            m_PPool[POOL_TYPE_RES],
            i,
            m_PSwapChain.Get()))
        {
            ELOG("Error : GraphicContext::init() failed create RTV");
            return false;
        }
    }
}

{// UAVの生成
    if (!m_UnorderAccessTarget.init(
        m_PDevice.Get(),
        m_PPool[POOL_TYPE_RES],
        width, height,
        m_PSwapChain.Get())) {
        ELOG("Error : GraphicContext::init() failed create UAV");
        return false;

    }

}


// 深度ステンシルバッファの生成
{
    if (!m_DepthTarget.init(
        m_PDevice.Get(),
        m_PPool[POOL_TYPE_DSV],
        width,
        height,
        DXGI_FORMAT_D32_FLOAT))
    {
        ELOG("Error : GraphicContext::init() failed create depthTarget");
        return false;
    }
}

// フェンスの生成.
if (!m_Fence.init(m_PDevice.Get()))
{
    ELOG("Error : GraphicContext::init() failed create fence");
    return false;
}

// ビューポートの設定.
{
    m_Viewport.TopLeftX = 0.0f;
    m_Viewport.TopLeftY = 0.0f;
    m_Viewport.Width = float(width);
    m_Viewport.Height = float(height);
    m_Viewport.MinDepth = 0.0f;
    m_Viewport.MaxDepth = 1.0f;
}

// シザー矩形の設定.
{
    m_Scissor.left = 0;
    m_Scissor.right = width;
    m_Scissor.top = 0;
    m_Scissor.bottom = height;
}


//  DirectXTK用
//  GraphisMemoryの作成
m_GraphicsMemory = std::make_unique<DirectX::GraphicsMemory>(m_PDevice.Get());

//  DirectXTKでスプライト描画の準備
DirectX::ResourceUploadBatch uploadBatch(m_PDevice.Get());
uploadBatch.Begin();

DirectX::RenderTargetState rtState(
    m_ColorTarget[0].getViewDesc().Format,
    m_DepthTarget.getViewDesc().Format);

DirectX::DX12::SpriteBatchPipelineStateDescription psoDesc(rtState);
    psoDesc.depthStencilDesc = DirectX::CommonStates::DepthDefault;
    psoDesc.blendDesc = DirectX::CommonStates::NonPremultiplied;

m_SpriteBatch = std::make_unique<DirectX::DX12::SpriteBatch>(m_PDevice.Get(), uploadBatch, psoDesc);

auto uploadFinished = uploadBatch.End(m_PQueue.Get());
uploadFinished.wait();

// 正常終了.
return true;

}


void GraphicContext::term() {
    // GPU処理の完了を待機.
    m_Fence.sync(m_PQueue.Get());

    // TermD3D()
    if (m_GraphicsMemory) 
    {
        m_GraphicsMemory->Commit(m_PQueue.Get());
        m_GraphicsMemory.reset();
    }
    
    m_SpriteBatch.reset();
    m_PGraphicPSOMap.clear();

    // フェンス破棄.
    m_Fence.term();

    m_UnorderAccessTarget.term();

    // レンダーターゲットビューの破棄.
    for (auto i = 0u; i < App::m_FrameCount; ++i)
    {
        m_ColorTarget[i].term();
    }

    // 深度ステンシルビューの破棄.
    m_DepthTarget.term();

    // コマンドリストの破棄.
    m_CommandList.term();

    for (auto i = 0; i < POOL_COUNT; ++i)
    {
        if (m_PPool[i] != nullptr)
        {
            m_PPool[i]->release();
            m_PPool[i] = nullptr;
        }
    }

    // スワップチェインの破棄.
    m_PSwapChain.Reset();

    // コマンドキューの破棄.
    m_PQueue.Reset();

    // デバイスの破棄.
    m_PDevice.Reset();

}


void GraphicContext::present(uint32_t interval) {
    // 画面に表示.
    m_PSwapChain->Present(interval, 0);

    m_GraphicsMemory->Commit(m_PQueue.Get());   //  DirectXTK用

    // 完了待ち.
    m_Fence.wait(m_PQueue.Get(), INFINITE);

    // フレーム番号を更新.
    m_FrameIndex = m_PSwapChain->GetCurrentBackBufferIndex();
}


#pragma region Getter/Setter
ID3D12Device* GraphicContext::device() const
{
    assert(m_Instance && "GraphicContext::device()");
    return m_Instance->m_PDevice.Get();
}

ID3D12CommandQueue* GraphicContext::queue() const
{
    assert(m_Instance && "GraphicContext::queue()");
    return m_Instance->m_PQueue.Get();
}

const ColorTarget* GraphicContext::colorTarget(uint16_t idx)
{
    assert(m_Instance && "GraphicContext::colorTarget()");
    return &m_Instance->m_ColorTarget[idx];
}

const UnorderAccessTarget* GraphicContext::unorderAccessTarget()
{
    assert(m_Instance && "GraphicContext::colorTarget()");
    return &m_Instance->m_UnorderAccessTarget;
}

DescriptorPool* GraphicContext::poolRes() const
{
    assert(m_Instance && "GraphicContext::poolRes()");
    return m_Instance->m_PPool[0];
}

const DepthTarget* GraphicContext::depthTarget() const
{
    assert(m_Instance && "GraphicContext::depthTarget()");
    return &m_Instance->m_DepthTarget;
}

CommandList& GraphicContext::commandList()
{
    assert(m_Instance && "GraphicContext::commandList()");
    return m_Instance->m_CommandList;
}

uint32_t GraphicContext::frameIndex() const
{
    assert(m_Instance && "GraphicContext::frameIndex()");
    return m_Instance->m_FrameIndex;
}

DirectX::SpriteBatch* GraphicContext::spriteBatch() const
{
    assert(m_Instance && "GraphicContext::spriteBatch()");
    return m_Instance->m_SpriteBatch.get();
}

ID3D12PipelineState* GraphicContext::getGPSO(const std::string& key)
{
    assert(m_Instance && "GraphicContext::getPSO()");

    auto it = m_PGraphicPSOMap.find(key);
    if (it == m_PGraphicPSOMap.end()) {
        ELOG("Error : GraphicContext::getPSO not found %s\n", key.c_str());
        assert(false);
    }
    return it->second.Get();
}

ID3D12PipelineState* GraphicContext::getCPSO(const std::string& key)
{
    assert(m_Instance && "GraphicContext::getPSO()");

    auto it = m_PComputePSOMap.find(key);
    if (it == m_PComputePSOMap.end()) {
        ELOG("Error : GraphicContext::getPSO not found %s\n", key.c_str());
        assert(false);
    }
    return it->second.Get();
}

std::vector<std::string> GraphicContext::getGPSOKeys() {
    auto size = m_PGraphicPSOMap.size();
    std::vector<std::string> temp;
    temp.reserve(size);
    for (const auto& [key, pso] : m_PGraphicPSOMap) {
        temp.push_back(key);
    }
    return temp;
}

D3D12_VIEWPORT* GraphicContext::viewport() const
{
    assert(m_Instance && "GraphicContext::viewport()");
    return &m_Instance->m_Viewport;
}

D3D12_RECT* GraphicContext::scissor() const
{
    assert(m_Instance && "GraphicContext::scissor()");
    return &m_Instance->m_Scissor;

}

DirectX::DX12::SpriteBatch* GraphicContext::spriteBatch() {
    assert(m_Instance && "GraphicContext::scissor()");
    return m_SpriteBatch.get();
}

Texture* GraphicContext::dummyTexure()const
{
    assert(m_Instance && "GraphicContext::dummyTexture()");
    return m_PDummyTexture;
}

#pragma endregion


