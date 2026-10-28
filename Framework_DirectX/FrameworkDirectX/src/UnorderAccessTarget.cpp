#include "UnorderAccessTarget.h"
#include "DescriptorPool.h"
#include "Logger.h"


UnorderAccessTarget::UnorderAccessTarget()
	: m_PTarget(nullptr)
	, m_PHandleUAV(nullptr)
	, m_PPoolUAV(nullptr)
	, m_ViewDesc{}
{
}


UnorderAccessTarget::~UnorderAccessTarget() {
	term();
}


bool UnorderAccessTarget::init(
	ID3D12Device* pDevice,
	DescriptorPool* pPoolUAV,
	uint32_t        width,
	uint32_t        height,
	IDXGISwapChain* pSwapChain)
{
	if (pDevice == nullptr || pPoolUAV == nullptr)
	{
		return false;
	}
	
	assert(m_PHandleUAV == nullptr && "UnoderAccessTarget::init() m_pHandleUAV != nullptr");
	assert(m_PPoolUAV == nullptr && "UnoderAccess::init() m_pPoolUAV != nullptr");
	m_PPoolUAV = pPoolUAV;
	m_PPoolUAV->addRef();

	m_PHandleUAV = m_PPoolUAV->allocHandle();
	if (m_PHandleUAV == nullptr)
	{
		ELOG("Error : UnorderAccessTarget::init - AllocHandle() Failed");
		return false;
	}

	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;

	DXGI_SWAP_CHAIN_DESC SCdesc;
	pSwapChain->GetDesc(&SCdesc);


	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = UINT64(width);
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = SCdesc.BufferDesc.Format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;


	auto hr = pDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(m_PTarget.GetAddressOf()));
	if (FAILED(hr))
	{
		ELOG("Error : UnorderAccessTarget::init - CreateCommitedResouce() Failed");
		return false;
	}
	m_PTarget->SetName(L"UATarget");

	pDevice->CreateUnorderedAccessView(
		m_PTarget.Get(),
		nullptr,
		nullptr,
		m_PHandleUAV->HandleCPU);

	return true;
}


void UnorderAccessTarget::term() {
	m_PTarget.Reset();

	if (m_PPoolUAV != nullptr && m_PHandleUAV != nullptr)
	{
		m_PPoolUAV->freeHandle(m_PHandleUAV);
		m_PHandleUAV = nullptr;
	}

	if (m_PPoolUAV != nullptr) {
		m_PPoolUAV->release();
		m_PPoolUAV = nullptr;
	}
}


DescriptorHandle* UnorderAccessTarget::getHandleUAV() const
{
	return m_PHandleUAV;
}


ID3D12Resource* UnorderAccessTarget::getResource()const {
	return m_PTarget.Get();
}


D3D12_RESOURCE_DESC UnorderAccessTarget::getDesc()const {
	if (!m_PTarget) {
		return D3D12_RESOURCE_DESC();
	}

	return m_PTarget->GetDesc();
}


D3D12_UNORDERED_ACCESS_VIEW_DESC UnorderAccessTarget::getViewDesc()const {
	return m_ViewDesc;
}

