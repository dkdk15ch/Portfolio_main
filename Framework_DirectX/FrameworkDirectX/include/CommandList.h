#pragma once
#include <d3d12.h>
#include <ComPtr.h>
#include <cstdint>
#include <vector>

//  コマンドリスト用クラス
class CommandList
{
public:
    CommandList();
    ~CommandList();

    //-------------------------------------------------------------------------
    // @brief      初期化処理を行います.
    //
    // @param[in]      pDevice     デバイスです.
    // @param[in]      type        コマンドリストタイプです.
    // @param[in]      count       アロケータの数です.ダブルバッファ化する場合は2に設定します.
    // @retval true    初期化に成功.
    // @retval fasle   初期化に失敗.
    //-------------------------------------------------------------------------
    bool init(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, uint32_t count);

    //-------------------------------------------------------------------------
    // @brief      終了処理を行います.
    //-------------------------------------------------------------------------
    void term();

    //-------------------------------------------------------------------------
    // @brief      リセット処理を行ったコマンドリストを取得します.
    //
    // @return     リセット処理を行ったコマンドリストを返却します.
    //-------------------------------------------------------------------------
    ID3D12GraphicsCommandList* reset();

    //  コマンドリストの取得
    ID3D12GraphicsCommandList* get()const { return m_PCmdList.Get(); }
private:
    ComPtr<ID3D12GraphicsCommandList>           m_PCmdList;         // コマンドリストです.
    std::vector<ComPtr<ID3D12CommandAllocator>> m_PAllocators;      // コマンドアロケータです.
    uint32_t                                    m_Index;            // アロケータ番号です.

    CommandList(const CommandList&) = delete;
    void operator = (const CommandList&) = delete;
};


