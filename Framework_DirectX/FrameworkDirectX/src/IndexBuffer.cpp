#include "IndexBuffer.h"

IndexBuffer::IndexBuffer()
    : m_PIB(nullptr)
{
    memset(&m_View, 0, sizeof(m_View));
}


IndexBuffer::~IndexBuffer()
{
    term();
}


bool IndexBuffer::init(ID3D12Device* pDevice, size_t size, const uint32_t* pInitData)
{
    // ヒーププロパティ.
    D3D12_HEAP_PROPERTIES prop = {};
    prop.Type = D3D12_HEAP_TYPE_UPLOAD;
    prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    prop.CreationNodeMask = 1;
    prop.VisibleNodeMask = 1;

    // リソースの設定.
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment = 0;
    desc.Width = UINT64(size);
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    // リソースを生成.
    auto hr = pDevice->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_PIB.GetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }

    m_PIB->SetName(L"IndexBuffer");

    // インデックスバッファビューの設定.
    m_View.BufferLocation = m_PIB->GetGPUVirtualAddress();
    m_View.Format = DXGI_FORMAT_R32_UINT;
    m_View.SizeInBytes = UINT(size);

    // 初期化データがあれば，書き込んでおく.
    if (pInitData != nullptr)
    {
        void* ptr = map();
        if (ptr == nullptr)
        {
            return false;
        }

        memcpy(ptr, pInitData, size);

        m_PIB->Unmap(0, nullptr);
    }

    // 正常終了.
    return true;
}


void IndexBuffer::term()
{
    m_PIB.Reset();
    memset(&m_View, 0, sizeof(m_View));
}


uint32_t* IndexBuffer::map()
{
    uint32_t* ptr;
    auto hr = m_PIB->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
    if (FAILED(hr))
    {
        return nullptr;
    }

    return ptr;
}


void IndexBuffer::unmap()
{
    m_PIB->Unmap(0, nullptr);
}


D3D12_INDEX_BUFFER_VIEW IndexBuffer::getView() const
{
    return m_View;
}
