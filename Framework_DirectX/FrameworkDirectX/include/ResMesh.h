#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <string>
#include <vector> 
#include <array>
#include "Vertex.h"

struct aiScene;

struct ResMaterial
{
    std::string name;
    DirectX::XMFLOAT3   Diffuse;        // 拡散反射成分です.
    DirectX::XMFLOAT3   Specular;       // 鏡面反射成分です.
    float               Alpha;          // 透過成分です.
    float               Shininess;      // 鏡面反射強度です.
    std::wstring        DiffuseMap;     // ディフューズマップファイルパスです.
    std::wstring        SpecularMap;    // スペキュラーマップファイルパスです.
    std::wstring        ShininessMap;   // シャイネスマップファイルパスです.
    std::wstring        NormalMap;      // 法線マップファイルパスです.
};


struct ResMesh_Static
{
    std::string name;
    std::vector<Vertex_Static>     Vertices;     // 頂点データです.
    std::vector<uint32_t>       Indices;      // 頂点インデックスです.
    uint32_t                    MaterialId;   // マテリアル番号です.
};

struct ResMesh_Skinned
{
    std::string name;
    std::vector<Vertex_Skinned>     Vertices;     // 頂点データです.
    std::vector<uint32_t>       Indices;      // 頂点インデックスです.
    uint32_t                    MaterialId;   // マテリアル番号です.
};

//-----------------------------------------------------------------------------
// @brief      メッシュをロードします.
//
// @param[in]      filename        ファイルパス.
// @param[out]     meshes          メッシュの格納先です.
// @param[out]     materials       マテリアルの格納先です.
// @retval true    ロードに成功.
// @retval false   ロードに失敗.
//-----------------------------------------------------------------------------
bool LoadStaticMesh(
    const wchar_t* filename,
    std::vector<ResMesh_Static>& meshes,
    std::vector<ResMaterial>& materials);
bool LoadStaticMeshFromMemroy(
    const char* data,
    size_t size,
    std::vector<ResMesh_Static>& meshes,
    std::vector<ResMaterial>& materials);


void loadMaterial(
    const aiScene* scene,
    std::vector<ResMaterial>& materials
);

