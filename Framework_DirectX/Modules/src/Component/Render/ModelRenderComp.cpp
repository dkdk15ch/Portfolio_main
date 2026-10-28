#include "Component/Render/ModelRenderComp.h"
#include "Component/Render/RootParam.h"
#include "Component/ComponentBase.h"
#include "ConstantBuffer.h"
#include "Material.h"
#include "Mesh.h"
#include "Component/Animator.h"
#include "Animation/Model.h"
#include "GraphicContext.h"
#include "GameObject/GameBase.h"
#include "StringHelper.h"
#include "Util/Loader.h"
#include "Manager/CameraManager.h"
#include "GameObject/Camera.h"
#include "Logger.h"


ModelRenderComp::ModelRenderComp(GameBase* owner, const std::string& modelKey)
    : RenderCompBase(owner, GPSOkey::MODEL)
{
    m_Animator = dynamic_cast<Animator*>(owner->getComponent(CompType::Animator));
        m_CacheKey = modelKey;
    if (!m_Animator) {
        return;
    }

    initFromKey();
    m_CacheKey.clear();
}

ModelRenderComp::ModelRenderComp(GameBase* owner, const std::wstring& path)
    : RenderCompBase(owner, GPSOkey::MODEL)
{
    m_CachePath = path;
    m_Animator = dynamic_cast<Animator*>(owner->getComponent(CompType::Animator));
    if (!m_Animator) {
        return;
    }

    initFromPath();
    m_CachePath.clear();
}

void ModelRenderComp::start()
{
    if (m_Animator) { return; }
    
    if (!m_CacheKey.empty()) {
        initFromKey();
        m_CacheKey.clear();
        return;
    }
    if (!m_CachePath.empty()) {
        initFromPath();
        m_CachePath.clear();
        return;
    }

    assert(m_Animator);
}

void ModelRenderComp::render(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer)
{
    RenderCompBase::render(pCmdList, frameIndex, lightBuffer);

    //DebugComp->Render(pCmdList, frameIndex, lightBuffer);
}

void ModelRenderComp::postUpdate(uint32_t frameIndex, double deltaTime)
{
    RenderCompBase::postUpdate(frameIndex, deltaTime);
}

void ModelRenderComp::term()
{
    m_Animator->term();
}

void ModelRenderComp::setRootParam(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer)
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
    auto texHandle = m_PMaterial->getTextureHandle(id, TU_DIFFUSE);

    auto instanceDeta = instData[frameIndex]->getAddress();
    pCmdList->SetGraphicsRootConstantBufferView(
        RootParam::INSTANCE_DATA, instanceDeta);


    auto bone = m_Animator->boneMatrixBuffer(frameIndex);
    pCmdList->SetGraphicsRootConstantBufferView(
        RootParam::BONE_DATA, bone->getAddress()
    );

    pCmdList->SetGraphicsRootDescriptorTable(RootParam::TABLE, texHandle);
}

void ModelRenderComp::initFromPath()
{
    //  mesh
    auto key = Loader::model(m_CachePath);
    m_Animator->setModel(key);
    setMesh(m_Animator->mesh());
    //  Materialの設定
    auto graphicContext = GraphicContext::m_Instance;
    auto device = graphicContext->device();
    auto pool = graphicContext->poolRes();
    std::vector<ResMaterial>    resMaterial;
    auto nameW = StringToWString(owner().name());
    //  texture
    const auto& texPaths = m_Animator->targetModel()->m_TexturePaths;
    if (texPaths.empty()) {
        ELOG("Erorr : ModelRenderComp::initFromPath - not texture");
        return;
    }

    Loader::texture(
        texPaths[0].c_str(),
        m_PMaterial.get(),
        nameW
    );
    auto ptr = m_PMaterial.get()->getBufferPtr<CB::MaterialBuffer>(0);
    ptr->Diffuse = { 1.0f,1.0f,1.0f };
    ptr->Alpha = 1.0f;


}

void ModelRenderComp::initFromKey()
{
    m_Animator->setModel(m_CacheKey);
    setMesh(m_Animator->mesh());

    //  Materialの設定
    auto graphicContext = GraphicContext::m_Instance;
    auto device = graphicContext->device();
    auto pool = graphicContext->poolRes();
    std::vector<ResMaterial>    resMaterial;
    auto nameW = StringToWString(owner().name());

    const auto& texPaths = m_Animator->targetModel()->m_TexturePaths;

    Loader::texture(
        texPaths[0].c_str(),
        m_PMaterial.get(),
        nameW
    );
    auto ptr = m_PMaterial.get()->getBufferPtr<CB::MaterialBuffer>(0);
    ptr->Diffuse = { 1.0f,1.0f,1.0f };
    ptr->Alpha = 1.0f;
}


