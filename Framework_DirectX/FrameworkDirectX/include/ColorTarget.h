#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <ComPtr.h>
#include <cstdint>

class DescriptorHandle;
class DescriptorPool;

//  描画対象リソース
class ColorTarget
{
public:
    ColorTarget();
    ~ColorTarget();

    //-------------------------------------------------------------------------
    // @brief      初期化処理を行います.
    //
    // @param[in]      pDevice     デバイスです.
    // @param[in]      pPoolRTV    ディスクリプタプールです.
    // @param[in]      width       横幅です.
    // @param[in]      height      縦幅です.
    // @param[in]      format      ピクセルフォーマットです.
    // @retval true    初期化に成功.
    // @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    bool init(
        ID3D12Device* pDevice,
        DescriptorPool* pPoolRTV,
        uint32_t        width,
        uint32_t        height,
        DXGI_FORMAT     format);

    //-------------------------------------------------------------------------
    // @brief      バックバッファから初期化処理を行います.
    //
    // @param[in]      pDevice     デバイスです.
    // @param[in]      pPoolRTV    ディスクリプタプールです.
    // @param[in]      index       バックバッファ番号です.
    // @param[in]      pSwapChain  スワップチェインです.
    // @retval true    初期化に成功.
    // @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    bool initFromBackBuffer(
        ID3D12Device* pDevice,
        DescriptorPool* pPoolRTV,
        DescriptorPool* pPoolRes,
        uint32_t        index,
        IDXGISwapChain* pSwapChain);

    // @brief      終了処理を行います.
    void term();

    //-------------------------------------------------------------------------
    // @brief      ディスクリプタハンドルを取得します.
    //
    // @return     ディスクリプタハンドルを返却します.
    //-------------------------------------------------------------------------
    DescriptorHandle* getHandleRTV() const;
    DescriptorHandle* getHandleSRV() const;

    //-------------------------------------------------------------------------
    // @brief      リソースを取得します.
    //
    // @return     リソースを返却します.
    //-------------------------------------------------------------------------
    ID3D12Resource* getResource() const;

    //-------------------------------------------------------------------------
    // @brief      リソース設定を取得します.
    //
    // @return     リソース設定を返却します.
    //-------------------------------------------------------------------------
    D3D12_RESOURCE_DESC getDesc() const;

    //-------------------------------------------------------------------------
    // @brief      レンダーターゲットビューの設定を取得します.
    //
    // @return     レンダーターゲットビューの設定を返却します.
    //-------------------------------------------------------------------------
    D3D12_RENDER_TARGET_VIEW_DESC getViewDesc() const;

private:
    ComPtr<ID3D12Resource>          m_PTarget;          // リソースです.
    DescriptorHandle* m_PHandleRTV;       // ディスクリプタハンドル(RTV用)です.
    DescriptorHandle* m_PHandleSRV;       // ディスクリプタハンドル(RTV用)です.

    DescriptorPool* m_PPoolRTV;         // ディスクリプタプール(RTV用)です.
    D3D12_RENDER_TARGET_VIEW_DESC   m_ViewDesc;         // レンダーターゲットビューの構成です.

    ColorTarget(const ColorTarget&) = delete;
    void operator = (const ColorTarget&) = delete;

};

