#pragma once
#include <App.h>
#include <Material.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include "AdditionalConstantBuffer.h"
#include "Sequence/Parent.h"
#include "Util/StepTimer.h"

class Player;
class Image;
class PrimitiveMesh;
class HUD;
class Font;
class IActorData;
class ISceneData;
class Pad;
class MyRand;


class SquashBreaker : public App
{
public:
    SquashBreaker(uint32_t width, uint32_t height);
    virtual ~SquashBreaker();

private:
    //  Manager
    std::unique_ptr<class DrawManager> m_PDrawMgr;
    std::unique_ptr<class LightManager> m_PLightMgr;
    std::unique_ptr<class AssetManager> m_PAssetMgr;
    std::unique_ptr<class CameraManager> m_PCameraMgr;
    std::unique_ptr<class ColliderManager> m_PColliderManager;
    std::unique_ptr<class ObjectParamManager> m_PObjectParamMgr;
    std::unique_ptr<class RenderManager> m_PRenderMgr;
    std::unique_ptr<class LevelManager> m_PLevelManager;
    std::unique_ptr<class SquashGameManager> m_PGameManager;
    //  InOutput
    std::unique_ptr<Pad> m_Pad;

    //	GeometoricPrimitive
    std::unique_ptr<PrimitiveMesh> m_PGeoPrimitive;

    std::unique_ptr<HUD> m_HUD;
    std::unique_ptr<Font> m_PFont;
    DX::StepTimer m_Timer;
    std::unique_ptr<MyRand> m_Rand;

    std::unique_ptr<IActorData> m_PActorData;
    std::unique_ptr<ISceneData> m_PSceneData;

    //  Global Constant Buffer
    std::unique_ptr<ConstantBuffer> m_PGlobalCB;

    bool onInit() override;
    void onTerm() override;
    void onRender() override;
    void update() override;
    void onMsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

};

