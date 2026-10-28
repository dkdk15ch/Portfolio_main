#pragma once
#include "Component/ComponentBase.h"
#include <SimpleMath.h>
#include <vector>
#include <map>
#include <d3d12.h>
#include "PointerHelper.h"
#include "Interface/IAsset.h"
#include "Manager/PSOKey.h"

class GameBase;
class Material;
class Mesh;
class ConstantBuffer;
class Camera;
namespace CB {
	struct InstanceParamBuffer;
}
class RenderBufferBase;


class RenderCompBase : public ComponentBase , public IAsset
{
public:
	RenderCompBase(const RenderCompBase&) = delete;
	RenderCompBase& operator=(const RenderCompBase&) = delete;

	RenderCompBase(GameBase* owner, std::string psoName = "Simple");
	virtual ~RenderCompBase();

	// override ComponentBase
	CompType compType() override final { return CompType::RenderComp; }
	void init()override;
	void preRender(uint32_t frameIndex) override;
	void render(ID3D12GraphicsCommandList* pCmdList,
		const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer) override;

	void setMaterial(LogPtr<Material> material) {
		m_PMaterial = std::move(material);}
	void setTexture(std::wstring path, DirectX::XMFLOAT2 tiling = DirectX::XMFLOAT2(1.0f,1.0f));
#pragma region Mesh
	void setMesh(Mesh* mesh) {
		m_Mesh = mesh;}
	void setMeshFromPath(std::wstring path);
	void setMeshFromMemory(std::string path);
	void setCubeMesh();
	void setSphereMesh();
	void setPlaneMesh();
#pragma endregion

	void setVisible(bool value) { m_BIsVisible = value; }
	void setMovable(bool value);

	void changePSO(std::string newPso);

#pragma region Getter・Setter
	Material* material()const { return m_PMaterial.get(); }
	Mesh* mesh() const { return m_Mesh; }
	CB::InstanceParamBuffer* getInstanceParam(uint32_t frameIndex);
#pragma endregion

	LogPtr<RenderBufferBase> m_Buffer;

protected:
	virtual void setRootParam(ID3D12GraphicsCommandList* pCmdList,
		const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer);

	LogPtr<Material> m_PMaterial;
	Mesh* m_Mesh;


private:
	bool m_BIsVisible;
	bool m_BIsFirstCycle = true;
	std::string m_PsoName;
};


class RenderBufferBase {
	friend RenderCompBase;

public:
	RenderBufferBase() = default;
	virtual ~RenderBufferBase();

	virtual void initRenderBuffer(
		GameBase* owner,
		Camera& camera
	);
	void updateWorldViewProj(GameBase& owner, uint32_t  frameIndex, Camera& camera);
	// instance Data
	virtual void setWorldMatrix(uint32_t frameIndex,
		DirectX::SimpleMath::Matrix matrix);
	void setColor(DirectX::XMVECTOR color, int instanceIndex = 0);

	const std::vector<LogPtr<ConstantBuffer>>& transform()const { return m_TransformBuffer; }
	const std::vector<LogPtr<ConstantBuffer>>& instanceData()const { return m_InstanceData; }

protected:
	void updateViewProj(uint32_t  frameIndex, Camera& camera);
	virtual void updateWorld(GameBase& owner, uint32_t frameIndex);

	//	Buffer
	std::vector<LogPtr<ConstantBuffer>> m_TransformBuffer;
	std::vector<LogPtr<ConstantBuffer>> m_InstanceData;
	
	bool m_BIsMovable = true;
};
