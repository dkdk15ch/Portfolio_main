#pragma once
#include <d3d12.h>
#include <ComPtr.h>

//  フェンスクラス
class Fence
{
public:
    Fence();
    ~Fence();

    //-------------------------------------------------------------------------
    //! @brief      初期化処理を行います.
    //!
    //! @param[in]      pDevice         デバイスです.
    //! @retval true    初期化に成功.
    //! @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    bool init(ID3D12Device* pDevice);

    //  終了処理
    void term();

    //-------------------------------------------------------------------------
    //! @brief      シグナル状態になるまで指定された時間待機します.
    //!
    //! @param[in]      pQueue          コマンドキューです.
    //! @param[in]      timeout         タイムアウト時間(ミリ秒).
    //-------------------------------------------------------------------------
    void wait(ID3D12CommandQueue* pQueue, UINT timeout);

    //-------------------------------------------------------------------------
    //! @brief      シグナル状態になるまでずっと待機します.
    //!
    //! @param[in]      pQueue          コマンドキューです.
    //-------------------------------------------------------------------------
    void sync(ID3D12CommandQueue* pQueue);

    void signal(ID3D12CommandQueue* pQueue);

private:
    ComPtr<ID3D12Fence> m_PFence;           //!< フェンスです.
    HANDLE              m_Event;            //!< イベントです.
    UINT                m_Counter;          //!< 現在のカウンターです.

    Fence(const Fence&) = delete;    // アクセス禁止.
    void operator = (const Fence&) = delete;    // アクセス禁止.
};

