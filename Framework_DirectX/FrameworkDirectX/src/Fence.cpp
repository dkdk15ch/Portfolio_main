#include "Fence.h"

Fence::Fence()
    : m_PFence(nullptr)
    , m_Event(nullptr)
    , m_Counter(0)
{ }


Fence::~Fence(){
    term();
}


bool Fence::init(ID3D12Device* pDevice)
{
    if (pDevice == nullptr)
    {
        return false;
    }

    // イベントを生成.
    m_Event = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
    if (m_Event == nullptr)
    {
        return false;
    }

    // フェンスを生成.
    auto hr = pDevice->CreateFence(
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(m_PFence.GetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }

    m_PFence->SetName(L"Fence");

    // カウンタ設定.
    m_Counter = 1;

    // 正常終了.
    return true;
}


void Fence::term()
{
    // ハンドルを閉じる.
    if (m_Event != nullptr)
    {
        CloseHandle(m_Event);
        m_Event = nullptr;
    }

    // フェンスオブジェクトを破棄.
    m_PFence.Reset();

    // カウンターリセット.
    m_Counter = 0;
}


//  シグナル状態になるまで指定時間待機
void Fence::wait(ID3D12CommandQueue* pQueue, UINT timeout)
{
    if (pQueue == nullptr)
    {
        return;
    }

    const auto fenceValue = m_Counter;

    // シグナル処理.
    auto hr = pQueue->Signal(m_PFence.Get(), fenceValue);
    if (FAILED(hr))
    {
        return;
    }

    // カウンターを増やす.
    m_Counter++;

    // 次のフレームの描画準備がまだであれば待機する.
    if (m_PFence->GetCompletedValue() < fenceValue)
    {
        // 完了時にイベントを設定.
        auto hr = m_PFence->SetEventOnCompletion(fenceValue, m_Event);
        if (FAILED(hr))
        {
            return;
        }

        // 待機処理.
        if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_Event, timeout, FALSE))
        {
            return;
        }
    }
}


//  シグナル状態になるまでずっと待機
void Fence::sync(ID3D12CommandQueue* pQueue)
{
    if (pQueue == nullptr)
    {
        return;
    }

    // シグナル処理.
    auto hr = pQueue->Signal(m_PFence.Get(), m_Counter);
    if (FAILED(hr))
    {
        return;
    }

    // 完了時にイベントを設定.
    hr = m_PFence->SetEventOnCompletion(m_Counter, m_Event);
    if (FAILED(hr))
    {
        return;
    }

    // 待機処理.
    if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_Event, INFINITE, FALSE))
    {
        return;
    }
}


void Fence::signal(ID3D12CommandQueue* pQueue)
{
    ++m_Counter;
    pQueue->Signal(m_PFence.Get(), m_Counter);
}
