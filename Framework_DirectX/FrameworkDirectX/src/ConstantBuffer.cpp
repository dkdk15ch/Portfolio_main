#include "ConstantBuffer.h"
#include "DescriptorPool.h"
#include "App.h"

ConstantBuffer::ConstantBuffer()
	: m_PCB(nullptr)
	, m_PHandle(nullptr)
	, m_PPool(nullptr)
	, m_PMappedPtr(nullptr)
{  }


ConstantBuffer::~ConstantBuffer()
{ term(); }


bool ConstantBuffer::init(
	ID3D12Device* pDevice, 
	DescriptorPool* pPool, 
	size_t size,
	std::wstring& ownerName)
{
	if (pDevice == nullptr || pPool == nullptr || size == 0)
	{ return false;	}

	assert(m_PPool == nullptr && "ConstantBuffer::init() m_pPool != nullptr");
	assert(m_PHandle == nullptr && "ConstantBuffer::init() m_pHandle != nullptr");

	m_PPool = pPool;
	m_PPool->addRef();
	
	size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
	UINT64 sizeAligned = (size + (align - 1)) & ~(align - 1);	//	alignに切り上げる

	//	ヒーププロパティ
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type					= D3D12_HEAP_TYPE_UPLOAD;
	prop.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference	= D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask		= 1;
	prop.VisibleNodeMask		= 1;
	
	//	リソースの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension			= D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment			= 0;
	desc.Width				= sizeAligned;
	desc.Height				= 1;
	desc.DepthOrArraySize	= 1;
	desc.MipLevels			= 1;
	desc.Format				= DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout				= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags				= D3D12_RESOURCE_FLAG_NONE;
	
	//	リソースを生成
	auto hr = pDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_PCB.GetAddressOf()));
	if (FAILED(hr))
	{ return false;	}

	auto name = L"ConstantBuffer-Owner: " + ownerName;
	m_PCB->SetName(name.c_str());

	//	メモリマッピングしておく
	hr = m_PCB->Map(0, nullptr, &m_PMappedPtr);
	if (FAILED(hr))
	{ return false;	}

	m_Desc.BufferLocation = m_PCB->GetGPUVirtualAddress();
	m_Desc.SizeInBytes = UINT(sizeAligned);
	m_PHandle = pPool->allocHandle();

	pDevice->CreateConstantBufferView(&m_Desc, m_PHandle->HandleCPU);

	//	正常終了
	return true;
}


void ConstantBuffer::term()
{
	//	メモリマッピングを解除して、定数バッファを解放
	if (m_PCB != nullptr)
	{
		m_PCB->Unmap(0, nullptr);
		m_PCB.Reset();
	}

	//	ビューを破棄
	if (m_PPool != nullptr)
	{
		m_PPool->freeHandle(m_PHandle);
		m_PHandle = nullptr;
	}

	//	ディスクリプタプールを解放
	if (m_PPool != nullptr)
	{
		m_PPool->release();
		m_PPool = nullptr;
	}

	m_PMappedPtr = nullptr;
}


#pragma region getter



D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::getAddress() const
{
	return m_Desc.BufferLocation;
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::getHandleCPU() const
{
	if (m_PHandle == nullptr)
	{
		return D3D12_CPU_DESCRIPTOR_HANDLE();
	}

	return m_PHandle->HandleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE ConstantBuffer::getHandleGPU() const
{
	if (m_PHandle == nullptr)
	{
		return D3D12_GPU_DESCRIPTOR_HANDLE();
	}
		
	return m_PHandle->HandleGPU;
}


void* ConstantBuffer::getPtr() const
{
	return m_PMappedPtr;
}
#pragma endregion


void CB::InstanceParamBuffer::setColor(DirectX::XMVECTOR color, int instanceIndex)
{
	this->DataArray[instanceIndex].Color = color;
}


void CB::InstanceParamBuffer::swap(uint32_t index1, uint32_t index2) {
	auto w = DataArray[index1];
	DataArray[index1] = DataArray[index2];
	DataArray[index2] = w;
}
