#pragma once
#include <d3d12.h>
#include <ComPtr.h>
#include <cstdint>

//  インデックスバッファ用クラス
class IndexBuffer
{
public:
    IndexBuffer();
    ~IndexBuffer();

    //-------------------------------------------------------------------------
    // @brief      初期化処理を行います.
    //
    // @param[in]      pDevice         デバイスです.
    // @param[in]      size            インデックスバッファサイズです.
    // @param[in]      pInitData       初期化データです.
    // @retval true    初期化に成功.
    // @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    bool init(ID3D12Device* pDevice, size_t size, const uint32_t* pInitData = nullptr);

    //  終了処理
    void term();

    //  メモリマッピング
    uint32_t* map();

    //  マッピング解除
    void unmap();

    //  インデックスバッファビューの取得
    D3D12_INDEX_BUFFER_VIEW getView() const;

private:
    ComPtr<ID3D12Resource>      m_PIB;      //< インデックスバッファです.
    D3D12_INDEX_BUFFER_VIEW     m_View;     //< インデックスバッファビューです.

    IndexBuffer(const IndexBuffer&) = delete;
    void operator = (const IndexBuffer&) = delete;
};

