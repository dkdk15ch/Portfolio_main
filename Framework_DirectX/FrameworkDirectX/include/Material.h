#pragma once
#include <DescriptorPool.h>
#include <ResourceUploadBatch.h>
#include <Texture.h>
#include <ConstantBuffer.h>
#include <map>
#include "Interface/IAsset.h"

//  マテリアル用クラス
class Material : public IAsset
{
public:
    enum TEXTURE_USAGE
    {
        TEXTURE_USAGE_DIFFUSE = 0,  // ディフューズマップとして利用します.
        TEXTURE_USAGE_SPECULAR,     // スペキュラーマップとして利用します.
        TEXTURE_USAGE_SHININESS,    // シャイネスマップとして利用します.
        TEXTURE_USAGE_NORMAL,       // 法線マップとして利用します.

        TEXTURE_USAGE_COUNT
    };

    Material();
    ~Material();

    //-------------------------------------------------------------------------
    // @brief      初期化処理を行います.
    //
    // @param[in]      pDevice         デバイスです.
    // @param[in]      pPool           ディスクリプタプールです(CBV_UAV_SRV用のものを設定します).
    // @param[in]      bufferSize      1マテリアルあたりの定数バッファのサイズです.
    // @param[in]      count           マテリアル数です.
    // @retval true    初期化に成功.
    // @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    bool init(
        ID3D12Device* pDevice,
        DescriptorPool* pPool,
        size_t          bufferSize,
        size_t          count,
        std::wstring& ownerName
       );

    //  終了処理
    void term();

    //-------------------------------------------------------------------------
    // @brief      テクスチャを設定します.
    //
    // @param[in]      index       マテリアル番号です.
    // @param[in]      usage       テクスチャの使用用途です.
    // @param[in]      path        テクスチャパスです.
    // @param[out]     batch       リソースアップロードバッチです.
    // @retval true    設定に成功.
    // @retval false   設定に失敗.
    //-------------------------------------------------------------------------
    bool setTexture(
        size_t                          index,
        TEXTURE_USAGE                   usage,
        const std::wstring& path,
        DirectX::ResourceUploadBatch& batch);

    bool setTexture(
        size_t                          index,
        TEXTURE_USAGE                   usage,
        const std::wstring& path,
        const std::string& registKey,
        DirectX::ResourceUploadBatch& batch,
        const char* data = nullptr,
        int size = 0
        );

    bool setLoadedTexture(
        size_t                          index,
        TEXTURE_USAGE                   usage,
        const std::string& key
    );

    //-------------------------------------------------------------------------
    // @brief      定数バッファのポインタを取得します.
    //
    // @param[in]      index       取得するマテリアル番号です.
    // @return     指定された番号に一致する定数バッファのポインタを返却します.
    //             一致するものが無い場合は nullptr を返却します.
    //-------------------------------------------------------------------------
    void* getBufferPtr(size_t index) const;

    //-------------------------------------------------------------------------
    // @brief      定数バッファのポインタを取得します.
    //
    // @param[in]      index       取得するマテリアル番号です.
    // @return     指定された番号に一致する定数バッファのポインタを返却します.
    //             一致するものが無い場合は nullptr を返却します.
    //-------------------------------------------------------------------------
    template<typename T>
    T* getBufferPtr(size_t index) const
    {
        return reinterpret_cast<T*>(getBufferPtr(index));
    }

    //-------------------------------------------------------------------------
    // @brief      定数バッファのGPU仮想アドレスを取得します.
    //
    // @param[in]      index       取得するマテリアル番号です.
    // @return     指定された番号に一致する定数バッファのGPU仮想アドレスを返却します.
    //-------------------------------------------------------------------------
    D3D12_GPU_VIRTUAL_ADDRESS getBufferAddress(size_t index) const;

    //-------------------------------------------------------------------------
    // @brief      テクスチャハンドルを取得します.
    //
    // @param[in]      index       取得するマテリアル番号です.
    // @param[in]      usage       取得するテクスチャの使用用途です.
    // @return     指定された番号に一致するテクスチャのディスクリプタハンドルを返却します.
    //-------------------------------------------------------------------------
    D3D12_GPU_DESCRIPTOR_HANDLE getTextureHandle(size_t index, TEXTURE_USAGE usage) const;

    //-------------------------------------------------------------------------
    // @brief      マテリアル数を取得します.
    //
    // @return     マテリアル数を返却します.
    //-------------------------------------------------------------------------
    size_t getCount() const;

    void setTiling(DirectX::XMFLOAT2 tiling, size_t index = 0);

private:
    struct Subset
    {
        ConstantBuffer* pCostantBuffer;                     // 定数バッファです.
        //D3D12_GPU_DESCRIPTOR_HANDLE     TextureHandle[TEXTURE_USAGE_COUNT]; // テクスチャハンドルです.
        Texture* pTextures[TEXTURE_USAGE_COUNT];
    };

    std::map<std::string, Texture*>    m_PTexture;     // テクスチャです.
    std::vector<Subset>                 m_Subset;       // サブセットです.
    ID3D12Device* m_PDevice;      // デバイスです.
    DescriptorPool* m_PPool;        // ディスクリプタプールです(CBV_UAV_SRV).

    Material(const Material&) = delete;
    void operator = (const Material&) = delete;
};

constexpr auto TU_DIFFUSE = Material::TEXTURE_USAGE_DIFFUSE;
constexpr auto TU_SPECULAR = Material::TEXTURE_USAGE_SPECULAR;
constexpr auto TU_SHININESS = Material::TEXTURE_USAGE_SHININESS;
constexpr auto TU_NORMAL = Material::TEXTURE_USAGE_NORMAL;
