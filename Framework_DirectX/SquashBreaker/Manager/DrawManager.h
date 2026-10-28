#pragma once
#include <vector>
#include "ComPtr.h"
#include <map>
#include <string>
#include <memory>
#include <SimpleMath.h>
#include <SpriteFont.h>
#include "Interface/IAsset.h"

class ConstantBuffer;
class Image;
class DescriptorPool;
class Mesh;
class Material;
class Texture;
struct ID3D12PipelineState;
struct ID3D12RootSignature;
struct ID3D12Device;
namespace DirectX {	namespace DX12	{class SpriteBatch;}}


class DrawManager : public IAsset
{
public:
	static DrawManager* m_Instance;

	DrawManager(ID3D12Device* device, DescriptorPool* pool);
	~DrawManager();

	void destory();

	//	ルートシグネチャ用 getter
	ID3D12RootSignature* rootSig()const { return m_PRootSig.Get(); }
	ID3D12RootSignature* csRootSig()const { return m_PCsRootSig.Get(); }

	//	フィルター用 enum/Setter/getter
	enum class FilterType {
		NONE,
		SEPIA,
		HIGHT_LUMINA,

		FILTER_NUM
	};
	FilterType filterType()const { return m_FilterType; }
	void setFilter(FilterType type) { m_FilterType = type; }

	//	スプライトフォント用Getter
	DirectX::SpriteFont& spriteFont()const { return *m_SpriteFont; }

	//	ノイズ用画像
	Texture* noise()const { return m_Noise; }
private:
	//	グラフィックPSOの生成
	bool creaeteGraphicPSO(
		ID3D12Device* device,
		const std::wstring& vsPath,
		const std::wstring& psPath,
		const std::string& PSOkey,
		D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc = nullptr);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_GraphicPSOBaseDesc;
	//	コンピュートPSOの生成
	bool createComputePSO(
		ID3D12Device* device,
		const std::wstring& csPath,
		const std::string& PSOkey,
		D3D12_COMPUTE_PIPELINE_STATE_DESC* desc = nullptr);

	ComPtr<ID3D12RootSignature>		m_PRootSig;
	ComPtr<ID3D12RootSignature>		m_PCsRootSig;

	std::unique_ptr<DirectX::SpriteFont>	m_SpriteFont;//	基本のフォント
	FilterType m_FilterType;

	Texture* m_Noise;

};

