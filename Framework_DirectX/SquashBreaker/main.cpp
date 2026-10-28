#if defined(DEBUG) || defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#include <cstdlib>
	#include <crtdbg.h>
	#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ );
#else
	#pragma comment(linker, "/entry:wmainCRTStartup")
#endif


#include "SquashBreaker.h"
#include "Converter/Converter.h"
#include "Global.h"
#include <dxgidebug.h>


int wmain(int argc, wchar_t** argv, wchar_t** envp)
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


#ifdef __MINGW32__
	if (FAILED(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED)))
		return 1;
#else
	if (FAILED(RoInitialize(RO_INIT_MULTITHREADED)))
		return 1;
#endif

	//Globalinit();

#if 0

	struct LoadDesc {
		const char* path;
		float scale;
	};

	LoadDesc desc[] = {
		/////////	StaticMesh	/////////////////////////////////////
		//{"res/Mesh/Player/BallCase3.fbx", 1.f},
		//{"res/Mesh/Player/Racket.fbx", 1.f},

		/////////	SkinnedMesh	///////////////////////////////////
		//{"res/Mesh/Player/Ch20_nonPBR.fbx",		1.f},
		//{"res/Mesh/Enemy/Bat/Bat.fbx",		1.f},
		//{"res/Mesh/Item/Crown1.fbx",		1.f},


		/////////	Animation	////////////////////////////////////
		//{"res/Animation/Enemy/Bat/Flying.fbx",		1.f},
		//{"res/Animation/Enemy/Bat/Death.fbx",		1.f},
		//{"res/Animation/Enemy/Bat/Attack.fbx",		1.f},
		
		//{"res/Animation/Enemy/Bat/BatTest.fbx",		1.f},
		//{"res/Animation/Enemy/Demon.fbx",		1.f},

		//{"res/Animation/Player/Swing3.fbx",			1.f},
		{"res/Animation/Player/GameOver.fbx",			1.f},
		
		//{"res/Animation/Player/Idle.fbx",			1.f},
		//{"res/Animation/Player/Run.fbx",			1.f},
		//{"res/Animation/Player/Smash.fbx",			1.f},
		//{"res/Animation/Player/Jump.fbx",			1.f},
		//{"res/Animation/Player/Damage.fbx",			1.f},
		//{"res/Animation/Player/HitResponse.fbx",	1.f}
	};

	Converter cv;
	int n = _countof(desc);
	int i;
	for (i = 0; i < n; ++i) {
		cv.fbxToJson(
			desc[i].path,
			"",
			desc[i].scale,
			desc[i].scale,
			-desc[i].scale,
			0, 1, 2
		);
	}
	
	return 0;
#endif

	SquashBreaker(1920, 1080).run();


#if defined(DEBUG) || defined(_DEBUG)
	{
		ComPtr<IDXGIDebug> pDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
		{
			OutputDebugStringW(L"\n=== DXGI Live Objects ===\n");
			pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
		}
	}
#endif

	GlobalTerm();

	return 0;
}