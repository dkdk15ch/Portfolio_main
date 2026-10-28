#include "Component/Render/RenderCompBase.h"
#include "Component/Render/RootParam.h"
#include "ConstantBuffer.h"
#include "GraphicContext.h"
#include "GameObject/GameBase.h"
#include "GameObject/PrimitiveMesh.h"
#include "Material.h"
#include "Mesh.h"
#include "GameObject/Camera.h"
#include "Manager/CameraManager.h"
#include "Manager/RenderManager.h"
#include <utility>
#include "StringHelper.h"
#include "Util/Loader.h"
#include "Logger.h"

RenderCompBase::RenderCompBase(GameBase* owner, std::string psoName)
    : ComponentBase(owner)
    ,m_PMaterial(MAKE_UNIQUE_LOG<Material>())
    , m_Mesh(nullptr)
    , m_BIsVisible(true)
    , m_PsoName(std::move(psoName))
    , m_Buffer(MAKE_UNIQUE_LOG<RenderBufferBase>())
{
    assert(owner);
    auto camera = CameraManager::m_Instance->currentCamera();
    m_Buffer->initRenderBuffer(owner, camera);

    std::wstring nameW = StringToWString(owner->name());
    Loader::texture(L"", m_PMaterial.get(), nameW,
        DirectX::XMFLOAT2(1.0f, 1.0f), true);

    auto mgr = RenderManager::m_Instance;
    mgr->add(m_PsoName, this);
}

RenderCompBase::~RenderCompBase()
{
    auto mgr = RenderManager::m_Instance;
    mgr->remove(m_PsoName, owner().name());

	m_PMaterial.reset();
    m_Buffer.reset();
}

void RenderCompBase::init()
{
    auto camera = CameraManager::m_Instance->currentCamera();
    m_Buffer->initRenderBuffer(&owner(), camera);
    m_BIsFirstCycle = true;
}

void RenderCompBase::preRender(uint32_t frameIndex)
{
    auto& camera = CameraManager::m_Instance->currentCamera();
    if (m_BIsFirstCycle) {// 初回のみ全バッファ更新
        uint32_t count = App::m_FrameCount;
        for (uint32_t i = 0; i < count; i++) {
            m_Buffer->updateWorldViewProj(owner(), i, camera);
        }
        m_BIsFirstCycle = false;
        return;
    }

    m_Buffer->updateWorldViewProj(owner(), frameIndex, camera);
}

void RenderCompBase::render(ID3D12GraphicsCommandList* pCmdList,
    const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer)
{
    if (!m_BIsVisible) { 
        return; 
    }
    int instanceNum = owner().instanceNum();
    if (instanceNum <= 0) { return; }

    setRootParam(pCmdList, frameIndex, lightBuffer);

    if (m_Mesh){
        m_Mesh->draw(pCmdList, instanceNum);
    }
    else{
        ELOG("Error : mesh nullptr");
    }
}

void RenderCompBase::setTexture(std::wstring path, DirectX::XMFLOAT2 tiling)
{
    std::wstring nameW = StringToWString(owner().name());
    Loader::texture(path.c_str(), m_PMaterial.get(), nameW , tiling);
}


CB::InstanceParamBuffer* RenderCompBase::getInstanceParam(uint32_t frameIndex)
{
    auto ptrInstance = 
        m_Buffer->instanceData()[frameIndex]->getPtr<CB::InstanceParamBuffer>();
    return ptrInstance;
}

void RenderCompBase::setRootParam(ID3D12GraphicsCommandList* pCmdList,
    const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer)
{
    auto& transform = m_Buffer->transform();
    auto& instData = m_Buffer->instanceData();

    //  各種ルートパラメータのセット
    pCmdList->SetGraphicsRootConstantBufferView(
        RootParam::TRANSFORM,
        transform[frameIndex]->getAddress());
    pCmdList->SetGraphicsRootConstantBufferView(
        RootParam::LIGHT, lightBuffer);
    auto id = m_Mesh->getMaterialId();
    pCmdList->SetGraphicsRootConstantBufferView(
        RootParam::MATERIAL, m_PMaterial->getBufferAddress(id));
    auto instanceDeta = instData[frameIndex]->getAddress();
    pCmdList->SetGraphicsRootConstantBufferView(
        RootParam::INSTANCE_DATA, instanceDeta);
    auto texHandle = m_PMaterial->getTextureHandle(id, TU_DIFFUSE);
    pCmdList->SetGraphicsRootDescriptorTable(RootParam::TABLE, texHandle);
}

void RenderCompBase::setMeshFromPath(std::wstring path) {
    std::string key = m_PAssetManager->getKeyFromPath(path, AssetType::MESH);
    if (key.empty()) {
        auto mesh = MAKE_UNIQUE_LOG<Mesh>();
        Loader::MESH(path.c_str(), *mesh, *m_PMaterial);
        std::wstring addKeyW = GetFileNameW(path.c_str());
        key = WStringToString(addKeyW);
        m_PAssetManager->Add(path, key, std::move(mesh));
        m_Mesh = m_PAssetManager->getMesh(key);
    }
    else {
        m_Mesh = m_PAssetManager->getMesh(key);
        auto nameW = StringToWString(owner().name());
        const auto& info = m_Mesh->texInfoList();

        bool result = Loader::texture(
            info.key,
            m_PMaterial.get(),
            nameW,
            info);
    }
}

void RenderCompBase::setMeshFromMemory(std::string path)
{
    std::wstring pathW = StringToWString(path);
    std::string key = m_PAssetManager->getKeyFromPath(pathW, AssetType::MESH);
    if (key.empty()) {
        auto mesh = MAKE_UNIQUE_LOG<Mesh>();
        Loader::MeshDesc desc;
        desc.key = path;
        Loader::meshFromMemroy(desc, mesh.get(), m_PMaterial.get());
        std::wstring addKeyW = GetFileNameW(pathW.c_str());
        key = WStringToString(addKeyW);
        m_PAssetManager->Add(pathW, key, std::move(mesh));
        m_Mesh = m_PAssetManager->getMesh(key);
    }
    else {
        m_Mesh = m_PAssetManager->getMesh(key);
        auto nameW = StringToWString(owner().name());
        const auto& info = m_Mesh->texInfoList();

        bool result = Loader::texture(
            info.key,
            m_PMaterial.get(),
            nameW,
            info);
    }
}

void RenderCompBase::setCubeMesh()
{
    auto priMesh = PrimitiveMesh::m_Instance;
    setMesh(priMesh->cubeMesh());
}

void RenderCompBase::setSphereMesh()
{
    auto priMesh = PrimitiveMesh::m_Instance;
    setMesh(priMesh->sphereMesh());
}

void RenderCompBase::setPlaneMesh()
{
    auto priMesh = PrimitiveMesh::m_Instance;
    setMesh(priMesh->planeMesh());
}

void RenderCompBase::setMovable(bool value)
{
    m_Buffer->m_BIsMovable = value;
}

void RenderCompBase::changePSO(std::string newPso)
{
    auto mgr = RenderManager::m_Instance;
    bool bCanContinue;
    bCanContinue = mgr->remove(newPso, owner().name());
    if (bCanContinue) {
        ELOG("Error : RenderComp::changePSO - failed remove ");
        return;
    }
    
    bCanContinue = mgr->add(newPso, this);
    if (bCanContinue) {
        ELOG("Error : RenderComp::changePSO - failed add ");
        return;
    }
    m_PsoName = std::move(newPso);
}

RenderBufferBase::~RenderBufferBase()
{
}

void RenderBufferBase::initRenderBuffer(
    GameBase* owner,
    Camera& camera)
{
    auto graphicContext = GraphicContext::m_Instance;
    std::wstring nameW = StringToWString(owner->name());

    auto FrameCount = App::m_FrameCount;
    m_TransformBuffer.resize(FrameCount);
    m_InstanceData.resize(FrameCount);

    for (auto i = 0u; i < FrameCount; ++i)
    {
        //  CB::Transform
        auto pCB_Transform = MAKE_UNIQUE_LOG<ConstantBuffer>();
        if (!pCB_Transform)
        {
            ELOG("Error : Out of memory.");
            return;
        }

        // 定数バッファ初期化.
        if (!pCB_Transform->init(
            graphicContext->device(),
            graphicContext->poolRes(),
            sizeof(CB::Transform) * FrameCount, nameW))
        {
            ELOG("Error : ConstantBuffer::init() Failed.");
            return;
        }

        auto pTransform = pCB_Transform->getPtr<CB::Transform>();
        pTransform->View = camera.getView();
        pTransform->Proj = camera.getProj();
        m_TransformBuffer[i] = std::move(pCB_Transform);

        //  CB::InstanceParam
        auto pCB_Instance = MAKE_UNIQUE_LOG<ConstantBuffer>();
        if (!pCB_Instance)
        {
            ELOG("Error : Out of memory.");
            return;
        }

        // 定数バッファ初期化.
        if (!pCB_Instance->init(
            graphicContext->device(),
            graphicContext->poolRes(),
            sizeof(CB::InstanceParamBuffer) * FrameCount, nameW))
        {
            ELOG("Error : ConstantBuffer::init() Failed.");
            return;
        }
        auto pInstance = pCB_Instance->getPtr<CB::InstanceParamBuffer>();

        // インスタンスデータを設定.
        for (uint32_t i = 0; i < owner->instanceNum(); i++) {
            pInstance->DataArray[i].World =
                DirectX::SimpleMath::Matrix::Identity;
            pInstance->DataArray[i].Color = { 1.0f,1.0f,1.0f,1.0f };
        }
        m_InstanceData[i] = std::move(pCB_Instance);
    }
}

/**
 * @brief   非インスタンシング描画時のBufferの更新
 */
void RenderBufferBase::updateWorldViewProj(
    GameBase& owner,
    uint32_t frameIndex, Camera& camera)
{
    updateViewProj(frameIndex, camera);
    if (!m_BIsMovable) return;
    updateWorld(owner, frameIndex);
}

void RenderBufferBase::setWorldMatrix(uint32_t frameIndex,
    DirectX::SimpleMath::Matrix matrix) {
    auto m_Instance = m_InstanceData[frameIndex]->getPtr<CB::InstanceParamBuffer>();
    m_Instance->DataArray[0].World = matrix;
}

void RenderBufferBase::setColor(DirectX::XMVECTOR color, int instanceIndex) {
    for (int i = 0; i < App::m_FrameCount; i++) {
        auto ptr = m_InstanceData[i]->getPtr<CB::InstanceParamBuffer>();
        ptr->setColor(color, instanceIndex);
    }
}

void RenderBufferBase::updateViewProj(uint32_t frameIndex, Camera& camera)
{
    auto pTransform = m_TransformBuffer[frameIndex]->getPtr<CB::Transform>();
    pTransform->View = camera.getView();
    pTransform->Proj = camera.getProj();
}

void RenderBufferBase::updateWorld(GameBase& owner, uint32_t frameIndex)
{
    const auto& t = owner.transform();
    auto pInstance = m_InstanceData[frameIndex]->getPtr<CB::InstanceParamBuffer>();
    pInstance->DataArray[0].World = t->worldMatrix();
    pInstance->DataArray[0].Color = owner.color();
}
