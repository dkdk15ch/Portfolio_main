#pragma once
#include <d3d12.h>
#include <atomic>
#include <ComPtr.h>
#include <Pool.h>

//	ディスクリプタハンドル
class DescriptorHandle
{
public:
	D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;

	bool hasCPU() const
	{ return HandleCPU.ptr != 0; }

	bool hasGPU() const
	{ return HandleGPU.ptr != 0; }
};

class DescriptorPool
{
public:
	/**
	 * @brief		ディスクリプタプールの作成
	 * @param[in]	pDevice		pDevice		デバイス
	 * @param[in]	pDesc 		pDesc		ディスクリプタヒープの構成設定
	 * @param[out]	ppPool 		ppPool		ディスクリプタヒープの格納先
	 * @return		true		生成成功
	 * @return		false		生成失敗
	 */
	static bool create(
		ID3D12Device* pDevice, 
		const D3D12_DESCRIPTOR_HEAP_DESC* pDesc,
		DescriptorPool** ppPool
		);

	void addRef();	 //	参照可能を増やす
	void release();	//	解放

	 //	参照カウントを取得
	uint32_t getCount()	const;

	/**
	 * @brief	ディスクリプタハンドルを割り当て
	 * @return	割り当てられたディスクリプタハンドルを返す
	 */
	DescriptorHandle* allocHandle();

	/**
	 * @brief			ディスクリプタハンドルを開放		
	 * @param pHandle	開放するハンドルへのポインタ
	 */
	void freeHandle(DescriptorHandle*& pHandle);

	//	利用可能なハンドル数を取得
	uint32_t getAvailableHandleCount()	const;

	//	割り当て済みのハンドル数を取得
	uint32_t getAllocatedHandleCount() const;

	//	ハンドル総数を取得
	uint32_t getHandleCount() const;

	//	ディスクリプタヒープを取得
	ID3D12DescriptorHeap* const getHeap() const;

private:
	std::atomic<uint32_t>			m_RefCount;			//	参照カウント
	Pool<DescriptorHandle>			m_Pool;				//	ディスクリプタハンドルプール
	ComPtr<ID3D12DescriptorHeap>	m_PHeap;			//	ディスクリプタヒープ
	uint32_t						m_DescriptorSize;	//	ディスクリプタサイズ

	DescriptorPool();
	~DescriptorPool();

	DescriptorPool	(const DescriptorPool&) = delete;	//	アクセス禁止
	void operator = (const DescriptorPool&) = delete;	//	アクセス禁止
};
