#include "App.h"
#include "keyboard.h"
#include "GraphicContext.h"

App* App::m_Instance = nullptr;

namespace {
    const auto m_ClassName = TEXT("SampleWindowClass");    // ウィンドウクラス名

} // namespace


App::App(uint32_t width, uint32_t height)
    : m_HInst(nullptr)
    , m_HWnd(nullptr)
    , m_Width(width)
    , m_Height(height)
{ 
    assert(m_Instance == nullptr && "App::App()");
    m_Instance = this;

    m_GraphicContext = std::make_unique<GraphicContext>();
}


App::~App(){ }


void App::run()
{
    if (initApp())
    {
        mainLoop();
    }
    else {
        printf("App::run failed Init\n");
    }

    printf("App::initApp Term\n");
    termApp();
}


//  初期化処理
bool App::initApp()
{
    // ウィンドウの初期化.
    if (!initWnd())
    {
        printf("App::initApp failed Wnd\n");
        return false;
    }

    // Direct3D 12の初期化.
    if (!m_GraphicContext->init())
    {
        printf("App::initApp failed GraphicCont\n");
        return false;
    }

    // アプリケーション固有の初期化.
    if (!onInit())
    {
        printf("App::initApp failed OnInit\n");
        return false;
    }

    // ウィンドウを表示.
    ShowWindow(m_HWnd, SW_SHOWNORMAL);

    // ウィンドウを更新.
    UpdateWindow(m_HWnd);

    // ウィンドウにフォーカスを設定.
    SetFocus(m_HWnd);

    // 正常終了.
    return true;
}


void App::termApp()
{
    // アプリケーション固有の終了処理.
    onTerm();

    // Direct3D 12の終了処理.
    m_GraphicContext->term();
    m_GraphicContext.reset();
    // ウィンドウの終了処理.
    termWnd();

    m_Instance = nullptr;
}


//   ウィンドウの初期化処理
bool App::initWnd()
{
    // インスタンスハンドルを取得.
    auto hInst = GetModuleHandle(nullptr);
    if (hInst == nullptr)
    {
        return false;
    }

    // ウィンドウの設定.
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wndProc;
    wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);
    wc.hCursor = LoadCursor(hInst, IDC_ARROW);
    wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = m_ClassName;
    wc.hIconSm = LoadIcon(hInst, IDI_APPLICATION);

    // ウィンドウの登録.
    if (!RegisterClassEx(&wc))
    {
        return false;
    }

    // インスタンスハンドル設定.
    m_HInst = hInst;

    // ウィンドウのサイズを設定.
    RECT rc = {};
    rc.right = static_cast<LONG>(m_Width);
    rc.bottom = static_cast<LONG>(m_Height);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // ウィンドウサイズを調整.
    //auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    auto style = WS_POPUP | WS_VISIBLE;
    AdjustWindowRect(&rc, style, FALSE);

    // ウィンドウを生成.
    m_HWnd = CreateWindowEx(
        0,
        m_ClassName,
        TEXT("SQUASH BREAKER"),
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        //rc.right - rc.left,
        //rc.bottom - rc.top,
        screenWidth,
        screenHeight,        
        nullptr,
        nullptr,
        m_HInst,
        this);

    if (m_HWnd == nullptr)
    {
        return false;
    }

    // 正常終了.
    return true;
}


//  ウィンドウの終了処理
void App::termWnd()
{
    // ウィンドウの登録を解除.
    if (m_HInst != nullptr)
    {
        UnregisterClass(m_ClassName, m_HInst);
    }

    m_HInst = nullptr;
    m_HWnd = nullptr;
}


//-----------------------------------------------------------------------------
//      メインループです.
//-----------------------------------------------------------------------------
void App::mainLoop()
{
    MSG msg = {};

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) == TRUE)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            update();
            onRender();
        }
    }
}


//   ウィンドウプロシージャ
LRESULT CALLBACK App::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    auto m_Instance = reinterpret_cast<App*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    
    switch (message)
    {
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP: 
    {
        DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
        break;
    }
    case WM_CREATE:
    {
        auto pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
    }
    break;
    case WM_DESTROY:
    { PostQuitMessage(0); }
    break;
    
    default:
    { /* DO_NOTHING */ }
    break;
    }
    
    if (m_Instance != nullptr)
    {
        m_Instance->onMsgProc(hWnd, message, wParam, lParam);
    }
    
    return DefWindowProc(hWnd, message, wParam, lParam);

}


