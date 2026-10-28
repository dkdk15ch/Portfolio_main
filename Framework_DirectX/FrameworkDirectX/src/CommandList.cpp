#include "CommandList.h"
#include <string>

CommandList::CommandList()
    : m_PCmdList(nullptr)
    , m_PAllocators()
    , m_Index(0)
{ }


CommandList::~CommandList(){
    term();
}


bool CommandList::init(
    ID3D12Device* pDevice, 
    D3D12_COMMAND_LIST_TYPE type, 
    uint32_t count)
{
    if (pDevice == nullptr || count == 0)
    {
        return false;
    }

    m_PAllocators.resize(count);

    for (auto i = 0u; i < count; ++i)
    {// アロケータの作成
        auto hr = pDevice->CreateCommandAllocator(
            type, IID_PPV_ARGS(m_PAllocators[i].GetAddressOf()));
        if (FAILED(hr))
        {
            return false;
        }

        std::wstring name = L"CommandAllocator_" + std::to_wstring(i);
        m_PAllocators[i]->SetName(name.c_str());
    }

    {// コマンドリストの作成
        auto hr = pDevice->CreateCommandList(
            1,
            type,
            m_PAllocators[0].Get(),
            nullptr,
            IID_PPV_ARGS(m_PCmdList.GetAddressOf()));
        if (FAILED(hr))
        {
            return false;
        }

        m_PCmdList->SetName(L"CommandList");

        m_PCmdList->Close();
    }

    m_Index = 0;
    return true;
}


void CommandList::term()
{
    m_PCmdList.Reset();

    for (size_t i = 0; i < m_PAllocators.size(); ++i)
    {
        m_PAllocators[i].Reset();
    }

    m_PAllocators.clear();
    m_PAllocators.shrink_to_fit();
}


ID3D12GraphicsCommandList* CommandList::reset()
{
    auto hr = m_PAllocators[m_Index]->Reset();
    if (FAILED(hr))
    {
        return nullptr;
    }

    hr = m_PCmdList->Reset(m_PAllocators[m_Index].Get(), nullptr);
    if (FAILED(hr))
    {
        return nullptr;
    }

    m_Index = (m_Index + 1) % uint32_t(m_PAllocators.size());
    return m_PCmdList.Get();
}

