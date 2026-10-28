#pragma once
#include <memory>
#include <string>
#include <SimpleMath.h>
#include "ComPtr.h"
#include "Interface/IAsset.h"

class DescriptorPool;
class Mesh;
class Image;
class Material;
struct TexInfo;
struct ID3D12Device;
struct ID3D12PipelineState;
class Model;

class Loader : public IAsset
{
public:
	static std::string model(const std::wstring& path);
	static std::string modelFromMemory(const std::string& path);

	struct animaDesc {
		std::wstring path = L"";
		char* data = nullptr;
		size_t size = 0;
		Model* model = nullptr;
		bool isLoop = false;
		double timeScale = 1.0;
	};
	static void animation(const std::wstring& path, Model* model,
		bool bIsLoop = false, double timeScale = 1.0);

	static void animation(animaDesc desc);
	/**
	 * @brief 
	 * @param filepath			パス
	 * @param (output)mesh		メッシュ
	 * @param (output)material	マテリアルバッファ
	 * @param Tiling			テクスチャのタイリング
	 * @param multColor			乗算カラー
	 * @return 
	 */


	static bool mesh(
		const wchar_t* filepath,
		Mesh& mesh,
		Material& material,
		DirectX::XMFLOAT2 Tiling = { 1.0f,1.0f }
	);

	static bool MESH(
		const wchar_t* filepath,
		Mesh& mesh,
		Material& material,
		DirectX::XMFLOAT2 Tiling = { 1.0f,1.0f }
	);

	struct MeshDesc {
		std::string key;
		DirectX::XMFLOAT2 Tiling = { 1.0f,1.0f };
	};
	static bool meshFromMemroy(
		const MeshDesc& desc,
		Mesh* mesh,
		Material* material
	);

	//	メッシュのみ
	static std::unique_ptr<Mesh> meshOnly(
		ID3D12Device* device, DescriptorPool* pool,
		const wchar_t* filepath);
	static std::unique_ptr<Mesh> meshOnlyFromMemory(
		ID3D12Device* device, DescriptorPool* pool,
		const char* filepath);



	//	バウンディング情報
	struct BoundingData {
		DirectX::SimpleMath::Vector3 minVertex;
		DirectX::SimpleMath::Vector3 maxVertex;
		float radius;
	};
	static BoundingData boudingSize(
		DescriptorPool* pool,
		const wchar_t* filepath, bool bIsCube);

	/**
	* @brief           後からtextureをロードして、セットする。PrimitiveMeshなどで使用
	*                  単一マテリアルのみ
	* @param filepath	パス
	* @param material	マテリアル
	* @param ownerName	オーナー名
	* @param Tiling		テクスチャのタイリング
	* @param bUseDummy	ダミーテクスチャ使用フラグ(true = 使用)
	* @return 
	*/
	static bool texture(
		const wchar_t* filepath,
		Material* material,
		std::wstring ownerName,
		DirectX::XMFLOAT2 Tiling = { 1.0f,1.0f },
		bool bUseDummy = false
	);

	static bool texture(
		const std::string& key,
		Material* material,
		std::wstring ownerName,
		const TexInfo& info,
		bool bUseDummy = false
	);
	


	static std::unique_ptr<Image> image(ID3D12Device* device, DescriptorPool* pool, const wchar_t* filepath, const char* data = nullptr, int size = 0);

private:
	static bool setLoadedTexture(
		const std::string& key,
		Material* material,
		std::wstring ownerName,
		DirectX::XMFLOAT2 Tiling = { 1.0f,1.0f }
	);

	static bool setLoadedTexture(
		const std::string& key,
		Material* material,
		std::wstring ownerName,
		const TexInfo& info
	);
};

