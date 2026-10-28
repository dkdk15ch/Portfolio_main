#include "SquashBreaker.h"
#include "FileUtil.h"
#include "Logger.h"
#include "CommonStates.h"
#include "DirectXHelpers.h"
#include "SimpleMath.h"
#include "GameObject/Player.h"
#include "DrawHelper/Image.h"
#include <windows.h>
#include <ResourceUploadBatch.h>
#include <format>
#include "Sequence/Title.h"
#include "GameObject/PrimitiveMesh.h"
#include "UI/HUD.h"
#include "Util/Font.h"
#include "Util/IJsonData.h"
#include "Manager/DrawManager.h"
#include "Manager/LightManager.h"
#include "Manager/AssetManager.h"
#include "Manager/CameraManager.h"
#include "Manager/ColliderManager.h"
#include "Manager/ObjectParamManager.h"
#include "Manager/RenderManager.h"
#include "Manager/PSOKey.h"
#include "Manager/LevelManager.h"
#include "Manager/SquashGameManager.h"
#include "Component/Render/RootParam.h"
#include "GraphicContext.h"
#include "InOutput/Pad.h"
#include "InOUtput/Audio.h"
#include "MyRand.h"
#include "GameObject/Param/CollisionLayerParam.h"


namespace {
    using namespace DirectX;
    using namespace DirectX::SimpleMath;
    struct Transform
    {
        Matrix   World;      // ワールド行列です.
        Matrix   View;       // ビュー行列です.
        Matrix   Proj;       // 射影行列です.
    };

    struct LightBuffer
    {
        Vector4  LightPosition;     // ライト位置です.
        Color    LightColor;        // ライトカラーです.
    };

    struct MaterialBuffer
    {
        Vector3    Diffuse;        // 拡散反射率です.
        float      Alpha;          // 透過度です.
    };

} // namespace


SquashBreaker::SquashBreaker(uint32_t width, uint32_t height) : App(width, height)
{
}


SquashBreaker::~SquashBreaker()
{

    
}


bool SquashBreaker::onInit()
{
    //  乱数用
    m_Rand = std::make_unique<MyRand>();
    //  アセット管理用マネージャクラス
    m_PAssetMgr = std::make_unique<AssetManager>();
    m_PAssetMgr->init();
    //  Jsonデータ
    m_PActorData = std::make_unique<IActorData>();
    m_PSceneData = std::make_unique<ISceneData>();
    m_PObjectParamMgr = std::make_unique<ObjectParamManager>();
    ISceneData::loadFromJson(L"dataTable/GameScene.json");
    IActorData::loadFromJson(L"dataTable/player.json");
    IActorData::loadFromJson(L"dataTable/PlayersObj.json");
    IActorData::loadFromJson(L"dataTable/Ball.json");
    IActorData::loadFromJson(L"dataTable/BlockWall.json");
    IActorData::loadFromJson(L"dataTable/Bat.json");
    IActorData::loadFromJson(L"dataTable/ClearItem.json");
    IActorData::loadFromJson(L"dataTable/CollisionLayer.json");
    std::string layerKey = "CollisionLayer";
    m_PObjectParamMgr->addParam(layerKey, layerKey, MAKE_UNIQUE_LOG<CollisionLayerParam>());
    //  DrawManagerの生成 -> RenderManagerの作成
    auto graphicContext = GraphicContext::m_Instance;
    auto device = graphicContext->device();
    auto pool = graphicContext->poolRes();
    auto queue = graphicContext->queue();
    m_PDrawMgr = std::make_unique<DrawManager>(device, pool);
    m_PRenderMgr = std::make_unique<RenderManager>();
    //  CameraManager
    m_PCameraMgr = std::make_unique<CameraManager>();
    //  Global CB
    m_PGlobalCB = std::make_unique<ConstantBuffer>();
    std::wstring name = L"GlobalCB";
    if (!m_PGlobalCB->init(device, pool, sizeof(CB::GlobalData), name)) {
        ELOG("Error : ConstantBuffer::init() Failed.");
        return false;
    }
    auto ptr = m_PGlobalCB->getPtr<CB::GlobalData>();
    ptr->windowSize = DirectX::XMFLOAT2(
        static_cast<float>(width()), static_cast<float>(height()));
    ptr->globalTime = 0.0f;
    ptr->deltaTime = 0.0f;
    //  フォント
    m_PFont = std::make_unique<Font>();
    //  LightManagerの生成
    m_PLightMgr = std::make_unique<LightManager>();
    // DirectXTK12 キーボード・ゲームパッド生成
    m_Pad = std::make_unique<Pad>();
    // DirectXTKAudio
    Audio::create();
    DirectX::ResourceUploadBatch uploadBatch(device);
    uploadBatch.Begin();
    //  プリミティブ形状の作成
    m_PGeoPrimitive = std::make_unique<PrimitiveMesh>(device, pool);
    //  HUD生成
    m_HUD = std::make_unique<HUD>();
    //  ColliderManager生成
    m_PColliderManager = std::make_unique<ColliderManager>();
    //  GameManager
    m_PGameManager = std::make_unique<SquashGameManager>();
    //  タイトル生成
    m_PLevelManager = std::make_unique<LevelManager>();
    auto seq = Sequence::Parent::instance();
    seq->create(new Sequence::Title());


    auto uploadFinished = uploadBatch.End(queue);
    uploadFinished.wait();

    return true;
}

void SquashBreaker::onTerm()
{
    Sequence::Parent::instance()->destroy();
    m_PLevelManager.reset();
    m_PGameManager.reset();
    m_PGeoPrimitive->term();
    m_PGeoPrimitive.reset();
    m_PFont.reset();
    m_PColliderManager->term();
    m_PColliderManager.reset();
    m_HUD->term();
    m_HUD.reset();
    m_PLightMgr->term();
    m_PLightMgr.reset();
    m_Pad.reset();
    Audio::destroy();
    m_PCameraMgr->destroy();
    m_PCameraMgr.reset();
    m_PDrawMgr->destory();
    m_PDrawMgr.reset();
    m_PObjectParamMgr.reset();
    IActorData::destroy();
    m_PActorData.reset();
    ISceneData::destroy();
    m_PSceneData.reset();
    m_PAssetMgr->destory();
    m_PAssetMgr.reset();
}

void SquashBreaker::onRender()
{
     auto graphicContext = GraphicContext::m_Instance;
    auto frameIndex = graphicContext->frameIndex();
    auto colorTarget = graphicContext->colorTarget();
    auto depthTarget = graphicContext->depthTarget();
    auto pool = graphicContext->poolRes();
    auto scissor = graphicContext->scissor();
    auto viewport = graphicContext->viewport();
    auto spriteBatch = graphicContext->spriteBatch();
    auto seq = Sequence::Parent::instance();

    // コマンドリストの記録を開始.
    auto pCmd = graphicContext->commandList().reset();
    if (!pCmd)
    {
        printf("Error CommandList.Reset()\n");
        return;
    }

    // 書き込み用リソースバリア設定.
    auto backBuffer = colorTarget[frameIndex].getResource();
    DirectX::TransitionResource(pCmd,
        backBuffer,
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);

    // ディスクリプタ取得.
    auto handleRTV = colorTarget[frameIndex].getHandleRTV();
    auto handleDSV = depthTarget->getHandleDSV();

    // レンダーターゲットを設定.
    pCmd->OMSetRenderTargets(1, &handleRTV->HandleCPU, FALSE, &handleDSV->HandleCPU);

    // クリアカラー.
    float clearColor[] = { 0.25f, 0.25f, 0.25f, 1.0f };

    // レンダーターゲットをクリア.
    pCmd->ClearRenderTargetView(handleRTV->HandleCPU, clearColor, 0, nullptr);

    // 深度ステンシルビューをクリア.
    pCmd->ClearDepthStencilView(handleDSV->HandleCPU, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


    ID3D12DescriptorHeap* const pHeaps[] = {
        pool->getHeap()
    };
    pCmd->SetDescriptorHeaps(1, pHeaps);
    pCmd->RSSetViewports(1, viewport);
    pCmd->RSSetScissorRects(1, scissor);
    
    {// 描画処理
        pCmd->SetGraphicsRootSignature(m_PDrawMgr->rootSig());

        //  通常描画
        {
            //  Sequence::Parent経由で呼ぶ
            auto cmdList = graphicContext->commandList().get();
            auto noise = m_PDrawMgr->noise();
            //  hlslのグローバルな値セット
            cmdList->SetGraphicsRootConstantBufferView(RootParam::GLOBAL_DATA, m_PGlobalCB->getAddress());
            cmdList->SetGraphicsRootDescriptorTable(RootParam::NOISE_TEX, noise->getHandleGPU());            
            seq->render(cmdList,frameIndex);
        }

        //  スプライト描画
        {
            if (!pHeaps[0]) {
                ELOG("Error: SRV Descriptor Heap is null");
                return;
            }

            //  描画準備
            pCmd->SetDescriptorHeaps(_countof(pHeaps), pHeaps);
            spriteBatch->SetViewport(*viewport);
            spriteBatch->Begin(pCmd);
            auto& spriteFont = m_PDrawMgr->spriteFont();
            //  Sequence::Parent経由で呼ぶ
            seq->renderSprite(*spriteBatch,spriteFont);
            
            //  HUD描画
            m_HUD->render(spriteFont);

            //  描画終了
            spriteBatch->End();
        }
    }

    auto filterType = m_PDrawMgr->filterType();
    if (filterType == DrawManager::FilterType::NONE) {
        // 表示用リソースバリア設定.
         DirectX::TransitionResource(pCmd,
            backBuffer,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);
    }
    else if(filterType == DrawManager::FilterType::SEPIA)
    {// セピア調フィルター
        pCmd->SetDescriptorHeaps(1, pHeaps);
        pCmd->SetComputeRootSignature(m_PDrawMgr->csRootSig());
        auto srv = graphicContext->getBackBuffer()->getHandleSRV();
        pCmd->SetComputeRootDescriptorTable(0, srv->HandleGPU);
        auto uav = graphicContext->unorderAccessTarget();
        pCmd->SetComputeRootDescriptorTable(1, uav->getHandleUAV()->HandleGPU);
        pCmd->SetPipelineState(graphicContext->getCPSO(CPSOkey::SEPIA_FILTER));
        pCmd->Dispatch((width() + 7) / 8, (height() + 7) / 8, 1);       
        auto backBuffer = graphicContext->getBackBuffer();
        graphicContext->copyRenderTarget(uav->getResource(), backBuffer->getResource());
    }
    else if (filterType == DrawManager::FilterType::HIGHT_LUMINA)
    {// 輝度抽出
        pCmd->SetDescriptorHeaps(1, pHeaps);
        pCmd->SetComputeRootSignature(m_PDrawMgr->csRootSig());
        auto srv = graphicContext->getBackBuffer()->getHandleSRV();
        pCmd->SetComputeRootDescriptorTable(0, srv->HandleGPU);
        auto uav = graphicContext->unorderAccessTarget();
        pCmd->SetComputeRootDescriptorTable(1, uav->getHandleUAV()->HandleGPU);
        pCmd->SetPipelineState(graphicContext->getCPSO(CPSOkey::LUMINA_FILTER));
        pCmd->Dispatch((width() + 7) / 8, (height() + 7) / 8, 1);
        auto backBuffer = graphicContext->getBackBuffer();
        graphicContext->copyRenderTarget(uav->getResource(), backBuffer->getResource());
    }

    

    // コマンドリストの記録を終了.
    auto hr = pCmd->Close();
    if (FAILED(hr))
    {
        return;
    }

    // コマンドリストを実行.
    ID3D12CommandList* pLists[] = { pCmd };
    graphicContext->queue()->ExecuteCommandLists(1, pLists);

    graphicContext->present(1);
}

void SquashBreaker::update()
{
    double deltaTime = 0.0f;
    m_Timer.tick([&]() {
        deltaTime = m_Timer.GetElapsedSeconds();
        });
    m_PGameManager->setDeltaTime(deltaTime);

    //  グローバル定数バッファ
    auto ptr = m_PGlobalCB->getPtr<CB::GlobalData>();
    ptr->windowSize = DirectX::XMFLOAT2(
        static_cast<float>(width()), static_cast<float>(height()));
    ptr->globalTime += static_cast<float>(deltaTime);
    ptr->deltaTime = static_cast<float>(deltaTime);
    //  Pad 
    auto graphicContex = GraphicContext::m_Instance;
    const uint32_t frameIndex = graphicContex->frameIndex();
    m_Pad->update();
    //  GameManager
    m_PGameManager->update(deltaTime);
    //  シーケンス
    auto seq = Sequence::Parent::instance();
    seq->update(frameIndex,deltaTime);
    //  DirectXTKAudio
    Audio::update();
    //  HUD
    m_HUD->update(deltaTime);
}

void SquashBreaker::onMsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //if (message == WM_SIZING) {
    //    auto ptr = m_pGlobalCB->getPtr<CB::GlobalData>();
    //    ptr->windowSize = DirectX::XMFLOAT2(
    //        static_cast<float>(width()), static_cast<float>(height()));
    //}

    if (message == WM_ACTIVATEAPP) {
        if (wParam) 
        {
            if (Pad::m_Instance && Pad::m_Instance->m_GamePad)
            {
                Pad::m_Instance->m_GamePad->Resume();
            }
        }
        else
        {
            if (Pad::m_Instance && Pad::m_Instance->m_GamePad)
            {
                Pad::m_Instance->m_GamePad->Suspend();
            }
        }
    }
}
