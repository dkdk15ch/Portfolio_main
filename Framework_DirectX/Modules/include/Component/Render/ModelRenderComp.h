#pragma once
#include "Component/Render/RenderCompBase.h"
#include "Component/ComponentBase.h"

class Model;
class BoneBuffer;
class Animator;

class ModelRenderComp : public RenderCompBase{
public:
	ModelRenderComp(GameBase* owner, const std::string& modelKey);
	ModelRenderComp(GameBase* owner, const std::wstring& path);
	~ModelRenderComp() = default;

	void start() override final;
	void render(ID3D12GraphicsCommandList* pCmdList,
		const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer) override;

	void postUpdate(uint32_t frameIndex, double deltaTime)	override final;
	void term() override final;

protected:
	void setRootParam(ID3D12GraphicsCommandList* pCmdList,
		const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer)override final;

private:
	void initFromPath();
	void initFromKey();
	std::string m_CacheKey;
	std::wstring m_CachePath;

	Animator* m_Animator;

};

class BoneBuffer {
public:
	
private:
	LogPtr<ConstantBuffer> m_BoneMatrix;
};
