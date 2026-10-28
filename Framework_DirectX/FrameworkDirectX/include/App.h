#pragma once
#include <Windows.h>
#include <cstdint>
#include <dxgi1_4.h>
#include <memory>
// ========================================================
// Linker
// ========================================================
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3dcompiler.lib")

class GraphicContext;

//  アプリケーションの管理クラス
class App
{
public:
    static App* m_Instance;
    static const uint32_t m_FrameCount = 2;   // フレームバッファ数です.

    App(uint32_t width, uint32_t height);
    virtual ~App();
    
    HINSTANCE hInst()   const   { return m_HInst; }
    HWND hWnd()         const   { return m_HWnd; }
    uint32_t width()    const   { return m_Width; }
    uint32_t height()   const   { return m_Height; }
    
    //! @brief      アプリケーションを実行します.
    void run();

protected:
    HINSTANCE   m_HInst;        // インスタンスハンドルです.
    HWND        m_HWnd;         // ウィンドウハンドルです.
    uint32_t    m_Width;        // ウィンドウの横幅です.
    uint32_t    m_Height;       // ウィンドウの縦幅です.

    //  派生クラスの実装を呼ぶ
    virtual bool onInit()  { return true; }
    virtual void onTerm() { /* DO_NOTHING */ }
    virtual void onRender()  { /* DO_NOTHING */ }
    virtual void update() { /* DO_NOTHING */ }
    virtual void onMsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    { /* DO_NOTHING */    }

private:
    std::unique_ptr<GraphicContext> m_GraphicContext;

    bool initApp();
    void termApp();
    bool initWnd();
    void termWnd();
    void mainLoop();

    static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
};
