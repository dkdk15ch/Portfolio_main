#pragma once
#include <d3d12.h>
#include <ComPtr.h>
#include <cstdint>

class DescriptorHandle;
class DescriptorPool;

//  深度バッファ
class DepthTarget
{
public:
    DepthTarget();
    ~DepthTarget();

    //-------------------------------------------------------------------------
    // @brief      初期化処理を行います.
    //
    // @param[in]      pDevice     デバイスです.
    // @param[in]      pPoolDSV    ディスクリプタプールです.
    // @param[in]      width       横幅です.
    // @param[in]      height      縦幅です.
    // @param[in]      format      ピクセルフォーマットです.
    // @retval true    初期化に成功.
    // @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    bool init(
        ID3D12Device* pDevice,
        DescriptorPool* pPoolDSV,
        uint32_t        width,
        uint32_t        height,
        DXGI_FORMAT     format);

    //  終了処理
    void term();

    // ディスクリプタハンドル(DSV用)を取得します.
    DescriptorHandle* getHandleDSV() const;

    // リソースを取得します.
    ID3D12Resource* getResource() const;

    // リソース設定を取得します.
    D3D12_RESOURCE_DESC getDesc() const;

    //  深度ステンシルビューの設定を取得します.
    D3D12_DEPTH_STENCIL_VIEW_DESC getViewDesc() const;

private:
    ComPtr<ID3D12Resource>          m_PTarget;          // リソースです.
    DescriptorHandle* m_PHandleDSV;       // ディスクリプタハンドル(DSV用)です.
    DescriptorPool* m_PPoolDSV;         // ディスクリプタプール(DSV用)です.
    D3D12_DEPTH_STENCIL_VIEW_DESC   m_ViewDesc;         // 深度ステンシルビューの設定です.

    DepthTarget(const DepthTarget&) = delete;
    void operator = (const DepthTarget&) = delete;
}; 
