#define NOMINMAX
#include "Manager/DrawManager.h"
#include "Manager/AssetManager.h"
#include <windows.h>
#include <iostream>
#include "ConstantBuffer.h"
#include "DescriptorPool.h"
#include <CommonStates.h>
#include "FIleUtil.h"
#include "resMesh.h"
#include <SpriteBatch.h>
#include "DrawHelper/Image.h"
#include "GraphicContext.h"
#include <d3dx12.h>
#include <d3dcompiler.h>
#include "Component/Render/RootParam.h"
#include "Logger.h"
#include "SafeDelete.h"
#include "PointerHelper.h"
#include "StringHelper.h"
#include <format>


DrawManager* DrawManager::m_Instance = nullptr;


namespace {
    using namespace DirectX;
    using namespace DirectX::SimpleMath;
}


DrawManager::DrawManager(
    ID3D12Device* device, 
    DescriptorPool* pool):
    m_FilterType(FilterType::NONE),
    m_GraphicPSOBaseDesc{},
    m_Noise(nullptr)
{
	if (m_Instance) return;
	m_Instance = this;

    {    // ルートシグニチャの生成
        auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
        flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
        flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        // ディスクリプタレンジを設定.
        D3D12_DESCRIPTOR_RANGE range = {};
        range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        range.NumDescriptors = 19;
        range.BaseShaderRegister = 0;
        range.RegisterSpace = 0;
        range.OffsetInDescriptorsFromTableStart = 0;

        // ルートパラメータの設定.
        D3D12_ROOT_PARAMETER param[RootParam::COUNT] = {};
        auto p = &param[RootParam::GLOBAL_DATA];
            p->ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            p->Descriptor.ShaderRegister = 0;
            p->Descriptor.RegisterSpace = 0;
            p->ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        p = &param[RootParam::TRANSFORM];
            p->ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            p->Descriptor.ShaderRegister = 1;
            p->Descriptor.RegisterSpace = 0;
            p->ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        p = &param[RootParam::LIGHT];
            p->ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            p->Descriptor.ShaderRegister = 2;
            p->Descriptor.RegisterSpace = 0;
            p->ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        p = &param[RootParam::MATERIAL];
            p->ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            p->Descriptor.ShaderRegister = 3;
            p->Descriptor.RegisterSpace = 0;
            p->ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        p = &param[RootParam::INSTANCE_DATA];
            p->ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            p->Descriptor.ShaderRegister = 4;
            p->Descriptor.RegisterSpace = 0;
            p->ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        p = &param[RootParam::BONE_DATA];
            p->ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            p->Descriptor.ShaderRegister = 5;
            p->Descriptor.RegisterSpace = 0;
            p->ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        
            D3D12_DESCRIPTOR_RANGE rangeNoise = {};
            rangeNoise.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            rangeNoise.NumDescriptors = 1;
            rangeNoise.BaseShaderRegister = 20; // t20
            rangeNoise.RegisterSpace = 0;
            rangeNoise.OffsetInDescriptorsFromTableStart =  D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        p = &param[RootParam::NOISE_TEX];
            p->ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            p->DescriptorTable.NumDescriptorRanges = 1;
            p->DescriptorTable.pDescriptorRanges = &rangeNoise;
            p->ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        p = &param[RootParam::TABLE];
            p->ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            p->DescriptorTable.NumDescriptorRanges = 1;
            p->DescriptorTable.pDescriptorRanges = &range;
            p->ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        // サンプラーの設定.
        D3D12_STATIC_SAMPLER_DESC samplerDesc[7] = {};
        samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[0].MipLODBias = 0;
        samplerDesc[0].MaxAnisotropy = 0;
        samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        samplerDesc[0].MinLOD = 0.0f;
        samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
        samplerDesc[0].ShaderRegister = 0;
        samplerDesc[0].RegisterSpace = 0;
        samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        samplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc[1].MipLODBias = 0;
        samplerDesc[1].MaxAnisotropy = 0;
        samplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDesc[1].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        samplerDesc[1].MinLOD = 0.0f;
        samplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
        samplerDesc[1].ShaderRegister = 1;
        samplerDesc[1].RegisterSpace = 0;
        samplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        samplerDesc[6].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc[6].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc[6].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc[6].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        samplerDesc[6].MipLODBias = 0;
        samplerDesc[6].MaxAnisotropy = 0;
        samplerDesc[6].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDesc[6].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        samplerDesc[6].MinLOD = 0.0f;
        samplerDesc[6].MaxLOD = D3D12_FLOAT32_MAX;
        samplerDesc[6].ShaderRegister = 6;
        samplerDesc[6].RegisterSpace = 0;
        samplerDesc[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


        samplerDesc[2].Filter = D3D12_FILTER_ANISOTROPIC;
        samplerDesc[2].MinLOD = 0.0f;
        samplerDesc[2].MaxLOD = D3D12_FLOAT32_MAX;
        samplerDesc[2].MipLODBias = 0;
        samplerDesc[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        samplerDesc[2].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDesc[2].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        samplerDesc[2].MaxAnisotropy = 4;
        samplerDesc[2].RegisterSpace = 0;
        samplerDesc[2].ShaderRegister = 2;

        samplerDesc[3].Filter = D3D12_FILTER_ANISOTROPIC;
        samplerDesc[3].MinLOD = 0.0f;
        samplerDesc[3].MaxLOD = D3D12_FLOAT32_MAX;
        samplerDesc[3].MipLODBias = 0;
        samplerDesc[3].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[3].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[3].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        samplerDesc[3].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDesc[3].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        samplerDesc[3].MaxAnisotropy = 8;
        samplerDesc[3].RegisterSpace = 0;
        samplerDesc[3].ShaderRegister = 3;

        samplerDesc[4].Filter = D3D12_FILTER_ANISOTROPIC;
        samplerDesc[4].MinLOD = 0.0f;
        samplerDesc[4].MaxLOD = D3D12_FLOAT32_MAX;
        samplerDesc[4].MipLODBias = 0;
        samplerDesc[4].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[4].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[4].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        samplerDesc[4].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDesc[4].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        samplerDesc[4].MaxAnisotropy = 16;
        samplerDesc[4].RegisterSpace = 0;
        samplerDesc[4].ShaderRegister = 4;

        samplerDesc[5].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        samplerDesc[5].MinLOD = 0.0f;
        samplerDesc[5].MaxLOD = D3D12_FLOAT32_MAX;
        samplerDesc[5].MipLODBias = 0;
        samplerDesc[5].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        samplerDesc[5].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        samplerDesc[5].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        samplerDesc[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        samplerDesc[5].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        samplerDesc[5].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
        samplerDesc[5].MaxAnisotropy = 1;
        samplerDesc[5].RegisterSpace = 0;
        samplerDesc[5].ShaderRegister = 5;

        // ルートシグニチャの設定.
        D3D12_ROOT_SIGNATURE_DESC desc = {};
        desc.NumParameters = _countof(param);
        desc.NumStaticSamplers = 7;
        desc.pParameters = param;
        desc.pStaticSamplers = samplerDesc;
        desc.Flags = flag;

        ComPtr<ID3DBlob> pBlob;
        ComPtr<ID3DBlob> pErrorBlob;

        // シリアライズ
        auto hr = D3D12SerializeRootSignature(
            &desc,
            D3D_ROOT_SIGNATURE_VERSION_1,
            pBlob.GetAddressOf(),
            pErrorBlob.GetAddressOf());
        if (FAILED(hr))
        {
            ELOG("Error : Serialize Create Failed. retcode = 0x%x", hr);
            return;
        }

        // ルートシグニチャを生成.
        hr = device->CreateRootSignature(
            0,
            pBlob->GetBufferPointer(),
            pBlob->GetBufferSize(),
            IID_PPV_ARGS(m_PRootSig.GetAddressOf()));
        if (FAILED(hr))
        {
            ELOG("Error : Root Signature Create Failed. retcode = 0x%x", hr);
            return;
        }
        m_PRootSig->SetName(L"RootSignature");
    }

    {// コンピュートシェーダ用ルートシグネチャ
        D3D12_DESCRIPTOR_RANGE range0{}, range1{};
        range0.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        range0.NumDescriptors = 1;
        range0.RegisterSpace = 0;
        range0.BaseShaderRegister = 0;

        range1.NumDescriptors = 1;
        range1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        range1.RegisterSpace = 0;
        range1.BaseShaderRegister = 0;
        //  ルートパラメータ
        CD3DX12_ROOT_PARAMETER params[2];
        params[0].InitAsDescriptorTable(1, &range0);
        params[1].InitAsDescriptorTable(1, &range1);

        CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc{};
        // ルートシグニチャの設定.
        rootSigDesc.Init(_countof(params), params);
        auto sampler = DirectX::CommonStates::StaticLinearWrap(0, D3D12_SHADER_VISIBILITY_ALL);
        rootSigDesc.pStaticSamplers = &sampler;

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> errBlob;
        D3D12SerializeRootSignature(
            &rootSigDesc,
            D3D_ROOT_SIGNATURE_VERSION_1_0,
            &signature, &errBlob
        );

        auto hr = device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&m_PCsRootSig)
        );

    }

    auto graphicContext = GraphicContext::m_Instance;

    // グラフィックパイプラインステートの生成.
    {
        //  最初にベースの設定を作成    ////
        m_GraphicPSOBaseDesc.InputLayout = Vertex_Static::m_InputLayout;
        m_GraphicPSOBaseDesc.pRootSignature = m_PRootSig.Get();
        m_GraphicPSOBaseDesc.RasterizerState = DirectX::CommonStates::CullNone;
        m_GraphicPSOBaseDesc.BlendState = DirectX::CommonStates::Opaque;
        m_GraphicPSOBaseDesc.DepthStencilState = DirectX::CommonStates::DepthDefault;
        m_GraphicPSOBaseDesc.SampleMask = UINT_MAX;
        m_GraphicPSOBaseDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        m_GraphicPSOBaseDesc.NumRenderTargets = 1;
        m_GraphicPSOBaseDesc.RTVFormats[0] = graphicContext->colorTarget()->getViewDesc().Format;
        m_GraphicPSOBaseDesc.DSVFormat = graphicContext->depthTarget()->getViewDesc().Format;
        m_GraphicPSOBaseDesc.SampleDesc.Count = 1;
        m_GraphicPSOBaseDesc.SampleDesc.Quality = 0;
        /////////////////////////////////

//  グラフィックPSO
//  ランバート用
        if (!creaeteGraphicPSO(device, L"HLSL/LambertVS.cso", L"HLSL/LambertPS.cso", "Lambert")) {
            ELOG("Error : Failed create LambertPSO");
            return;
        }
        //  シンプル描画用
        if (!creaeteGraphicPSO(device, L"HLSL/SimpleTexVS.cso", L"HLSL/SimpleTexPS.cso", "Simple")) {
            ELOG("Error : Failed create SimplePSO");
            return;
        }
        //  シンプル描画用（alphaあり）
        auto alphaDesc = m_GraphicPSOBaseDesc;
        alphaDesc.BlendState = DirectX::CommonStates::NonPremultiplied;
        alphaDesc.DepthStencilState = DirectX::CommonStates::DepthRead;
        if (!creaeteGraphicPSO(device, L"HLSL/SimpleTexVS.cso", L"HLSL/SimpleAlphaPS.cso", "SimpleAlpha", &alphaDesc)) {
            ELOG("Error : Failed create SimplePSO");
            return;
        }

        //  Model用
        auto modelDesc = m_GraphicPSOBaseDesc;
        //modelDesc.RasterizerState = DirectX::CommonStates::Wireframe;
        modelDesc.InputLayout = Vertex_Skinned::m_InputLayout;
        if (!creaeteGraphicPSO(device, L"HLSL/ModelVS.cso",
            L"HLSL/ModelPS.cso", "Model", &modelDesc)) {
            ELOG("Error : Failed create ModelPSO");
            return;
        }

        //  ワイヤーフレーム用
        auto wireDesc = m_GraphicPSOBaseDesc;
        wireDesc.RasterizerState = DirectX::CommonStates::Wireframe;
        wireDesc.InputLayout = Vertex_Static::m_InputLayout;
        if (!creaeteGraphicPSO(device, L"HLSL/SimpleTexVS.cso", L"HLSL/SimpleTexPS.cso", "Wireframe", &wireDesc)) {
            ELOG("Error : Failed create wireframePSO");
            return;
        }

        //  コンピュートPSO
        //  セピアフィルター用
        if (!createComputePSO(device, L"HLSL/SepiaFilterCS.cso", "SepiaFilter")) {
            ELOG("Error : Failed create SepiaFilter");
            return;
        }
        //  輝度抽出
        if (!createComputePSO(device, L"HLSL/HighLuminanceFilter.cso", "HighLuminanceFilter")) {
            ELOG("Error : Failed create HightLuminanceFilter");
            return;
        }
    }

    //  SpriteFont
    DirectX::ResourceUploadBatch uploadBatch(device);
    uploadBatch.Begin();

    auto handle = pool->allocHandle();

    auto path = "res/Fonts/Stencil.spritefont";
    auto data = m_PAssetManager->Read(path);
    if (data.empty()) {
        std::wstring fontPath;
        if (!SearchFilePath(L"res/Fonts/Stencil.spritefont", fontPath)) {
            printf("Font file not found\n");
            return;
        }
        m_SpriteFont = std::make_unique<DirectX::SpriteFont>(
            device, uploadBatch, fontPath.c_str(),
            handle->HandleCPU, handle->HandleGPU
        );
    }
    else {
        m_SpriteFont = std::make_unique<DirectX::SpriteFont>(
            device, uploadBatch, reinterpret_cast<const uint8_t*>(data.data()), data.size(),
            handle->HandleCPU, handle->HandleGPU
        );
    }


    

    //  ダミーテクスチャの生成
    {
        auto texture = MAKE_UNIQUE_LOG<Texture>();
        if (!texture) {
            ELOG("Error : GraphicContext::init() failed create dummyTexture");
            return;
        }

        // ファイルパスが存在するかチェックします.
        auto path = "res/Util/White1x1.dds";
        auto pathW = StringToWString(path);
        auto data = m_PAssetManager->Read(path);
        std::wstring findPath;
        if (data.empty()) {
            findPath = GetFilePathW(pathW.c_str());
            if (findPath.empty()) {
                assert(false);
                return;
            }
        }

        if (!texture->init(device, pool,
            findPath.c_str(), uploadBatch, data.data(), data.size()))
        {
            ELOG("Error : GraphicContext::init() failed initialize dummyTexture");
            return;
        }
        m_PAssetManager->Add(pathW, "Dummy", std::move(texture));
        graphicContext->setDummy(m_PAssetManager->getTexture("Dummy"));
    }


    //  ノイズ用画像
    {
        auto texture = MAKE_UNIQUE_LOG<Texture>();
        if (!texture) {
            ELOG("Error : GraphicContext::init() failed create dummyTexture");
            return;
        }

        // ファイルパスが存在するかチェックします.
        auto path = "res/Util/Noise.dds";
        auto pathW = StringToWString(path);
        auto data = m_PAssetManager->Read(path);
        std::wstring findPath;
        if (data.empty()) {
            findPath = GetFilePathW(pathW.c_str());
            if (findPath.empty()) {
                assert(false);
                return;
            }
        }

        if (!texture->init(device, pool,
            findPath.c_str(), uploadBatch, data.data(), data.size()))
        {
            ELOG("Error : GraphicContext::init() failed initialize dummyTexture");
            return;
        }
        m_PAssetManager->Add(pathW, "Noise", std::move(texture));
        m_Noise = m_PAssetManager->getTexture("Noise");
    }

    auto uploadFinished = uploadBatch.End(graphicContext->queue());
    uploadFinished.wait();
}


DrawManager::~DrawManager()
{

}


void DrawManager::destory()
{
    m_PRootSig.Reset();
    m_PCsRootSig.Reset();
    m_SpriteFont.reset();
}


bool DrawManager::creaeteGraphicPSO(
    ID3D12Device* device,
    const std::wstring& vsPath, const std::wstring& psPath, 
    const std::string& PSOkey,
    D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc)
{
    HRESULT hr = 0;
    std::wstring VSPathW = vsPath;
    std::wstring PSPathW = psPath;
    std::string VSPath = WStringToString(VSPathW);
    std::string PSPath = WStringToString(PSPathW);
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    if (desc) {
        psoDesc = *desc;
    }
    else {
        psoDesc = m_GraphicPSOBaseDesc;
    }
    auto assetMgr = AssetManager::m_Instance;
    auto VSData = assetMgr->readHLSL(VSPath.c_str());
    auto PSData = assetMgr->readHLSL(PSPath.c_str());

    ComPtr<ID3DBlob> pVSBlob;
    ComPtr<ID3DBlob> pPSBlob;
    if (VSData.empty() || PSData.empty()) {
        // 頂点シェーダを検索
        if (!SearchFilePath(vsPath.c_str(), VSPathW))
        {
            ELOG("Error : %ls Not Found.", vsPath.c_str());
            return false;
        }
        // ピクセルシェーダを検索
        if (!SearchFilePath(psPath.c_str(), PSPathW))
        {
            ELOG("Error : %ls Not Found.", psPath.c_str());
            return false;
        }
        //  VS・PSの読込
        hr = D3DReadFileToBlob(VSPathW.c_str(), pVSBlob.GetAddressOf());
        if (FAILED(hr))
        {
            ELOG("Error : D3DReadFiledToBlob() Failed. path = %ls", vsPath.c_str());
            return false;
        }
        hr = D3DReadFileToBlob(PSPathW.c_str(), pPSBlob.GetAddressOf());
        if (FAILED(hr))
        {
            ELOG("Error : D3DReadFileToBlob() Failed. path = %ls", psPath.c_str());
            return false;
        }
        psoDesc.VS = { pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize() };
        psoDesc.PS = { pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize() };
    }
    else {
        psoDesc.VS = { VSData.data(), VSData.size()};
        psoDesc.PS = { PSData.data(), PSData.size()};
    }

    //  PSOの生成
    ComPtr<ID3D12PipelineState> PSO;
    hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(PSO.GetAddressOf()));

    if (FAILED(hr))
    {
        ELOG("Error : ID3D12Device::CreateGraphicsPipelineState() Failed. retcode = 0x%x", hr);
        return false;
    }
    std::wstring name = std::format(L"DrawManager::m_pPSOSet[{}]", StringToWString(PSOkey));
    PSO->SetName(name.c_str());
    //  リストに追加
    auto graphicContext = GraphicContext::m_Instance;    
    graphicContext->addGPSO(PSOkey, PSO);

    return true;
}

bool DrawManager::createComputePSO(ID3D12Device* device, const std::wstring& csPath, const std::string& PSOkey, D3D12_COMPUTE_PIPELINE_STATE_DESC* desc)
{
    HRESULT hr = 0;
    std::wstring CSPathW = csPath;
    std::string CSPath = WStringToString(CSPathW);
    D3D12_COMPUTE_PIPELINE_STATE_DESC cpDesc{};
    if (desc) {
        cpDesc = *desc;
    }
    else {
        cpDesc.pRootSignature = m_PCsRootSig.Get();
    }
    auto assetMgr = AssetManager::m_Instance;
    auto CSData = assetMgr->readHLSL(CSPath.c_str());

    ComPtr<ID3DBlob> pCSBlob;
    if (CSData.empty()) {
        if (!SearchFilePath(csPath.c_str(), CSPathW))
        {
            ELOG("Error : %ls Not Found.", CSPathW.c_str());
            return false;
        }
        hr = D3DReadFileToBlob(CSPathW.c_str(), pCSBlob.GetAddressOf());
        if (FAILED(hr))
        {
            ELOG("Error : D3DReadFileToBlob() Failed. path = %ls", CSPathW.c_str());
            return false;
        }
        cpDesc.CS = { pCSBlob->GetBufferPointer(),pCSBlob->GetBufferSize() };
    }
    else {
        cpDesc.CS = { CSData.data(), CSData.size() };
    }
    
    //  コンピュートパイプライン作成
    ComPtr<ID3D12PipelineState> PSO;
    hr = device->CreateComputePipelineState(&cpDesc, IID_PPV_ARGS(&PSO));

    if (FAILED(hr)) {
        ELOG("Error : ID3D12Device::CreatePipelineState() Failed. retcode = 0x%x", hr);
        return false;
    }
    std::wstring name = std::format(L"DrawManager::m_pPSOSet[{}]", StringToWString(PSOkey));
    PSO->SetName(name.c_str());
    //  リストに追加
    auto graphicContext = GraphicContext::m_Instance;
    graphicContext->addCPSO(PSOkey, PSO);

    return true;
}

