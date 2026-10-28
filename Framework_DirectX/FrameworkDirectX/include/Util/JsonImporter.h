#pragma once
#include <string>
#include <vector>
#include <unordered_map> 
#include "PointerHelper.h"
#include "Util/Json.hpp" 
class Vertex_Static;
class Vertex_Skinned;
struct ResMaterial;
struct ResMesh_Static;
struct ResMesh_Skinned;
class ModelNode;
class Bone;
class Model;
class Animation;

class JsonImporter {
public:
	~JsonImporter() = default;

	// ファイルからJSONを取得する関数（呼び出し側で1回実行）
	static nlohmann::json getJson(const std::wstring& filepath);

	// JSONデータを受け取る各パース関数
	static void staticMesh(const nlohmann::json& input, ResMesh_Static& out, ResMaterial& outMat);
	static void skinnedMesh(const nlohmann::json& input, 
		ResMesh_Skinned& outMesh, ResMaterial& outMat, Model& outModel);
	static void staticVertex(const nlohmann::json& input, ResMesh_Static& out);
	static void skinnedVertex(const nlohmann::json& input, ResMesh_Skinned& out);
	static void Material(const nlohmann::json& input, ResMaterial& out);
	static void Node(const nlohmann::json& input, Model& outModel);
	static void skelton(const nlohmann::json& input, Model& outModel);
	static void animation3D(const nlohmann::json& input, std::unordered_map<std::string, Animation*>& out);

private:
	JsonImporter() = default;
};