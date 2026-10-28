#pragma once
#include <ResMesh.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <Interface/IAsset.h>

struct TexInfo {
    std::vector<ResMaterial> resMat;
    std::string key;
    DirectX::XMFLOAT2 tiling{};
};

//  メッシュ用クラス
class Mesh : public IAsset
{
public:
    Mesh();
    virtual ~Mesh();

    AssetType type() override { return AssetType::MESH; }

    //-------------------------------------------------------------------------
    // @brief      初期化処理を行います.
    //
    // @param[in]      pDevice         デバイスです.
    // @param[in]      resource        リソースメッシュです.
    // @retval true    初期化に成功.
    // @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    bool init_Static(ID3D12Device* device, const ResMesh_Static& resource);
    bool init_Skinned(ID3D12Device* device, const ResMesh_Skinned& resource);

    //  終了処理
    void term();

    //-------------------------------------------------------------------------
    // @brief      描画処理を行います.
    //
    // @param[in]      pCmdList        コマンドリストです.
    //-------------------------------------------------------------------------
    void draw(ID3D12GraphicsCommandList* pCmdList, UINT instanceCount = 1);

    //-------------------------------------------------------------------------
    // @brief      マテリアルIDを取得します.
    //
    // @return     マテリアルIDを返却します.
    //-------------------------------------------------------------------------
    uint32_t getMaterialId() const;

    void setTexInfoList(TexInfo info) {
        m_TexInfoList = std::move(info);
    }
    const TexInfo& texInfoList()const {
        return m_TexInfoList;
    }

private:
    VertexBuffer    m_VB;               // 頂点バッファです.
    IndexBuffer     m_IB;               // インデックスバッファです.
    uint32_t        m_MaterialId;       // マテリアルIDです.
    uint32_t        m_IndexCount;       // インデックス数です.

    TexInfo m_TexInfoList;

    Mesh(const Mesh&) = delete;     // アクセス禁止.
    void operator = (const Mesh&) = delete;     // アクセス禁止.

public:
#ifdef _DEBUG
    static constexpr const char* m_ClassName = "Mesh";
#endif
};