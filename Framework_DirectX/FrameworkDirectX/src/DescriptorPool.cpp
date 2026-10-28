#include "DescriptorPool.h"
#include "Material.h"

#include <iostream>

DescriptorPool::DescriptorPool()
	:m_RefCount			(1)
	,m_Pool				()
	,m_PHeap			()
	,m_DescriptorSize	(0)
{ }


DescriptorPool::~DescriptorPool()
{
	m_Pool.term();
	m_PHeap.Reset();
	m_DescriptorSize = 0;
}


void DescriptorPool::addRef()
{
	m_RefCount++;
}


void DescriptorPool::release()
{
	m_RefCount--;

	if(m_RefCount==0)
	{
		delete this;
	}

}


DescriptorHandle* DescriptorPool::allocHandle()
{
	//	初期化関数
	auto func = [&](uint32_t index, DescriptorHandle* pHandle)
	{
		auto handleCPU = m_PHeap->GetCPUDescriptorHandleForHeapStart();
		handleCPU.ptr += m_DescriptorSize * index;

		D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = {};
		auto flags = m_PHeap->GetDesc().Flags;
		if (flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
		{
		handleGPU = m_PHeap->GetGPUDescriptorHandleForHeapStart();
		handleGPU.ptr += m_DescriptorSize * index;
		}
		pHandle->HandleCPU = handleCPU;
		pHandle->HandleGPU = handleGPU;
	};
	//	初期化関数を実行してからハンドルを返す
	return m_Pool.Alloc(func);
}


void DescriptorPool::freeHandle(DescriptorHandle*& pHandle)
{
	if (pHandle != nullptr)
	{
		//	ハンドルプールを元に戻す
		m_Pool.Free(pHandle);

		//	nullptrでクリアしておく
		pHandle = nullptr;
	}
}


bool DescriptorPool::create(
	ID3D12Device* pDevice, 
	const D3D12_DESCRIPTOR_HEAP_DESC* pDesc, 
	DescriptorPool** ppPool)
{
	//	引数チェック
	if (pDevice == nullptr || pDesc == nullptr || ppPool == nullptr)
	{
		return false;
	}

	//	インスタンスを生成
	auto m_Instance = new(std::nothrow)DescriptorPool();
	if(m_Instance==nullptr)
	{
		return false;
	}

	//	ディスクリプタヒープを生成
	auto hr = pDevice->CreateDescriptorHeap(
		pDesc,
		IID_PPV_ARGS(m_Instance->m_PHeap.GetAddressOf())
	);

	//	失敗したら開放処理を行って終了
	if (FAILED(hr))
	{
		m_Instance->release();
		return false;
	}

	m_Instance->m_PHeap->SetName(L"DescriptorHeap");

	//	プールを初期化
	if (!m_Instance->m_Pool.init(pDesc->NumDescriptors))
	{
		m_Instance->release();
		return false;
	}

	//	ディスクリプタの加算サイズを取得
	m_Instance->m_DescriptorSize = pDevice->GetDescriptorHandleIncrementSize(pDesc->Type);

	//	インスタンスを設定
	*ppPool = m_Instance;

	//	正常終了
	return true;
}

#pragma region getter

uint32_t DescriptorPool::getCount()	const {
	return m_RefCount;
}

uint32_t DescriptorPool::getAvailableHandleCount() const
{
	return m_Pool.getAvailableCount();
}

uint32_t DescriptorPool::getAllocatedHandleCount() const
{
	return m_Pool.getUsedCount();
}

uint32_t DescriptorPool::getHandleCount() const
{
	return m_Pool.getSize();
}

ID3D12DescriptorHeap* const DescriptorPool::getHeap() const
{
	return m_PHeap.Get();
}
#pragma endregion
