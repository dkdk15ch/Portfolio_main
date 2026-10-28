#pragma once
#include <d3d12.h>
#include <ComPtr.h>

//  頂点バッファ用クラス
class VertexBuffer
{
public:
    VertexBuffer();
    ~VertexBuffer();

    //-------------------------------------------------------------------------
    // @brief      初期化処理を行います.
    //
    // @param[in]      pDevice         デバイスです.
    // @param[in]      size            頂点バッファサイズです.
    // @param[in]      stride          1頂点あたりのサイズです.
    // @param[in]      pInitData       初期化データです.
    // @retval true    初期化に成功.
    // @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    bool init(ID3D12Device* pDevice, size_t size, size_t stride, const void* pInitData = nullptr);

    //-------------------------------------------------------------------------
    // @brief      初期化処理を行います.
    //
    // @param[in]      pDevice         デバイスです.
    // @param[in]      size            頂点バッファサイズです.
    // @param[in]      pInitData       初期化データです.
    // @retval true    初期化に成功.
    // @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    template<typename T>
    bool init(ID3D12Device* pDevice, size_t size, const T* pInitData = nullptr)
    {
        return init(pDevice, size, sizeof(T), pInitData);
    }

    //  終了処理
    void term();

    //  マッピング・解除
    void* map();
    void unmap();
    template<typename T>
    T* map() const
    {
        return reinterpret_cast<T*>(map());
    }

    //  頂点バッファビューを取得します.
    D3D12_VERTEX_BUFFER_VIEW getView() const;

private:
    ComPtr<ID3D12Resource>      m_PVB;      // 頂点バッファです.
    D3D12_VERTEX_BUFFER_VIEW    m_View;     // 頂点バッファビューです.

    VertexBuffer(const VertexBuffer&) = delete;     // アクセス禁止.
    void operator = (const VertexBuffer&) = delete;     // アクセス禁止.
};

