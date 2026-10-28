#include "Util/JsonImporter.h"
#include "Vertex.h"
#include "Animation/Bone.h"
#include "Animation/Animation.h"
#include "Animation/AnimationNode.h"
#include "Animation/AnimationKeyFrame.h"
#include "Animation/ModelMesh.h"
#include "Animation/ModelNode.h"
#include "Animation/Model.h"
#include "FileUtil.h"
#include <istream>
#include <iostream>

void JsonImporter::staticMesh(const nlohmann::json& input, ResMesh_Static& out, ResMaterial& outMat)
{
}

void JsonImporter::skinnedMesh(const nlohmann::json& input, ResMesh_Skinned& outMesh, ResMaterial& outMat, Model& outModel)
{


	skinnedVertex(input, outMesh);
	Material(input, outMat);
	Node(input, outModel);
	skelton(input, outModel);
}


void JsonImporter::staticVertex(const nlohmann::json& input, ResMesh_Static& out)
{
	if (input.empty()) { return; }
	const auto& root = input[0];
	if (!root.contains("Mesh")) { return; }

	const auto& meshJson = root["Mesh"][0];
	if (!meshJson.contains("Vertice")) { return; }
	out.name = meshJson["MeshName"].get<std::string>();
	out.Vertices.clear();
	out.Vertices.reserve(meshJson["Num_Vertice"].get<size_t>());
	//	頂点
	for (const auto& vJson : meshJson["Vertice"])
	{
		auto vData = vJson.get<std::vector<float>>();
		if (vData.size() < 8) { continue; }
		Vertex_Static v{};

		v.m_Position = { vData[0], vData[1], vData[2] };
		v.m_Normal = { vData[3], vData[4], vData[5] };
		v.m_TexCoord = { vData[6], vData[7] };

		out.Vertices.push_back(v);
	}
	//	インデックス
	if (!root.contains("Indices")) { return; }
	const auto& indicesJson = root["Indices"][0];
	out.Indices.clear();
	out.Indices.reserve(indicesJson.size() * 3);
	for (const auto& index : indicesJson) {
		auto iData = index.get<std::vector<uint32_t>>();
		if (iData.size() != 3) { continue; }
		out.Indices.push_back(iData[0]);
		out.Indices.push_back(iData[1]);
		out.Indices.push_back(iData[2]);
	}
}

void JsonImporter::skinnedVertex(const nlohmann::json& input, ResMesh_Skinned& out)
{
	if (input.empty()) { return; }
	const auto& root = input[0];
	if (!root.contains("Mesh")) { return; }

	const auto& meshJson = root["Mesh"][0];
	if (!meshJson.contains("Vertice")) { return; }
	out.name = meshJson["MeshName"].get<std::string>();
	out.Vertices.clear();
	out.Vertices.reserve(meshJson["Num_Vertice"].get<size_t>());
	//	頂点
	for (const auto& vJson : meshJson["Vertice"])
	{
		auto vData = vJson.get<std::vector<float>>();
		if (vData.size() < 16) { continue; }
		Vertex_Skinned v{};

		v.m_Position = { vData[0], vData[1], vData[2] };
		v.m_Normal = { vData[3], vData[4], vData[5] };
		v.m_TexCoords[0] = { vData[6], vData[7] };
		v.m_BoneID.x = static_cast<uint32_t>(vData[8]);
		v.m_BoneID.y = static_cast<uint32_t>(vData[9]);
		v.m_BoneID.z = static_cast<uint32_t>(vData[10]);
		v.m_BoneID.w = static_cast<uint32_t>(vData[11]);
		v.m_BoneWeight.x = vData[12];
		v.m_BoneWeight.y = vData[13];
		v.m_BoneWeight.z = vData[14];
		v.m_BoneWeight.w = vData[15];

		out.Vertices.push_back(v);
	}
	//	インデックス
	if (!meshJson.contains("Indices")) { return; }
	const auto& indicesJson = meshJson["Indices"];
	out.Indices.clear();
	out.Indices.reserve(indicesJson.size() * 3);
	for (const auto& index : indicesJson) {
		auto iData = index.get<std::vector<uint32_t>>();
		if (iData.size() != 3) { continue; }
		out.Indices.push_back(iData[0]);
		out.Indices.push_back(iData[1]);
		out.Indices.push_back(iData[2]);
	}
}

void JsonImporter::Material(const nlohmann::json& input, ResMaterial& out)
{
	if (input.empty()) { return; }

	const auto& root = input[0];

	const auto& meshJson = root["Mesh"][0];
	if (!meshJson.contains("MaterialName")) { return; }
	out.name = meshJson["MaterialName"].get<std::string>();
	// ★ ここで実際に認識されている型と中身をログに出力
	std::cout << "Type: " << meshJson["Materials"].type_name() << std::endl;
	std::cout << "Content: " << meshJson["Materials"].dump() << std::endl;

	auto m = meshJson["Materials"].get<std::vector<float>>();
	if (m.size() >= 8) {
		out.Diffuse.x = m[0];
		out.Diffuse.y = m[1];
		out.Diffuse.z = m[2];
		out.Specular.x = m[3];
		out.Specular.y = m[4];
		out.Specular.z = m[5];
		out.Alpha = m[6];
		out.Shininess = m[7];
	}
	out.DiffuseMap	=  StringToWString(meshJson.value<std::string>("Diffuse", ""));
	out.SpecularMap =  StringToWString(meshJson.value<std::string>("Specular", ""));
	out.NormalMap	=  StringToWString(meshJson.value<std::string>("NormalMap", ""));
}

void JsonImporter::Node(const nlohmann::json& input, Model& outModel)
{
	if (input.empty()) { return; }

	const auto& root = input[0];
	
	const auto& modelNode = root["ModelNode"];
	auto size = modelNode.size();
	auto& outNodes = outModel.m_ModelNodeList;
	outNodes.reserve(size);

	std::vector<int> parentTemp;
	parentTemp.reserve(size);
	std::vector<std::vector<int>> childrenTemp;
	childrenTemp.reserve(size);

	for (auto& node : modelNode) {
		auto n = MAKE_UNIQUE_LOG<ModelNode>();
		n->m_Name = node["Name"].get<std::string>();
		auto matrix = node["Matrix"].get<std::vector<float>>();
		n->m_LocalPosition.x = matrix[0];
		n->m_LocalPosition.y = matrix[1];
		n->m_LocalPosition.z = matrix[2];
		n->m_LocalRotation.x = matrix[3];
		n->m_LocalRotation.y = matrix[4];
		n->m_LocalRotation.z = matrix[5];
		n->m_LocalRotation.w = matrix[6];
		n->m_LocalScale.x = matrix[7];
		n->m_LocalScale.y = matrix[8];
		n->m_LocalScale.z = matrix[9];
		auto parent = node["Parent"].get<int>();
		parentTemp.push_back(parent);
		auto children = node.value<std::vector<int>>("Child", {-1});
		childrenTemp.push_back(children);
		auto meshs = node.value<std::vector<int>>("Mesh", {});
		if (!meshs.empty()) {
			n->m_MeshIndexList = meshs;
		}
		auto bones = node.value<std::vector<int>>("Bone", {});
		if (!bones.empty()) {
			n->m_BoneIndexList = bones;
		}
		if (!outModel.m_NodeMap.contains(n->m_Name)) {
			outModel.m_NodeMap[n->m_Name] = n.get();
		}

		outNodes.push_back(std::move(n));
	}

	for (size_t i = 0; i < size; ++i) {
		int pIdx = parentTemp[i];
		if (pIdx >= 0 && pIdx < static_cast<int>(size)) {
			outNodes[i]->m_Parent = outNodes[pIdx].get();
		}
		else {
			outNodes[i]->m_Parent = nullptr;
		}
		for (int cIdx : childrenTemp[i]) {
			if (cIdx >= 0 && cIdx < static_cast<int>(size)) {
				outNodes[i]->m_Children.push_back(outNodes[cIdx].get());
			}
		}
	} 
}

void JsonImporter::skelton(const nlohmann::json& input, Model& outModel)
{
	if (input.empty()) { return; }

	const auto& root = input[0];
	const auto& boneArray = root["Bone"];
	if (boneArray.size() < 0) { return; }

	auto& outBones = outModel.m_BoneList;
	outBones.reserve(boneArray.size());

	for (const auto& bone : boneArray) {
		auto b = MAKE_UNIQUE_LOG<Bone>();
		std::string name = bone["Name"].get<std::string>();
		b->m_Name = name ;
		b->m_Index = bone["ID"].get<std::uint32_t>();
		b->m_NodeIndex = bone["Node"].get<std::uint32_t>();
		if (b->m_Index == 0) {
			assert(b->m_NodeIndex < outModel.m_ModelNodeList.size());
			outModel.m_RootBoneNode = outModel.m_ModelNodeList[b->m_NodeIndex].get();
		}

		if (!outModel.m_BoneMap.contains(name)) {
			outModel.m_BoneMap[name] = b.get();
		}
		outBones.push_back(std::move(b));
	}

}

void JsonImporter::animation3D(const nlohmann::json& input, std::unordered_map<std::string, Animation*>& out)
{
	if (input.empty()) { return; }

	const auto& root = input[0];
	const auto& animation = root["Animation"];
	if (animation.size() < 0) { return; }

	auto anima = MAKE_UNIQUE_LOG<Animation>();
	anima->m_Name = animation["Name"].get<std::string>();
	anima->m_Duration = animation["Duration"].get<double>();
	anima->m_TicksPerSecond = animation["TickPerSec"].get<double>();
	
	if (!animation.contains("Node")) { return; }
	const auto& aniNodeList = animation["Node"];
	anima->m_NodeList.reserve(aniNodeList.size());

	for (auto& n : aniNodeList) {
		auto aniNode = MAKE_UNIQUE_LOG<AnimationNode>();
		aniNode->m_Name = n["Name"].get<std::string>();
		const auto& keys = n["Key"];
		auto size = keys.size();
		aniNode->m_KeyFrames.reserve(size);
		for (const auto& keyframe : keys) {
			if (keyframe.size() < 12) { continue; }

			AnimationKeyFrame k;
			k.m_Time = keyframe[0].get<double>();
			k.m_Tick = keyframe[1].get<int>();
			k.m_Position.x = keyframe[2].get<float>();
			k.m_Position.y = keyframe[3].get<float>();
			k.m_Position.z = keyframe[4].get<float>();
			k.m_Rotation.x = keyframe[5].get<float>();
			k.m_Rotation.y = keyframe[6].get<float>();
			k.m_Rotation.z = keyframe[7].get<float>();
			k.m_Rotation.w = keyframe[8].get<float>();
			k.m_Scale.x = keyframe[9].get<float>();
			k.m_Scale.y = keyframe[10].get<float>();
			k.m_Scale.z = keyframe[11].get<float>();
			aniNode->m_KeyFrames.push_back(k);
		}
		
		anima->m_NodeList.push_back(std::move(aniNode));
		std::string animName = anima->m_Name;
		out[animName] = anima.get();

		//	マネージャー登録
	}
}

nlohmann::json JsonImporter::getJson(const std::wstring& filepath)
{
	nlohmann::json input;
	//  ファイルチェック
	//std::wstring path;
	//if (!SearchFilePath(filepath.c_str(), path))
	//{
	//	ELOG("Error : IActorData::LoadFromJson - File Not Found.");
	//	return input;
	//}
	//std::ifstream ifs(path);

	//ifs >> input;
	//// 入力が配列かどうかチェック
	//if (!input.is_array() || input.size() == 0) {
	//	ELOG("Error : ISceneData::LoadFromJson - Input is not array or empty.");
	//	return input;
	//}

	return input;
}

 