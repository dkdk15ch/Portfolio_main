#pragma once
#include <d3d12.h>
#include <ComPtr.h>
#include <ColorTarget.h>
#include <UnorderAccessTarget.h>
#include <DepthTarget.h>
#include <CommandList.h>
#include <d3dcompiler.h>
#include <DescriptorPool.h>
#include <Fence.h>
#include <SpriteBatch.h>
#include <DescriptorHeap.h>
#include <GraphicsMemory.h>
#include <unordered_map>
#include "App.h"
#include "Interface/IAsset.h"

class DeescriptorPool;
class Texture;

//  DirectX12のグラフィック関連を管理するクラス
class GraphicContext : public IAsset
{
public:
    friend class App;

	static GraphicContext* m_Instance;

    GraphicContext();
    ~GraphicContext();

    enum POOL_TYPE
    {
        POOL_TYPE_RES = 0,     // CBV / SRV / UAV
        POOL_TYPE_SMP = 1,     // Sampler
        POOL_TYPE_RTV = 2,     // RTV
        POOL_TYPE_DSV = 3,     // DSV
        POOL_COUNT = 4,
    };

#pragma region Getter
    ID3D12Device* device() const;
    ID3D12CommandQueue* queue() const;
    const ColorTarget* colorTarget(uint16_t idx = 0);
    const UnorderAccessTarget* unorderAccessTarget();
    DescriptorPool* poolRes() const;
    const DepthTarget* depthTarget() const;
    CommandList& commandList();
    uint32_t frameIndex()const;
    DirectX::SpriteBatch* spriteBatch() const;
    ID3D12PipelineState* getGPSO(const std::string& key);
    ID3D12PipelineState* getCPSO(const std::string& key);
    std::vector<std::string> getGPSOKeys();
    D3D12_VIEWPORT* viewport() const;
    D3D12_RECT* scissor() const;
    DirectX::DX12::SpriteBatch* spriteBatch();
    Texture* dummyTexure() const;
#pragma endregion

    void setDummy(Texture* dummy) { m_PDummyTexture = dummy; }
    //  PSOの登録
    void addGPSO(const std::string& key, ComPtr<ID3D12PipelineState> pso);
    void addCPSO(const std::string& key, ComPtr<ID3D12PipelineState> pso);
    
    //  copy to srv
    void copyRenderTarget(ID3D12Resource* srcRes, ID3D12Resource* dstRes);
    //  バックバッファの取得
    ColorTarget* getBackBuffer();

    //  表示の差し替え
    void present(uint32_t iterval);
private:
	bool init();
    void term();

    ComPtr<ID3D12Device>        m_PDevice;                   // デバイスです.
    ComPtr<ID3D12CommandQueue>  m_PQueue;                    // コマンドキューです.
    ComPtr<IDXGISwapChain3>     m_PSwapChain;                // スワップチェインです.
    ColorTarget                 m_ColorTarget[App::m_FrameCount];   // カラーターゲットです.
    UnorderAccessTarget m_UnorderAccessTarget;  //  CSの出力ターゲット
    DepthTarget                 m_DepthTarget;               // 深度ターゲットです.
    DescriptorPool* m_PPool[POOL_COUNT];         // ディスクリプタプールです.
    CommandList                 m_CommandList;               // コマンドリストです.
    uint32_t                    m_FrameIndex;                // フレーム番号です.
    Fence                       m_Fence;                     // フェンスです.
    D3D12_VIEWPORT              m_Viewport;                  // ビューポートです.
    D3D12_RECT                  m_Scissor;                   // シザー矩形です.

    std::unique_ptr<DirectX::GraphicsMemory> m_GraphicsMemory;
    std::unique_ptr<DirectX::DX12::SpriteBatch> m_SpriteBatch;			//	スプライトバッチ
    Texture* m_PDummyTexture;

    std::unordered_map<std::string, ComPtr<ID3D12PipelineState>>m_PGraphicPSOMap;		//	パイプラインス
    std::unordered_map<std::string, ComPtr<ID3D12PipelineState>>m_PComputePSOMap;		//	パイプラインス

};

