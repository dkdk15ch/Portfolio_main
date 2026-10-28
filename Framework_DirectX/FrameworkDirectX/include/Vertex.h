#pragma once
#include <SimpleMath.h>
#include <array>
#include <d3d12.h>

enum class VertexType {
	Vertex_Skinned,
	Vertex_Static,
};

class Vertex_Static
{
public:
    Vertex_Static() = default;
    Vertex_Static(
        DirectX::XMFLOAT3 const& position,
        DirectX::XMFLOAT3 const& normal,
        DirectX::XMFLOAT2 const& texcoord,
        DirectX::XMFLOAT3 const& tangent)
        : m_Position(position)
        , m_Normal(normal)
        , m_TexCoord(texcoord)
        , m_Tangent(tangent)
    { /* DO_NOTHING */
    }

    DirectX::XMFLOAT3   m_Position;
    DirectX::XMFLOAT3   m_Normal;
    DirectX::XMFLOAT2   m_TexCoord;
    DirectX::XMFLOAT3   m_Tangent;

    static const D3D12_INPUT_LAYOUT_DESC m_InputLayout;

private:
    static const int m_InputElementCount = 4;
    static const D3D12_INPUT_ELEMENT_DESC m_InputElements[m_InputElementCount];
};

class Vertex_Skinned {
public:
    Vertex_Skinned() = default;
    Vertex_Skinned(
        DirectX::XMFLOAT3 const& pos,
        DirectX::XMFLOAT3 const& normal,
        DirectX::XMFLOAT3 const& tangent,
        DirectX::XMFLOAT4 const& color,
        std::array<DirectX::XMFLOAT2, 2> texCoords,
        DirectX::XMUINT4 boneID,
        DirectX::XMFLOAT4 boneWeight)
        : m_Position(pos)
        , m_Normal(normal)
        , m_Tangent(tangent)
        , m_Color(color)
        , m_TexCoords(texCoords)
        , m_BoneID(boneID)
        , m_BoneWeight(boneWeight)
    {
    }

    DirectX::XMFLOAT3   m_Position;
    DirectX::XMFLOAT3   m_Normal;
    DirectX::XMFLOAT3   m_Tangent;
    DirectX::XMFLOAT4   m_Color;
    std::array<DirectX::XMFLOAT2, 2> m_TexCoords;   //  メイン + 1
    DirectX::XMUINT4 m_BoneID;
    DirectX::XMFLOAT4 m_BoneWeight;

    static const D3D12_INPUT_LAYOUT_DESC m_InputLayout;

private:
    static const int m_InputElementCount = 8;
    static const D3D12_INPUT_ELEMENT_DESC m_InputElements[m_InputElementCount];
};
