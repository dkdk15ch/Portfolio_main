#pragma once
#include <d3d12.h>
#include <ComPtr.h>
#include <ResourceUploadBatch.h>
#include <DirectXMath.h>
#include <Interface/IAsset.h>

class DescriptorHandle;
class DescriptorPool;

//  テクスチャ用クラス
class Texture : public IAsset
{
public:

    Texture();
    ~Texture();

    AssetType type() override { return AssetType::TEXTURE; }

    //-------------------------------------------------------------------------
    // @brief      初期化処理を行います.
    //
    // @param[in]      pDevice     デバイスです.
    // @param[in]      pPool       ディスクリプタプールです.
    // @param[in]      filename    ファイル名です.
    // @param[out]     batch       更新バッチです. テクスチャの更新に必要なデータを格納します.
    // @retval true    初期化に成功.
    // @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    bool init(
        ID3D12Device* pDevice,
        DescriptorPool* pPool,
        const wchar_t* filename,
        DirectX::ResourceUploadBatch& batch);

    bool init(
        ID3D12Device* pDevice,
        DescriptorPool* pPool,
        const wchar_t* filename,
        DirectX::ResourceUploadBatch& batch,
        const char* data,
        int size
        );

    //-------------------------------------------------------------------------
    // @brief      初期化処理を行います.
    //
    // @param[in]      pDevice     デバイスです.
    // @param[in]      pPool       ディスクリプタプールです
    // @param[in]      pDesc       構成設定です.
    // @param[in]      isCube      キューブマップである場合には true を指定します.
    // @retval true    初期化に成功.
    // @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    bool init(
        ID3D12Device* pDevice,
        DescriptorPool* pPool,
        const D3D12_RESOURCE_DESC* pDesc,
        bool                        isCube);

    //-------------------------------------------------------------------------
    // @brief      終了処理を行います.
    //-------------------------------------------------------------------------
    void term();

    ID3D12Resource* getResource()const;

    //  テクスチャサイズの取得
    DirectX::XMUINT2 getTextureSize();

    // CPUディスクリプタハンドルを取得します.
    D3D12_CPU_DESCRIPTOR_HANDLE getHandleCPU() const;

    //  GPUディスクリプタハンドルを取得します.
    D3D12_GPU_DESCRIPTOR_HANDLE getHandleGPU() const;

private:
    ComPtr<ID3D12Resource>  m_PTex;
    DescriptorHandle* m_PHandle;
    DescriptorPool* m_PPool;

    Texture(const Texture&) = delete;      // アクセス禁止.
    void operator = (const Texture&) = delete;      // アクセス禁止.

    //-------------------------------------------------------------------------
    // @brief      シェーダリソースビューの設定を求めます.
    //
    // @param[in]      isCube      キューブマップかどうか?
    // @return     シェーダリソースビューの設定を返却します.
    //-------------------------------------------------------------------------
    D3D12_SHADER_RESOURCE_VIEW_DESC getViewDesc(bool isCube);

public:
#ifdef _DEBUG
    static constexpr const char* m_ClassName = "Texture";
#endif
};