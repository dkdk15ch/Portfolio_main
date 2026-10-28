#include "Material.h"
#include "Texture.h"
#include "FileUtil.h"
#include "GraphicContext.h"
#include "Logger.h"
#include "StringHelper.h"
#include <iostream>

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
Material::Material()
    : m_PDevice(nullptr)
    , m_PPool(nullptr)
{ /* DO_NOTHING */
}

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
Material::~Material()
{
    term();
}

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool Material::init
(
    ID3D12Device* pDevice,
    DescriptorPool* pPool,
    size_t          bufferSize,
    size_t          count,
    std::wstring& ownerName
)
{
    if (pDevice == nullptr || pPool == nullptr)
    {
        ELOG("Error : Invalid Argument.");
        return false;
    }

    term();

    m_PDevice = pDevice;
    m_PDevice->AddRef();

    m_PPool = pPool;
    m_PPool->addRef();

    m_Subset.resize(count);

    auto size = bufferSize * count;
    if (size > 0)
    {
        for (size_t i = 0; i < m_Subset.size(); ++i)
        {
            auto pBuffer = new (std::nothrow) ConstantBuffer();
            if (pBuffer == nullptr)
            {
                ELOG("Error : Out of memory.");
                return false;
            }

            if (!pBuffer->init(pDevice, pPool, bufferSize,ownerName))
            {
                ELOG("Error : ConstantBuffer::init() Failed.");
                return false;
            }

            m_Subset[i].pCostantBuffer = pBuffer;
            for (auto j = 0; j < TEXTURE_USAGE_COUNT; ++j)
            {
                m_Subset[i].pTextures[j] = nullptr;
                //m_Subset[i].TextureHandle[j].ptr = 0;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < m_Subset.size(); ++i)
        {
            m_Subset[i].pCostantBuffer = nullptr;
            for (auto j = 0; j < TEXTURE_USAGE_COUNT; ++j)
            {
                m_Subset[i].pTextures[j] = nullptr;
                //m_Subset[i].TextureHandle[j].ptr = 0;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void Material::term()
{
    for (size_t i = 0; i < m_Subset.size(); ++i)
    {
        if (m_Subset[i].pCostantBuffer != nullptr)
        {
            m_Subset[i].pCostantBuffer->term();
            delete m_Subset[i].pCostantBuffer;
            m_Subset[i].pCostantBuffer = nullptr;
        }
    }

    m_Subset.clear();
    m_PTexture.clear();

    if (m_PDevice != nullptr)
    {
        m_PDevice->Release();
        m_PDevice = nullptr;
    }
    
    if (m_PPool != nullptr)
    {
        m_PPool->release();
        m_PPool = nullptr;
    }
}

//-----------------------------------------------------------------------------
//      テクスチャを設定します.
//-----------------------------------------------------------------------------
bool Material::setTexture
(
    size_t                          index,
    TEXTURE_USAGE                   usage,
    const std::wstring& path,
    DirectX::ResourceUploadBatch& batch
)
{
    // 範囲内であるかチェック.
    if (index >= getCount())
    {
        return false;
    }    
    std::wstring addKeyW = GetFileNameW(path.c_str());
    std::string addKey = WStringToString(addKeyW);
    // 既に登録済みかチェック.
    if (m_PTexture.find(addKey) != m_PTexture.end())
    {
        //m_Subset[index].TextureHandle[usage] = m_pTexture[addKey]->GetHandleGPU();
        m_Subset[index].pTextures[usage] = m_PTexture[addKey];
        return true;
    }
    // ファイルパスが存在するかチェックします.
    std::wstring findPath;
    if (!SearchFilePathW(path.c_str(), findPath))
    {
        return true;
    }
    // ファイル名であることをチェック.
    {
        if (PathIsDirectoryW(findPath.c_str()) != FALSE)
        {
            return true;
        }
    }
    // インスタンス生成.
    auto pTexture = MAKE_UNIQUE_LOG<Texture>();
    if (pTexture == nullptr)
    {
        ELOG("Error : Out of memory.");
        return false;
    }
    // 初期化.
    if (!pTexture->init(m_PDevice, m_PPool, findPath.c_str(), batch))
    {
        ELOG("Error : Material::SetTexture() Failed.");
        pTexture->term();
        return false;
    }
    m_PAssetManager->Add(path, addKey, std::move(pTexture));
    auto newTex = m_PAssetManager->getTexture(addKey);
    m_PTexture[addKey] = newTex;
    //m_Subset[index].TextureHandle[usage] = newTex->GetHandleGPU();
    m_Subset[index].pTextures[usage] = m_PTexture[addKey];

    return true;
}

bool Material::setTexture(
    size_t index, TEXTURE_USAGE usage, 
    const std::wstring& path, 
    const std::string& registKey, 
    DirectX::ResourceUploadBatch& batch, 
    const char* data, int size)
{
    // 範囲内であるかチェック.
    if (index >= getCount())
    {
        return false;
    }
    // 既に登録済みかチェック.
    if (m_PTexture.find(registKey) != m_PTexture.end())
    {
        m_Subset[index].pTextures[usage] = m_PTexture[registKey];
        return true;
    }

    // インスタンス生成.
    auto pTexture = MAKE_UNIQUE_LOG<Texture>();
    if (pTexture == nullptr)
    {
        ELOG("Error : Out of memory.");
        return false;
    }

    if (data && size > 0) {
        if (!pTexture->init(m_PDevice, m_PPool, path.c_str(), batch, data, size))
        {
            ELOG("Error : Material::SetTexture() Failed.");
            pTexture->term();
            return false;
        }
    }
    else {
        // ファイルパスが存在するかチェックします.
        std::wstring findPath;
        if (!SearchFilePathW(path.c_str(), findPath))
        {
            return true;
        }
        // ファイル名であることをチェック.
        {
            if (PathIsDirectoryW(findPath.c_str()) != FALSE)
            {
                return true;
            }
        }

        // 初期化.
        if (!pTexture->init(m_PDevice, m_PPool, findPath.c_str(), batch))
        {
            ELOG("Error : Material::SetTexture() Failed.");
            pTexture->term();
            return false;
        }
    }
    m_PAssetManager->Add(path, registKey, std::move(pTexture));
    auto newTex = m_PAssetManager->getTexture(registKey);
    m_PTexture[registKey] = newTex;
    m_Subset[index].pTextures[usage] = m_PTexture[registKey];

    return true;
}

bool Material::setLoadedTexture(
    size_t                          index,
    TEXTURE_USAGE                   usage,
    const std::string& key
) 
{
    if (key.empty())
    {
        m_Subset[index].pTextures[usage] = nullptr;
        return true;
    }

    //  マテリアルに登録済みかチェック
    if (m_PTexture.find(key) != m_PTexture.end())
    {
        //m_Subset[index].TextureHandle[usage] = m_pTexture[key]->GetHandleGPU();
        m_Subset[index].pTextures[usage] = m_PTexture[key];
        return true;
    }
    auto text = m_PAssetManager->getTexture(key);
    if (!text)
    {
        ELOG("Error : AssetManager::GetTexture - failed");
        m_PTexture[key] = nullptr;
        m_Subset[index].pTextures[usage] = nullptr;
        return false;
    }
    m_PTexture[key] = text;
    m_Subset[index].pTextures[usage] = m_PTexture[key];

    return true;
}

//-----------------------------------------------------------------------------
//      定数バッファのポインタを取得します.
//-----------------------------------------------------------------------------
void* Material::getBufferPtr(size_t index) const
{
    if (index >= getCount())
    {
        return nullptr;
    }

    return m_Subset[index].pCostantBuffer->getPtr();
}

//-----------------------------------------------------------------------------
//      定数バッファのアドレスを取得します.
//-----------------------------------------------------------------------------
D3D12_GPU_VIRTUAL_ADDRESS Material::getBufferAddress(size_t index) const
{
    if (index >= getCount())
    {
        return D3D12_GPU_VIRTUAL_ADDRESS();
    }

    return m_Subset[index].pCostantBuffer->getAddress();
}

//-----------------------------------------------------------------------------
//      テクスチャハンドルを取得します.
//-----------------------------------------------------------------------------
D3D12_GPU_DESCRIPTOR_HANDLE Material::getTextureHandle(size_t index, TEXTURE_USAGE usage) const
{
    //auto result = m_Subset[index].TextureHandle[usage];

    //if (index >= GetCount() || !result.ptr)
    //{
    //    auto graphicContex = GraphicContext::instance;
    //    return graphicContex->dummyTexure()->GetHandleGPU();;
    //}

    if (m_Subset.size() <= index) {
        return GraphicContext::m_Instance->dummyTexure()->getHandleGPU();
    }

    if (m_Subset[index].pTextures[usage] != nullptr)
    {
        return m_Subset[index].pTextures[usage]->getHandleGPU(); 
    }
    return GraphicContext::m_Instance->dummyTexure()->getHandleGPU();

    /*return result;*/
}

//-----------------------------------------------------------------------------
//      マテリアル数を取得します.
//-----------------------------------------------------------------------------
size_t Material::getCount() const
{
    return m_Subset.size();
}



void Material::setTiling(DirectX::XMFLOAT2 tiling, size_t index)
{
    assert(tiling.x > 0 && tiling.y > 0 && L"Material::SetTiling");
    auto ptr = getBufferPtr<CB::MaterialBuffer>(index);
    ptr->TilingX = tiling.x;
    ptr->TilingY = tiling.y;
}

