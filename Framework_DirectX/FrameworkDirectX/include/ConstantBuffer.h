#pragma once
#include <d3d12.h>
#include <ComPtr.h>
#include <vector>
#include <SimpleMath.h>
#include <string>

class DescriptorHandle;
class DescriptorPool;

namespace CB {//　定数バッファ用構造体
	struct Transform
	{
		DirectX::SimpleMath::Matrix   View;       //!< ビュー行列です.
		DirectX::SimpleMath::Matrix   Proj;       //!< 射影行列です.
	};

	struct LightBuffer
	{
		DirectX::SimpleMath::Vector4  LightPosition;     //!< ライト位置です.
		DirectX::SimpleMath::Color    LightColor;        //!< ライトカラーです.
	};

	struct MaterialBuffer
	{
		DirectX::SimpleMath::Vector3    Diffuse;        //!< 拡散反射率です.
		float Alpha;          //!< 透過度です.
		float TilingX;
		float TilingY;
	};

	struct InstanceParamBuffer {
		struct Data {
			DirectX::SimpleMath::Matrix World;
			DirectX::XMVECTOR Color;
		};
		Data DataArray[100];

		void setColor(DirectX::XMVECTOR color, int InstanceIndex = 0);
		void swap(uint32_t index1, uint32_t index2);
	};

	struct BoneParameter
	{
		DirectX::XMFLOAT4X4 bone[512];
	};

} // namespace


//-----------------------------------------------------------------------------
// 定数バッファ用クラス
//-----------------------------------------------------------------------------
class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	/**
	 * @brief				初期化処理
	 * @param[in] pDevice	デバイス
	 * @param[in] pPool		ディスクリプタプール
	 * @param size
	 * @return true			初期化成功
	 * @return false		初期化失敗
	 */
	bool init(
		ID3D12Device* pDevice,
		DescriptorPool* pPool,
		size_t			size,
		std::wstring& ownerName
	);

	//	終了処理
	void term();

	 //	GPU仮想アドレスを取得
	D3D12_GPU_VIRTUAL_ADDRESS getAddress() const;
	
	 //	CPUディスクリプタハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE getHandleCPU() const;
	
	 //	GPUディスクリプタハンドルを取得	
	D3D12_GPU_DESCRIPTOR_HANDLE getHandleGPU() const;

	 //	メモリマッピング済みポインタを取得
	void* getPtr() const;

	 //		メモリマッピング済みポインタを取得
	template<typename T>
	T* getPtr()	{ return reinterpret_cast<T*>(getPtr());}

private:
	ComPtr<ID3D12Resource>			m_PCB;			//	定数バッファ
	DescriptorHandle*				m_PHandle;		//	ディスクリプタハンドル
	DescriptorPool*					m_PPool;		//	ディスクリプタプール
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_Desc;			//	定数バッファビューの構成設定
	void*							m_PMappedPtr;	//	マップ済みポインタ

	ConstantBuffer	(const ConstantBuffer&) = delete;	//	アクセス禁止
	void operator = (const ConstantBuffer&) = delete;	//	アクセス禁止

public:
	static constexpr const char* m_ClassName = "ConstantBuffer";
};

