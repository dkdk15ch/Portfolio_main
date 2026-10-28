#include "DrawHelper/Image.h"
#include "Texture.h"
#include "FileUtil.h"
#include <cassert>
#include "windows.h"
#include "App.h"
#include <ResourceUploadBatch.h>
#include "GraphicContext.h"
#include "Logger.h"
#include "StringHelper.h"

Image::Image(
    ID3D12Device* pDevice
    ,DescriptorPool* pPool 
    ,DirectX::ResourceUploadBatch& batch
    ,const std::wstring& path		
    ,const char* data
    ,int size) 
    : m_Texture(nullptr), m_Size(0,0)
{
    std::string key = m_PAssetManager->getKeyFromPath(path, AssetType::TEXTURE);
    if (!key.empty()) {
        m_Texture = m_PAssetManager->getTexture(key);
        m_Size = m_Texture->getTextureSize();
        return;
    }
    //  インスタンス生成
    auto pTexture = MAKE_UNIQUE_LOG<Texture>();
    assert(pTexture && "Image::Image() pTexture == nullptr");
    //  初期化
    if (!pTexture->init(pDevice, pPool, path.c_str(), batch, data, size))
    {
        pTexture->term();
        pTexture = nullptr;
        return;
    }
    //  登録
    std::wstring addKeyW = GetFileNameW(path.c_str());
    std::string addKey = WStringToString(addKeyW);
    m_PAssetManager->Add(path, addKey, std::move(pTexture));
    auto newTex = m_PAssetManager->getTexture(addKey);

    m_Texture = newTex;
    m_Size =  m_Texture->getTextureSize();
    
}


Image::Image() :
    m_Texture(nullptr), m_Size(0, 0)
{
}


Image::~Image() {
    m_Texture = nullptr;
}


/**
 * @brief           通常描画       
 * @param position  描画左上座標
 * @param srcRect   元画像の切り取り範囲
 * @param color     MultColor
 */
void Image::draw(
    DirectX::DX12::SpriteBatch& spriteBatch,
    const DirectX::XMFLOAT2& position,
    const RECT* srcRect,
    DirectX::FXMVECTOR color)const
{

    //  テクスチャがなければダミーを使用
    auto graphicContext = GraphicContext::m_Instance;
    Texture* tex = m_Texture ? m_Texture : graphicContext->dummyTexure();
    D3D12_GPU_DESCRIPTOR_HANDLE srv = tex->getHandleGPU();
    if (srv.ptr == 0) return;

    if (!tex) {
        ELOG("Image::draw - no texture (both null)");
    }
    else {
        auto cpu = tex->getHandleCPU();
        auto gpu = tex->getHandleGPU();
        if (gpu.ptr == 0) ELOG("Image::draw - SRV GPU handle is 0");
    }

    spriteBatch.Draw(srv, m_Size, position, srcRect, color);
}


/**
* @brief                    引き伸ばして描画 
 * @param destinationRect   引き伸ばすRECT
 * @param color             MultColor
 */
void Image::drawStretchRect(
    DirectX::DX12::SpriteBatch& spriteBatch,
    const RECT& destinationRect, 
    const RECT* sourceRect,
    DirectX::FXMVECTOR color,
    float rotation,
    DirectX::XMFLOAT2 origin
    ) const
{
    //  テクスチャがなければダミーを使用
    auto graphicContext = GraphicContext::m_Instance;
    const Texture* tex = m_Texture ? m_Texture : graphicContext->dummyTexure();
    D3D12_GPU_DESCRIPTOR_HANDLE srv = tex->getHandleGPU();
    if (srv.ptr == 0) return;

    if (!tex) {
        ELOG("Image::draw - no texture (both null)");
    }
    else {
        auto cpu = tex->getHandleCPU();
        auto gpu = tex->getHandleGPU();
        if (gpu.ptr == 0) ELOG("Image::draw - SRV GPU handle is 0");
    }
    spriteBatch.Draw(srv, m_Size, destinationRect, nullptr, color, rotation, origin);
}


/**
 * @brief   フルスクリーンで画面描画
 */
void Image::drawFullScreen(DirectX::DX12::SpriteBatch& spriteBatch)
{
    auto pos = DirectX::XMFLOAT2(0, 0);
    RECT rect{};
    rect.left = 0;
    rect.top = 0;
    auto app = App::m_Instance;
    rect.right = app->width();
    rect.bottom = app->height();
    auto color = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

    drawStretchRect(spriteBatch, rect, nullptr, color);
}

void Image::drawBackGround(DirectX::DX12::SpriteBatch& spriteBatch)
{
    auto pos = DirectX::XMFLOAT2(0, 0);
    RECT rect{};
    rect.left = 0;
    rect.top = 0;
    auto app = App::m_Instance;
    rect.right = app->width();
    rect.bottom = app->height();
    auto color = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

    //  テクスチャがなければダミーを使用
    auto graphicContext = GraphicContext::m_Instance;
    const Texture* tex = m_Texture ? m_Texture : graphicContext->dummyTexure();
    D3D12_GPU_DESCRIPTOR_HANDLE srv = tex->getHandleGPU();
    if (srv.ptr == 0) return;

    if (!tex) {
        ELOG("Image::draw - no texture (both null)");
    }
    else {
        auto cpu = tex->getHandleCPU();
        auto gpu = tex->getHandleGPU();
        if (gpu.ptr == 0) ELOG("Image::draw - SRV GPU handle is 0");
    }
    const float rot = 0.f;
    const auto ori = DirectX::XMFLOAT2(0.f, 0.f);
    const auto effect = DirectX::DX12::SpriteEffects_None;
    const float depth = 1.f;
    spriteBatch.Draw(srv, m_Size, rect, nullptr, color, rot, ori, effect, depth);
}


int Image::width() const { return m_Size.x; }

int Image::height()const { return m_Size.y; }
