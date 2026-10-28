#include "Component/Render/InstancingRenderComp.h"
#include "GameObject/GameBase.h"
#include "Manager/CameraManager.h"
#include "GraphicContext.h"
#include "GameObject/Camera.h"
#include <utility>
#include "StringHelper.h"
#include "Logger.h"

InstancingRenderComp::InstancingRenderComp(GameBase* owner) : RenderCompBase(owner)

{
    assert(owner);
    m_Buffer = MAKE_UNIQUE_LOG<InstancingRenderBuffer>();
    auto camera = CameraManager::m_Instance->currentCamera();
    m_Buffer->initRenderBuffer(owner, camera);
}

InstancingRenderComp::~InstancingRenderComp()
{
}


InstancingRenderBuffer::InstancingRenderBuffer()
{
}

void InstancingRenderBuffer::updateWorld(GameBase& owner, uint32_t frameIndex)
{
    auto pInstance = m_InstanceData[frameIndex]->getPtr<CB::InstanceParamBuffer>();

    auto data = owner.instanceDataList();
    uint32_t instNum = owner.instanceNum();
    for (uint32_t i = 0; i < instNum; i++) {
        pInstance->DataArray[i].World =
            data[i].m_Transform->worldMatrix();
        pInstance->DataArray[i].Color = data[i].m_Color;
    }
}

void InstancingRenderBuffer::initRenderBuffer(
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
            pInstance->DataArray[i].Color =
                DirectX::Colors::White;
        }
        m_InstanceData[i] = std::move(pCB_Instance);
    }
}
