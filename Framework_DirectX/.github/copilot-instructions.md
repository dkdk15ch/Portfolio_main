# Copilot / AI Agent Instructions — Framework_DirectX

This repository is a small DirectX 12 sample/framework. Keep instructions concise and actionable: point to where code lives, how to build/run, and the specific patterns used here.

1. Big picture
- Core app logic: `include/App.h` and `src/App.cpp` implement a tiny DirectX12 application class `App` (double-buffered FrameCount=2) that: creates device, swapchain, RTVs, root signature, PSO and renders a single triangle.
- Entry point: `src/main.cpp` constructs `App(960,540)` and calls `Run()`.
- Shaders: vertex shader `HLSL/SimpleVS.hlsl` (or `project/SimplePS.hlsl` for PS) are compiled to `.cso` files and loaded at runtime with `D3DReadFileToBlob(L"SimpleVS.cso", ...)`.

2. Build & run (Windows / Visual Studio)
- Preferred: open `project/Framework_DirectX.sln` in Visual Studio 2022 (VC++ toolset v143) and build Debug|x64.
- CLI (PowerShell):
  - msbuild: `msbuild .\project\Framework_DirectX.vcxproj /p:Configuration=Debug /p:Platform=x64`
  - After build, run the executable located at `project\x64\Debug\Framework_DirectX.exe` or `x64\Debug\Framework_DirectX.exe` depending on build output.
- Important: shaders must be next to the running EXE. The project normally produces `SimpleVS.cso` and `SimplePS.cso` in the build output; ensure those `.cso` files are in the same folder as the executable at runtime.

3. Project-specific patterns & conventions
- File layout: headers in `include/`, C++ sources in `src/`, HLSL in `HLSL/` or sometimes in `project/` (pixel shader). The VCXPROJ uses relative paths (see `<ClCompile>` and `<FxCompile>` entries).
- COM smart pointer alias: `template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;` (defined in `include/App.h`) — use this consistently for D3D12 objects.
- Constant buffers: `Transform` is aligned to 256 bytes (see `alignas(256) Transform`) and mapped per-frame into upload heaps. The code allocates one CB per frame (FrameCount) and maps them persistently.
- Root signature / CBV usage: root parameter 0 is a CBV (`register(b0)`) visible to the vertex shader. See root signature creation in `src/App.cpp` and `cbuffer Transform : register(b0)` in `HLSL/SimpleVS.hlsl`.
- Shader loading: the app loads precompiled shader objects (`.cso`) at runtime using `D3DReadFileToBlob`. If you edit shaders, ensure the project compiles them to `.cso` (FxCompile entries in the vcxproj) or run the offline compiler (fxc/dxc) to produce matching files.

4. Integration points & dependencies
- Native Windows + D3D12 required. Linker libs are explicitly listed in `include/App.h` via `#pragma comment(lib, ...)` for `d3d12.lib`, `dxgi.lib`, and `d3dcompiler.lib`.
- Tooling assumptions: Visual Studio (msbuild) and Windows SDK. VC toolset v143 is used in the project file.

5. Quick edits guidance (examples)
- Adding a new shader: place HLSL under `HLSL/`, add an `<FxCompile Include="..\HLSL\MyShader.hlsl">` entry in `project/Framework_DirectX.vcxproj` (follow the existing SimpleVS/PS examples), build, and copy the generated `.cso` to the exe folder.
- Changing window size: edit the call in `src/main.cpp` where `App app(960, 540);` is constructed.
- Modifying CB layout: update `include/App.h`'s `Transform` struct and the HLSL `cbuffer Transform` fields; keep the same packing/alignment (256 bytes) and ordering.

6. Debugging notes
- Runtime failures often come from missing `.cso` files or device initialization errors. Check build output folder for `SimpleVS.cso` / `SimplePS.cso` and run the exe from that folder to avoid path issues.
- Use Visual Studio debugger for breakpoints in `src/App.cpp`. Enable `D3D12` debug layer (not currently present in code) by creating the device with debug flags when troubleshooting GPU-level issues.

7. What not to change without caution
- Changing `FrameCount` or constant-buffer alignment requires coordinated changes in descriptor heap sizing and CPU/GPU pointer math (see `m_CBV`, `m_pHeapCBV` usage in `src/App.cpp`).
- Do not rename shader entrypoints or register bindings without updating both HLSL and PSO/root-signature setup.

8. Where to look for more context
- Runtime and resource code: `src/App.cpp` (longest, contains device, pipeline, CB setup).
- Interface and types: `include/App.h` (App class and common structs/typedefs).
- Project build definition: `project/Framework_DirectX.vcxproj` and `project/Framework_DirectX.sln`.

If anything here is unclear or missing (for example, if you use a different shader build step or a custom launch configuration), tell me which area to expand and I will iterate.
