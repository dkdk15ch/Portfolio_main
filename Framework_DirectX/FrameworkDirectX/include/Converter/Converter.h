#pragma once
#include <vector>
#include <map>
#include <string>
#include <SimpleMath.h>
#include <unordered_map>

namespace fbxsdk {
	class FbxMesh;
	class FbxSurfaceMaterial;
	class FbxSkeleton;
	class FbxNode;
	class FbxScene;
	class FbxGeometryConverter;
	class FbxCluster;

	enum class Type {
		staticMesh,
		skinnedMesh,
		Animation,
	};
}

class Converter
{
public:
	/**
	 * @brief
	 * @param fbxFilename	変換元
	 * @param txtFilename	変換後
	 * @param fx,fy,fz		座標調整用
	 * @param idx0,1,2		z,y入れ替え用
	 */
	void fbxToJson(
		const char* fbxFilename,
		const char* txtFilename,
		float fx = 1, float fy = 1, float fz = 1,
		int idx0 = 0, int idx1 = 1, int idx2 = 2
	);


private:
	void Clear();

	void staticMeshToJson(fbxsdk::FbxGeometryConverter& converter, fbxsdk::FbxScene* scene, int idx0, int idx1, int idx2);
	void skinnedMeshToJson(fbxsdk::FbxGeometryConverter& converter, fbxsdk::FbxScene* scene, int idx0, int idx1, int idx2);
	void boneToJson(fbxsdk::FbxGeometryConverter& converter, fbxsdk::FbxScene* scene);
	void animationToJson(fbxsdk::FbxGeometryConverter& converter, fbxsdk::FbxScene* scene);
	bool hasMesh(fbxsdk::FbxScene* scene);
	bool hasBone(fbxsdk::FbxScene* scene);
	bool hasAnimation(fbxsdk::FbxScene* scene);

	void loadMaterial(fbxsdk::FbxSurfaceMaterial* material);
	std::string getTexturePath(fbxsdk::FbxSurfaceMaterial* material, const char* propertyName);
	struct MATERIAL {
		std::vector<float> materials;
		std::string diffuseName;
		std::string specularName;
		std::string shinessName;
		std::string normalName;
	};

#pragma region Mesh
	std::map<std::string, MATERIAL> m_MaterialMap;
	int m_Idx0;
	int m_Idx1;
	int	m_Idx2;
	void loadPositions(fbxsdk::FbxMesh* mesh, int k);
	void loadNormals(fbxsdk::FbxMesh* mesh, int k);
	void loadTangents(fbxsdk::FbxMesh* mesh, int k);
	void loadTexcoords(fbxsdk::FbxMesh* mesh, int k);
	void loadIndices(fbxsdk::FbxMesh* mesh, int k);
	void loadMaterialName(fbxsdk::FbxMesh* mesh, int k);
	int m_NumMeshParts;
	struct MeshPart {
		std::string				Name;
		std::vector<float>      Positions;
		std::vector<float>      Normals;
		std::vector<float>      Tangents;
		std::vector<float>      Texcoords;
		std::vector<uint16_t>	BoneIDs;
		std::vector<float>		BoneWeights;
		std::vector<uint16_t>   Indices;

		std::string             MaterialName;
		int MaterialID;
	};
	std::vector<MeshPart> m_MeshParts;
#pragma endregion

#pragma region Node Bone
	void loadBone(fbxsdk::FbxMesh* mesh, int k);
	void loadNode(fbxsdk::FbxNode* node, int l);
	int getBoneIndex(fbxsdk::FbxCluster* cluster);
	void addBone(std::string boneName);
	int allocBoneID() { return m_BoneAllocator++; }
	int m_BoneAllocator = 0;

	int m_NumNodeParts;
	
	struct NodePart {
		std::string Name;
		DirectX::SimpleMath::Vector3 Pos = { 0.0f, 0.0f, 0.0f };
		DirectX::SimpleMath::Quaternion Rot = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::SimpleMath::Vector3 Scale = { 1.0f, 1.0f, 1.0f };
		int ParentIndex = -1;
		std::vector<int> ChildrenIndices;
		std::vector<int> MeshIndex;
		std::vector<int> BoneIndex;
		fbxsdk::FbxNode* Node = nullptr; 
		int Index = -1;
	};
	int m_NextIdx = 0;
	std::vector<NodePart> m_NodeParts;
	std::unordered_map<std::string, int> m_NodeMap;

	int m_NumBoneParts;
	struct BonePart {
		std::string Name;
		uint16_t ID;
		int NodeIndex = -1;
	};
	std::vector<BonePart> m_BoneParts;
#pragma endregion

#pragma region Animation
	struct Key {
		double Time;
		double Tick;
		DirectX::SimpleMath::Vector3 Pos;
		DirectX::SimpleMath::Quaternion Rot;
		DirectX::SimpleMath::Vector3 Scale;
	};

	struct AnimaNode {
		std::string Name;
		std::vector<Key> Keys;
	};

	struct AnimaPart {
		std::string Name;
		std::vector<AnimaNode> Nodes;
		double TickPerSec;
		double Duration;
	};
	std::vector<AnimaPart> m_AnimaParts;
	int m_NumAnimaParts = 0;
#pragma endregion


	void createTextConbineVertex(
		const char* txtFilename, 
		float fx, float fy, float fz, fbxsdk::Type type);
	void createTextVertex_Static(
		std::ofstream& ofs,
		const char* txtFilename, 
		float fx, float fy, float fz);
	void createTextVertex_Skinned(
		std::ofstream& ofs,
		const char* txtFilename,
		float fx, float fy, float fz);
	void createTextMaterial(std::ofstream& ofs, const char* txtFilename);
	void createTextAnimation(
		std::ofstream& ofs,
		const char* txtFilename,
		float fx, float fy, float fz);
};

