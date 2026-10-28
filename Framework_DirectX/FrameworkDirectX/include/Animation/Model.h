#pragma once
#include <string>
#include "Vertex.h"
#include <memory>
#include <unordered_map>
#include <Interface/IAsset.h>

class Animation;
class Bone;
class ModelMesh;
class ModelNode;
class Mesh;
struct ResMaterial;
struct ResMesh_Static;
struct ResMesh_Skinned;
struct aiMesh;
struct aiScene;
struct aiNode;
struct aiAnimation;

namespace Assimp { class Importer; }

class AssimpPack : public IAsset
{
public:

	AssimpPack();
	virtual ~AssimpPack();

	const aiScene* getScene()const { return scene; }
	void init(std::wstring path, const char* data = nullptr, size_t = 0);

	static void setbEnableBakingLoad(bool bEnable) { m_BEnableBakingLoad = bEnable; }
private:
	std::wstring name;
	std::wstring path;
	const aiScene* scene;
	std::unique_ptr<Assimp::Importer> importer;
	static bool m_BEnableBakingLoad;
public:
#ifdef _DEBUG
	static constexpr const char* m_ClassName = "AssimpPack";
#endif
};

class Model : public IAsset
{
public:
	virtual ~Model();
	//	ロード元からクローンするための演算子
	Model& operator=(const Model& other) {
		return *this;
	}

	LogPtr<Model> Clone();

	AssetType type() override { return AssetType::MODEL; }

	void init(const std::wstring& path, const char* data = nullptr, size_t = 0);
	void Destroy();

	void addAnimation(const std::wstring& path, bool bIsLoop = false, double timeScale = 1.0);
	void addAnimation(const std::wstring& path, char* data = nullptr, size_t size = 0, bool bIsLoop = false, double timeScale = 1.0);
	void addAnimations(const std::wstring& path);

	void exportBinary(const std::wstring& path, const std::wstring& subKey);
	void importBinary(const std::wstring& path, const std::wstring& subKey);

	void setName(const std::string& name) { m_Name = name; }
	std::string& name() { return m_Name; }

	std::vector<LogPtr<ModelMesh>> m_ModelMeshList;
	std::vector<LogPtr<ModelNode>> m_ModelNodeList;
	std::vector<LogPtr<Bone>> m_BoneList;
	std::vector<std::string> m_AnimationNameList;
	std::vector<Bone*> getBoneList();
	std::vector<LogPtr<ModelNode>> cloneModelNodeList();
	std::vector<LogPtr<Bone>> cloneBoneList();

	uint32_t ROOT_NODE_INDEX = 0;
	ModelNode* m_RootBoneNode;

	//	アクセス用マップ
	std::unordered_map<std::string, std::vector<ModelMesh*>> m_MeshListMap;
	std::unordered_map<std::string, ModelNode*> m_NodeMap;
	std::unordered_map<std::string, Bone*> m_BoneMap;
	std::unordered_map<std::string, Animation*> m_AnimationMap;
	ModelNode* findNode(const std::string& name);
	ModelNode* findNode(int index);
	std::vector<ModelMesh*> findMesh(const std::string& name);
	ModelMesh* findMeshByIndex(const int& index);
	Bone* findBone(const std::string& name);
	Bone* findBoneFromIndex(int index);
	uint32_t findNodeIndex(const ModelNode* target) const;
	Animation* findAnimation(const std::string& key)const;

	std::vector<std::wstring> m_TexturePaths;

#pragma region LoadFromFbx
	static void loadVertex_Skinned(
		aiMesh* assimp_mesh,
		ResMesh_Skinned& meshes,
		const DirectX::SimpleMath::Matrix& meshMatrix 
		);
	static void loadIndex(aiMesh* assimp_mesh, std::vector<uint32_t>& indexs);

	void loadBone(aiMesh* assimp_mesh,
		ResMesh_Skinned& resMeshe);
	void loadNode(aiNode* root, uint32_t count);

	std::string loadAnimation(
		const std::wstring& path,
		aiAnimation* aiAnim, 
		aiNode* root,
		bool bIsLoop = false,
		double timeScale = 1.0f
		);
	
	void loadRootBone(aiScene* scene);
	void setNodeData();

	std::vector<ModelNode*> getNodeList() {
		std::vector<ModelNode*> rawPointers;
		rawPointers.reserve(m_ModelNodeList.size());

		for (const auto& node : m_ModelNodeList) {
			rawPointers.push_back(node.get());
		}
		return rawPointers;
	}
#pragma endregion

#pragma region LoadFromJson
	void loadVertex_Skinned(const std::wstring& filepath);

	static void loadIndex(const std::wstring& filepath, std::vector<uint32_t>& indexs);
	
	void loadBone(const std::wstring& filepath,
		ResMesh_Skinned& resMeshe,
		std::vector<Vertex_Skinned>& vertexs);
	void loadNode(const std::wstring& filepath);

	std::string loadAnimation(
		const std::wstring& path,
		bool bIsLoop = false,
		double timeScale = 1.0f
	);


#pragma endregion


private:
	std::string m_Name;
	std::wstring m_Path;
	int allocBoneID() { return m_BoneAllocator++; }
	void addBone(LogPtr<Bone> bone);
	uint32_t countNodes(aiNode* node);
	void createNodes(aiNode* node);
	void initNodes();
	void linkParent(aiNode* node);
	const aiNode* findArmatureNode(const aiNode* node);
	const aiNode* findParentNodeForMesh(const aiNode* node, uint32_t meshIndex);

	int m_BoneAllocator = 0;
	LogPtr<AssimpPack> m_AssimpPack;

	
public:
	static constexpr const char* m_ClassName = "Model";

	void setVertColor(ResMesh_Skinned& resMeshe);
};

