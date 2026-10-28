//#if defined(DEBUG)|| defined(_DEBUG)
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#endif//difine(DEBUG) || defined(_DEBUG)
//
//// ========================================================
//// Includes
//// ========================================================
//#include "App.h"
//
//// ========================================================
////	メインエントリーポイントです
//// ========================================================
//int wmain(int argc, wchar_t** argv, wchar_t** evnp)
//{
//#if defined(DEBUG)|| defined(_DEBUG)
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif//difined(DEBUG) || defined(_Debug)
//
//
//	//	アプリケーションの実行
//	App app(960, 540);
//	app.Run();
//
//	return 0;
//}