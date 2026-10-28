#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <ComPtr.h>
#include <cstdint>

class DescriptorHandle;
class DescriptorPool;

//  読書き可能な描画リソース用クラス
class UnorderAccessTarget
{
public:
	UnorderAccessTarget();
	~UnorderAccessTarget();

	/**
	 * @brief           初期化
	 * @param pDevice   デバイス
	 * @param pPoolRTV  プール
	 * @param width     幅   
	 * @param height    高さ
	 * @param pSwapChain スワップチェーン
	 * @return 
	 */
	bool init(
        ID3D12Device* pDevice,
        DescriptorPool* pPoolRTV,
        uint32_t        width,
        uint32_t        height, 
        IDXGISwapChain* pSwapChain
    );

    //  終了処理
	void term();

    //  getter
    DescriptorHandle* getHandleUAV() const;
    ID3D12Resource* getResource() const;
    D3D12_RESOURCE_DESC getDesc() const;
    D3D12_UNORDERED_ACCESS_VIEW_DESC getViewDesc() const;

private:
    ComPtr<ID3D12Resource>          m_PTarget;          // リソースです.
    DescriptorHandle* m_PHandleUAV;       // ディスクリプタハンドル(UAV用)です.
    DescriptorPool* m_PPoolUAV;         // ディスクリプタプール(UAV用)です.
    D3D12_UNORDERED_ACCESS_VIEW_DESC   m_ViewDesc;         // UAV

    UnorderAccessTarget(const UnorderAccessTarget&) = delete;
    void operator = (const UnorderAccessTarget&) = delete;
};

