#define NOMINMAX
#include "Util/Loader.h"
#include "FIleUtil.h"
#include "resMesh.h"
#include "Material.h"
#include "GraphicContext.h"
#include "DrawHelper/Image.h"
#include "Logger.h"
#include "StringHelper.h"
#include "PointerHelper.h"

#include "Mesh.h"
#include <Interface/IAsset.h>

#include "Animation/Animation.h"
#include "Animation/Model.h"




std::string Loader::model(const std::wstring& path)
{
    std::string temp;

    auto mgr = m_PAssetManager;
    auto key = mgr->getKeyFromPath(path, AssetType::MODEL);
    if (key.empty()) {
        auto model = MAKE_UNIQUE_LOG<Model>();
        auto data = mgr->Read(WStringToString(path).c_str());
        model->init(path, data.data(), data.size());
        temp = model->name();
        mgr->Add(path, temp, std::move(model));
    }
    else {
        auto model = mgr->getModel(key);
        temp = model->name();
    }
    return temp;
}

void Loader::animation(const std::wstring& path, Model* model, bool bIsLoop, double timeScale)
{
    assert(model);
    auto mgr = m_PAssetManager;
    auto data = mgr->Read(WStringToString(path).c_str());
    return model->addAnimation(path, data.data(), data.size(), bIsLoop, timeScale);
}


void Loader::animation(animaDesc desc)
{
    assert(desc.model);
    assert(!desc.path.empty() || desc.data || desc.size > 0);
    return desc.model->addAnimation(desc.path, desc.data, desc.size, desc.isLoop, desc.timeScale);

}

bool Loader::mesh(const wchar_t* filepath, Mesh& mesh, Material& material, DirectX::XMFLOAT2 Tiling)
{
    auto graphicContext = GraphicContext::m_Instance;
    auto device = graphicContext->device();
    auto pool = graphicContext->poolRes();

    std::wstring pathW = filepath;
    std::string path = WStringToString(pathW);
    std::vector<ResMesh_Static>        resMesh;
    std::vector<ResMaterial>    resMaterial;

    auto mgr = m_PAssetManager;
    auto data = mgr->Read(path.c_str());
    if (data.empty()) {
        if (!SearchFilePath(filepath, pathW))
        {
            ELOG("Error : File Not Found.");
            return false;
        }
        // メッシュリソースをロード.
        if (!LoadStaticMesh(pathW.c_str(), resMesh, resMaterial))
        {
            ELOG("Error : Load Mesh Failed. filepath = %ls", pathW.c_str());
            return false;
        }
    }
    else {
        if (!LoadStaticMeshFromMemroy(data.data(), data.size(), resMesh, resMaterial))
        {
            ELOG("Error : Load Mesh Failed. filepath = %ls", pathW.c_str());
            return false;
        }
    }

    std::wstring dir = GetDirectoryPath(pathW.c_str());
    // 初期化処理.
    if (!mesh.init_Static(device, resMesh[0]))
    {
        ELOG("Error : Mesh Initialize Failed.");
        return false;
    }

    // マテリアル初期化.
    if (!material.init(
        device,
        pool,
        sizeof(CB::MaterialBuffer),
        resMaterial.size(),
        pathW
    ))
    {
        ELOG("Error : Material::init() Failed.");
        return false;
    }

    // リソースバッチを用意.
    DirectX::ResourceUploadBatch batch(device);

    // バッチ開始.
    batch.Begin();

    //  テクスチャとマテリアルを設定.
    for (size_t i = 0; i < resMaterial.size(); ++i)
    {
        auto pMaterialBuffer = material.getBufferPtr<CB::MaterialBuffer>(i);
        pMaterialBuffer->Diffuse = resMaterial[i].Diffuse;
        pMaterialBuffer->Alpha = resMaterial[i].Alpha;
        pMaterialBuffer->TilingX = Tiling.x;
        pMaterialBuffer->TilingY = Tiling.y;

        std::wstring filename = RemoveDirectoryPathW(resMaterial[i].DiffuseMap);
        filename = ConvFileExtension(filename, L".dds");

        std::wstring path = dir + L"textures/" + filename;
        material.setTexture(i, TU_DIFFUSE, path, batch);
    }
    // バッチ終了.
    auto future = batch.End(graphicContext->queue());

    return true;
}

bool Loader::MESH(const wchar_t* filepath, Mesh& mesh, Material& material, DirectX::XMFLOAT2 Tiling)
{
    auto graphicContext = GraphicContext::m_Instance;
    auto device = graphicContext->device();
    auto pool = graphicContext->poolRes();

    std::wstring pathW = filepath;
    std::string path = WStringToString(pathW);
    std::vector<ResMesh_Static>        resMesh;
    std::vector<ResMaterial>    resMaterial;
    const auto mgr = m_PAssetManager;
    auto data = mgr->Read(path.c_str());

    if (data.empty()) {
        if (!SearchFilePath(filepath, pathW))
        {
            ELOG("Error : File Not Found.");
            return false;
        }
        // メッシュリソースをロード.
        if (!LoadStaticMesh(pathW.c_str(), resMesh, resMaterial))
        {
            ELOG("Error : Load Mesh Failed. filepath = %ls", pathW.c_str());
            return false;
        }
    }
    else {
        if (!LoadStaticMeshFromMemroy(data.data(), data.size(), resMesh, resMaterial))
        {
            ELOG("Error : Load Mesh Failed. filepath = %ls", pathW.c_str());
            return false;
        }
    }

    std::wstring dir = GetDirectoryPath(pathW.c_str());
    std::wstring texDir = ReplaceW(dir, L"/Mesh/", L"/Textures/");


    // 初期化処理.
    if (!mesh.init_Static(device, resMesh[0]))
    {
        ELOG("Error : Mesh Initialize Failed.");
        return false;
    }

    // マテリアル初期化.
    if (!material.init(
        device,
        pool,
        sizeof(CB::MaterialBuffer),
        resMaterial.size(),
        pathW// owner名
    ))
    {
        ELOG("Error : Material::init() Failed.");
        return false;
    }

    // リソースバッチを用意.
    DirectX::ResourceUploadBatch batch(device);

    // バッチ開始.
    batch.Begin();
    TexInfo texInfo;

    //  テクスチャとマテリアルを設定.
    for (size_t i = 0; i < resMaterial.size(); ++i)
    {
        auto pMaterialBuffer = material.getBufferPtr<CB::MaterialBuffer>(i);
        pMaterialBuffer->Diffuse = resMaterial[i].Diffuse;
        pMaterialBuffer->Alpha = resMaterial[i].Alpha;
        pMaterialBuffer->TilingX = Tiling.x;
        pMaterialBuffer->TilingY = Tiling.y;
        if (!resMaterial[i].DiffuseMap.empty())
        {
            //  パス作成
            std::wstring filename = RemoveDirectoryPathW(resMaterial[i].DiffuseMap);
            filename = ConvFileExtension(filename, L".dds");
            std::wstring texPath = texDir + filename;
            std::wstring folderAndFile = GetFolderAndFileNameW(texPath.c_str());
            folderAndFile = RemoveExtensionW(folderAndFile);
            //  キー作成(上位フォルダ＋ファイル名)
            std::string key = WStringToString(folderAndFile);
            material.setTexture(i, TU_DIFFUSE, texPath, key, batch);
            texInfo.key = key;
        }
        else {
            texInfo.key = "";

        }
    }
    // バッチ終了.
    auto future = batch.End(graphicContext->queue());

    texInfo.resMat = resMaterial;
    texInfo.tiling = Tiling;
    mesh.setTexInfoList(texInfo);

    return true;
}

bool Loader::meshFromMemroy(const MeshDesc& desc, Mesh* mesh, Material* material)
{
    if (desc.key.empty() || !mesh || !material) { return false; }

    auto graphicContext = GraphicContext::m_Instance;
    auto device = graphicContext->device();
    auto pool = graphicContext->poolRes();

    std::wstring path = StringToWString(desc.key);
    const auto mgr = m_PAssetManager;

    std::wstring dir = GetDirectoryPath(path.c_str());
    std::wstring texDir = ReplaceW(dir, L"/Mesh/", L"/Textures/");

    std::vector<ResMesh_Static>        resMesh;
    std::vector<ResMaterial>    resMaterial;

    // メッシュリソースをロード.
    auto meshData = m_PAssetManager->Read(desc.key.c_str());
    if (!LoadStaticMeshFromMemroy(meshData.data(), meshData.size(), resMesh, resMaterial))
    {
        ELOG("Error : Load Mesh Failed. filepath = %ls", path.c_str());
        return false;
    }

    // 初期化処理.
    if (!mesh->init_Static(device, resMesh[0]))
    {
        ELOG("Error : Mesh Initialize Failed.");
        return false;
    }

    // マテリアル初期化.
    if (!material->init(
        device,
        pool,
        sizeof(CB::MaterialBuffer),
        resMaterial.size(),
        path// owner名
    ))
    {
        ELOG("Error : Material::init() Failed.");
        return false;
    }

    // リソースバッチを用意.
    DirectX::ResourceUploadBatch batch(device);

    // バッチ開始.
    batch.Begin();
    TexInfo texInfo;

    //  テクスチャとマテリアルを設定.
    for (size_t i = 0; i < resMaterial.size(); ++i)
    {
        auto pMaterialBuffer = material->getBufferPtr<CB::MaterialBuffer>(i);
        pMaterialBuffer->Diffuse = resMaterial[i].Diffuse;
        pMaterialBuffer->Alpha = resMaterial[i].Alpha;
        pMaterialBuffer->TilingX = desc.Tiling.x;
        pMaterialBuffer->TilingY = desc.Tiling.y;
        if (!resMaterial[i].DiffuseMap.empty())
        {
            //  パス作成
            std::wstring filename = RemoveDirectoryPathW(resMaterial[i].DiffuseMap);
            filename = ConvFileExtension(filename, L".dds");
            std::wstring texPath = texDir + filename;
            std::wstring folderAndFile = GetFolderAndFileNameW(texPath.c_str());
            folderAndFile = RemoveExtensionW(folderAndFile);
            //  キー作成(上位フォルダ＋ファイル名)
            std::string key = WStringToString(folderAndFile);
            auto texData = m_PAssetManager->Read(WStringToString(texPath).c_str());
            material->setTexture(i, TU_DIFFUSE, texPath, key, batch, texData.data(), texData.size());
            texInfo.key = key;
        }
        else {
            texInfo.key = "";

        }
    }
    // バッチ終了.
    auto future = batch.End(graphicContext->queue());

    texInfo.resMat = resMaterial;
    texInfo.tiling = desc.Tiling;
    mesh->setTexInfoList(texInfo);

    return true;
}

// Loader::meshのmaterialを含めない版
std::unique_ptr<Mesh> Loader::meshOnly(ID3D12Device* device, DescriptorPool* pool, const wchar_t* filepath)
{
    std::wstring pathW = filepath;
    std::string path = WStringToString(pathW);
    std::vector<ResMesh_Static>        resMesh;
    std::vector<ResMaterial>    resMaterial;
    const auto mgr = m_PAssetManager;
    auto data = mgr->Read(path.c_str());

    if (data.empty()) {
        if (!SearchFilePath(filepath, pathW))
        {
            ELOG("Error : File Not Found.");
            return nullptr;
        }
        // メッシュリソースをロード.
        if (!LoadStaticMesh(pathW.c_str(), resMesh, resMaterial))
        {
            ELOG("Error : Load Mesh Failed. filepath = %ls", pathW.c_str());
            return nullptr;
        }
    }
    else {
        if (!LoadStaticMeshFromMemroy(data.data(), data.size(), resMesh, resMaterial))
        {
            ELOG("Error : Load Mesh Failed. filepath = %ls", pathW.c_str());
            return nullptr;
        }
    }

    std::wstring dir = GetDirectoryPath(pathW.c_str());

    //// メッシュ生成.
    auto mesh = std::make_unique<Mesh>();

    // チェック.
    if (mesh == nullptr)
    {
        ELOG("Error : Out of memory.");
        return nullptr;
    }

    // 初期化処理.
    if (!mesh->init_Static(device,resMesh[0]))
    {
        ELOG("Error : Mesh Initialize Failed.");
        return nullptr;
    }

    return mesh;
}


//  BoundingSizeを取得
//  CubeとSphereのみ対応
Loader::BoundingData Loader::boudingSize(DescriptorPool* pool, const wchar_t* filepath, bool bIsCube)
{
    auto result = BoundingData();
    result.maxVertex = DirectX::XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    result.minVertex = DirectX::XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
    result.radius = 0.0f;

    std::wstring pathW = filepath;
    std::string path = WStringToString(pathW);
    std::vector<ResMesh_Static>        resMesh;
    std::vector<ResMaterial>    resMaterial;
    
    const auto mgr = m_PAssetManager;
    auto data = mgr->Read(path.c_str());

    if (data.empty()) {
        if (!SearchFilePath(filepath, pathW))
        {
            ELOG("Error : File Not Found.");
            return result;
        }
        // メッシュリソースをロード.
        if (!LoadStaticMesh(pathW.c_str(), resMesh, resMaterial))
        {
            ELOG("Error : Load Mesh Failed. filepath = %ls", pathW.c_str());
            return result;
        }
    }
    else {
        if (!LoadStaticMeshFromMemroy(data.data(), data.size(), resMesh, resMaterial))
        {
            ELOG("Error : Load Mesh Failed. filepath = %ls", pathW.c_str());
            return result;
        }
    }

    std::wstring dir = GetDirectoryPath(pathW.c_str());

    //  min/max radiusを格納
    for (const auto& vertex : resMesh[0].Vertices) {
        // X, Y, Z それぞれで最小値・最大値を記録
        result.minVertex.x = std::min(result.minVertex.x, vertex.m_Position.x);
        result.minVertex.y = std::min(result.minVertex.y, vertex.m_Position.y);
        result.minVertex.z = std::min(result.minVertex.z, vertex.m_Position.z);

        result.maxVertex.x = std::max(result.maxVertex.x, vertex.m_Position.x);
        result.maxVertex.y = std::max(result.maxVertex.y, vertex.m_Position.y);
        result.maxVertex.z = std::max(result.maxVertex.z, vertex.m_Position.z);
    }

    if (bIsCube) return result;    //  Cubeの場合ココまで

    // 中心を計算
    DirectX::XMFLOAT3 center = {
        (result.minVertex.x + result.maxVertex.x) * 0.5f,
        (result.minVertex.y + result.maxVertex.y) * 0.5f,
        (result.minVertex.z + result.maxVertex.z) * 0.5f
    };

    // 中心から最大頂点までの距離を求める
    result.radius = 0.0f;
    for (const auto& vertex : resMesh[0].Vertices) {
        float dx = vertex.m_Position.x - center.x;
        float dy = vertex.m_Position.y - center.y;
        float dz = vertex.m_Position.z - center.z;
        float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
        result.radius = std::max(result.radius, dist);
    }

    return result;
}


bool Loader::texture(const wchar_t* filepath, Material* material, std::wstring ownerName, 
    DirectX::XMFLOAT2 Tiling, bool bUseDummy)
{
    if (!material) {
        ELOG("Error - material null");
        return false;
    }
    //  マネージャーをチェック
    std::wstring pathW = filepath;
    const auto mgr = m_PAssetManager;
    if (mgr->containsFromPath(pathW, AssetType::TEXTURE)) {
        auto key = mgr->getKeyFromPath(pathW, AssetType::TEXTURE);
        bool result = setLoadedTexture(key, material, ownerName, Tiling);
        if (result) { return true; }
    }
    //  アーカイブチェック
    std::string path = WStringToString(pathW);
    auto data = m_PAssetManager->Read(WStringToString(filepath).c_str());

    if (data.empty() && !bUseDummy && !SearchFilePath(filepath, pathW))
    {
        ELOG("Error : File Not Found.");
        return false;
    }
     
    auto graphicContext = GraphicContext::m_Instance;
    auto device = graphicContext->device();
    auto pool = graphicContext->poolRes();
    // マテリアルが未初期化なら作成・初期化
    if (!material->init(
        device,
        pool,
        sizeof(CB::MaterialBuffer), 1,
        ownerName
    ))
    {
        ELOG("Error : Material::init() Failed.");
        return false;
    }

    // リソースバッチを用意
    DirectX::ResourceUploadBatch batch(device);
    batch.Begin();
    //  Materialを設定
    auto ptr = material->getBufferPtr<CB::MaterialBuffer>(0);
    ptr->Alpha = 1.0f;
    ptr->TilingX = Tiling.x;
    ptr->TilingY = Tiling.y;
    // テクスチャを設定
    material->setTexture(0, TU_DIFFUSE, pathW, path, batch, data.data(), data.size());

    // バッチ終了
    auto future = batch.End(graphicContext->queue());
    future.wait();

    return true;
}

bool Loader::texture(
    const std::string& key,
    Material* material, 
    std::wstring ownerName, 
    const TexInfo& info, 
    bool bUseDummy)
{
    std::wstring path;
    const auto mgr = m_PAssetManager;
    if (mgr->getTexture(key) || key.empty()) {
        bool result = setLoadedTexture(key, material, ownerName, info);
        if (result) { return true; }
    }
    return false;
}

bool Loader::setLoadedTexture(
    const std::string& key, 
    Material* material, 
    std::wstring ownerName, 
    DirectX::XMFLOAT2 Tiling)
{
    auto graphicContext = GraphicContext::m_Instance;
    auto device = graphicContext->device();
    auto pool = graphicContext->poolRes();

    if (!material->init(
        device,
        pool,
        sizeof(CB::MaterialBuffer), 1,
        ownerName
    ))
    {
        ELOG("Error : Material::init() Failed.");
        return false;
    }

    //  Materialを設定
    auto ptr = material->getBufferPtr<CB::MaterialBuffer>(0);
    ptr->TilingX = Tiling.x;
    ptr->TilingY = Tiling.y;
    // テクスチャを設定
    material->setLoadedTexture(0, TU_DIFFUSE, key);

    return true;
}

bool Loader::setLoadedTexture(
    const std::string& key,
    Material* material,
    std::wstring ownerName,
    const TexInfo& info)
{
    auto graphicContext = GraphicContext::m_Instance;
    auto device = graphicContext->device();
    auto pool = graphicContext->poolRes();

    if (!material->init(
        device,
        pool,
        sizeof(CB::MaterialBuffer), 1,
        ownerName
    ))
    {
        ELOG("Error : Material::init() Failed.");
        return false;
    }

    //  Materialを設定
    auto ptr = material->getBufferPtr<CB::MaterialBuffer>(0);
    ptr->Diffuse = info.resMat[0].Diffuse;
    ptr->Alpha = info.resMat[0].Alpha;
    ptr->TilingX = info.tiling.x;
    ptr->TilingY = info.tiling.y;
    // テクスチャを設定
    material->setLoadedTexture(0, TU_DIFFUSE, key);

    return true;
}

/**
* @brief           画像のロード
* @param device    デバイス
* @param pool      ディスクリプタプール
* @param filepath  ファイルパス 例) res/xxx/xxx.dds
*                  ※dds化すること。
* @return
*/
std::unique_ptr<Image> Loader::image(ID3D12Device* device, DescriptorPool* pool, const wchar_t* filepath, const char* data, int size)
{
    if (!device || !pool)
    {
        ELOG("Error : DrawManager::Loader::image()");
        return nullptr;
    }

    DirectX::ResourceUploadBatch uploadBatch(device);
    uploadBatch.Begin();
    std::wstring path = filepath;
    if (!data || size <= 0) {
        //  パスチェック
        if (!SearchFilePath(filepath, path))
        {
            ELOG("Error : File Not Found.");
            return nullptr;
        }
    }

    auto image = std::make_unique<Image>(device, pool, uploadBatch, path, data, size);

    auto graphicContext = GraphicContext::m_Instance;
    auto uploadFinished = uploadBatch.End(graphicContext->queue());
    uploadFinished.wait();

    return image;
}
