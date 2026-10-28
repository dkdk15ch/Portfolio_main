#include "Mesh.h"
#include <cassert>
#include "GraphicContext.h"

Mesh::Mesh()
    : m_MaterialId(UINT32_MAX)
    , m_IndexCount(0)
{ 
}


Mesh::~Mesh()
{
    term();
}


bool Mesh::init_Static(ID3D12Device* pDevice, const ResMesh_Static& resource)
{
    if (!m_VB.init<Vertex_Static>(
        pDevice, sizeof(Vertex_Static) * resource.Vertices.size(), resource.Vertices.data()))
    {
        return false;
    }

    if (!m_IB.init(
        pDevice, sizeof(uint32_t) * resource.Indices.size(), resource.Indices.data()))
    {
        return false;
    }

    m_MaterialId = resource.MaterialId;
    if (m_MaterialId == UINT32_MAX) {
        m_MaterialId = 0;
    }
    m_IndexCount = uint32_t(resource.Indices.size());

    if (m_IndexCount <= 0) {
        printf("=========\n");
    }

    return true;
}

bool Mesh::init_Skinned(ID3D12Device* pDevice, const ResMesh_Skinned& resource)
{
    if (!m_VB.init<Vertex_Skinned>(
        pDevice, sizeof(Vertex_Skinned) * resource.Vertices.size(), resource.Vertices.data()))
    {
        return false;
    }

    if (!m_IB.init(
        pDevice, sizeof(uint32_t) * resource.Indices.size(), resource.Indices.data()))
    {
        return false;
    }

    m_MaterialId = resource.MaterialId;
    m_IndexCount = uint32_t(resource.Indices.size());

    if (m_IndexCount <= 0) {
        printf("=========\n");
    }

    return true;
}


void Mesh::term()
{
    m_VB.term();
    m_IB.term();
    m_MaterialId = UINT32_MAX;
    m_IndexCount = 0;
}


void Mesh::draw(ID3D12GraphicsCommandList* pCmdList, UINT instanceCount)
{
    if (m_IndexCount <= 0) {
        printf("=========\n");
    }

    assert(m_IndexCount > 0 && "Mesh::draw() m_Index");
    auto VBV = m_VB.getView();
    auto IBV = m_IB.getView();
    assert(VBV.BufferLocation != 0 && "Mesh::draw() VBV");
    assert(IBV.BufferLocation != 0 && "Mesh::draw() IBV");


    pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pCmdList->IASetVertexBuffers(0, 1, &VBV);
    pCmdList->IASetIndexBuffer(&IBV);
    pCmdList->DrawIndexedInstanced(m_IndexCount, instanceCount, 0, 0, 0);
}


uint32_t Mesh::getMaterialId() const
{
    return m_MaterialId;
}
