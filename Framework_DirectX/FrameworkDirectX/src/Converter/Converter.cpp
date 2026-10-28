#include "Converter/Converter.h"
#include "fbxsdk.h"
#include <cassert>
#include <algorithm>
#include <fstream>
#include <FileUtil.h>
#include <format>
#include <iostream>

namespace {
	struct WeightData {
		uint16_t boneIDs[4] = { 0, 0, 0, 0 };
		float    weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	};

	float ClampEps(float v, float eps = 0.0001f) {
		if (std::abs(v - 1.0f) < eps) return 1.0f;
		if (std::abs(v - 0.0f) < eps) return 0.0f;
		if (std::abs(v - (-1.0f)) < eps) return -1.0f;
		return v;
	}
}

void Converter::fbxToJson(const char* fbxFilename, const char* txtFilename, float fx, float fy, float fz, int idx0, int idx1, int idx2)
{
	Clear();

	//FbxManager作成
	FbxManager* fbx_manager = fbxsdk::FbxManager::Create();
	assert(fbx_manager != nullptr);
	//FbxImporter作成
	FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "");
	assert(fbx_importer != nullptr);
	// fbxファイル名をインポーターにセット
	auto fbxPath = GetFilePath(fbxFilename);
	bool result = fbx_importer->Initialize(fbxPath.c_str());
	assert(result == true);
	//FbxScene作成
	FbxScene* fbx_scene = FbxScene::Create(fbx_manager, "");
	assert(fbx_scene != nullptr);
	// sceneにデータをインポート
	result = fbx_importer->Import(fbx_scene);
	assert(result == true);
	fbx_importer->Destroy();
	//コンバータを用意
	FbxGeometryConverter converter(fbx_manager);

	fbxsdk::Type type;
	if (!hasBone(fbx_scene)) {
		type = fbxsdk::Type::staticMesh;
	}
	else if (!hasMesh(fbx_scene)) {
		type = fbxsdk::Type::Animation;
	}
	else {
		type = fbxsdk::Type::skinnedMesh;
	}

	switch (type)
	{
	case fbxsdk::Type::staticMesh:
	{
		staticMeshToJson(converter, fbx_scene, idx0, idx1, idx2);
		break;
	}
	case fbxsdk::Type::skinnedMesh:
	{
		skinnedMeshToJson(converter, fbx_scene, idx0, idx1, idx2);
		break;
	}
	case fbxsdk::Type::Animation:
	{
		animationToJson(converter, fbx_scene);
		break;
	}

	}
	// fbxデータを読み終えたので破壊します。
	fbx_scene->Destroy();
	fbx_manager->Destroy();
	//	書き込み先ファイルないときに、作成
	auto txtPath = GetFilePath(txtFilename);
	if (txtPath == "") {
		auto path = GetDirectoryPath(fbxPath.c_str());
		auto name = RemoveExtension(fbxPath) + ".json";
		txtPath = path + name;
		std::ofstream ofs(txtPath);
	}

	createTextConbineVertex(txtPath.c_str(), fx, fy, fz, type);//pos,norm,texまとめて出力

	std::cout << "Success : " << txtPath << std::endl;
}

void Converter::Clear() {
	m_NumMeshParts = 0;
	m_MeshParts.clear();
	m_BoneAllocator = 0;
	m_NumNodeParts = 0;
	m_NextIdx = 0;
	m_NodeParts.clear();
	m_NodeMap.clear();
	m_NumBoneParts = 0;
	m_BoneParts.clear();
	m_NumAnimaParts = 0;
	m_AnimaParts.clear();

}

void Converter::staticMeshToJson(fbxsdk::FbxGeometryConverter& converter, fbxsdk::FbxScene* scene, int idx0, int idx1, int idx2)
{
	// メッシュに使われているマテリアル単位でメッシュを分割する
	converter.SplitMeshesPerMaterial(scene, true);
	// ポリゴンを三角形にする
	converter.Triangulate(scene, true);

	//①マテリアル読み込み
	// sceneの中の全マテリアルなので複数あることもある。使用していないマテリアルがあったりする。
	int numMaterials = scene->GetSrcObjectCount<FbxSurfaceMaterial>();

	for (int i = 0; i < numMaterials; i++) {
		loadMaterial(scene->GetSrcObject<FbxSurfaceMaterial>(i));
	}

	//②分割したメッシュをパーツごとに読み込む
	m_Idx0 = idx0;
	m_Idx1 = idx1;
	m_Idx2 = idx2;//xyzの入れ替えに使用する
	m_NumMeshParts = scene->GetSrcObjectCount<FbxMesh>();
	m_MeshParts.clear();
	m_MeshParts.resize(m_NumMeshParts);
	for (int j = 0; j < m_NumMeshParts; j++) {
		auto* mesh = scene->GetSrcObject<FbxMesh>(j);
		m_MeshParts[j].Name = mesh->GetName();
		loadPositions(mesh, j);
		loadNormals(mesh, j);
		loadTangents(mesh, j);
		loadTexcoords(mesh, j);
		loadIndices(mesh, j);
		loadMaterialName(mesh, j);
	}
}

void Converter::skinnedMeshToJson(fbxsdk::FbxGeometryConverter& converter, fbxsdk::FbxScene* scene, int idx0, int idx1, int idx2)
{
	m_NumNodeParts = scene->GetSrcObjectCount<FbxNode>();
	m_NumBoneParts = scene->GetSrcObjectCount<FbxSkeleton>();
	m_NodeParts.clear();
	m_NodeParts.reserve(m_NumNodeParts);
	m_BoneParts.clear();
	m_BoneParts.reserve(m_NumBoneParts);
	auto* root = scene->GetRootNode();
	loadNode(root, -1);
	// メッシュに使われているマテリアル単位でメッシュを分割する
	converter.SplitMeshesPerMaterial(scene, true);
	// ポリゴンを三角形にする
	converter.Triangulate(scene, true);
	//①マテリアル読み込み
	// sceneの中の全マテリアルなので複数あることもある。使用していないマテリアルがあったりする。
	int numMaterials = scene->GetSrcObjectCount<FbxSurfaceMaterial>();
	for (int i = 0; i < numMaterials; i++) {
		loadMaterial(scene->GetSrcObject<FbxSurfaceMaterial>(i));
	}
	//②分割したメッシュをパーツごとに読み込む
	m_Idx0 = idx0;
	m_Idx1 = idx1;
	m_Idx2 = idx2;//xyzの入れ替えに使用する
	m_NumMeshParts = scene->GetSrcObjectCount<FbxMesh>();
	m_MeshParts.clear();
	m_MeshParts.resize(m_NumMeshParts);
	for (int j = 0; j < m_NumMeshParts; j++) {
		auto* mesh = scene->GetSrcObject<FbxMesh>(j);
		m_MeshParts[j].Name = mesh->GetName();
		loadPositions(mesh, j);
		loadNormals(mesh, j);
		loadTexcoords(mesh, j);
		loadIndices(mesh, j);
		loadMaterialName(mesh, j);
		loadBone(mesh, j);

		auto* node = mesh->GetNode();
		if (node) {
			auto it = m_NodeMap.find(node->GetName());
			if (it != m_NodeMap.end()) {
				m_NodeParts[it->second].MeshIndex.push_back(j);
			}
		}
	}
}

void Converter::boneToJson(fbxsdk::FbxGeometryConverter& converter, fbxsdk::FbxScene* scene)
{
	m_NumNodeParts = scene->GetSrcObjectCount<FbxNode>();
	m_NumBoneParts = scene->GetSrcObjectCount<FbxSkeleton>();
	m_NodeParts.clear();
	m_NodeParts.reserve(m_NumNodeParts);
	m_BoneParts.clear();
	m_BoneParts.reserve(m_NumBoneParts);
	auto* root = scene->GetRootNode();
	loadNode(root, -1);

	for (int i = 0; i < m_NumBoneParts; ++i) {
		auto* skeleton = scene->GetSrcObject<FbxSkeleton>(i);
		if (!skeleton) continue;
		auto* boneNode = skeleton->GetNode();
		if (!boneNode) continue;

		std::string boneName = boneNode->GetName();
		addBone(boneName);
	}

}

void Converter::animationToJson(fbxsdk::FbxGeometryConverter& converter, fbxsdk::FbxScene* scene)
{
	boneToJson(converter, scene);

	m_NumAnimaParts = scene->GetSrcObjectCount<FbxAnimStack>();
	for (int i = 0; i < m_NumAnimaParts; ++i) {
		//	name
		FbxAnimStack* aniStack = scene->GetSrcObject<FbxAnimStack>(i);
		scene->SetCurrentAnimationStack(aniStack);
		AnimaPart aniPart;
		aniPart.Name = aniStack->GetName();
		//	duration
		FbxTimeSpan timeSpan = aniStack->GetLocalTimeSpan();
		FbxTime duration = timeSpan.GetDuration();
		aniPart.Duration = duration.GetSecondDouble();
		//	tick/sec
		FbxTime::EMode timeMode = scene->GetGlobalSettings().GetTimeMode();
		double ticksPerSecond = FbxTime::GetFrameRate(timeMode);
		aniPart.TickPerSec = ticksPerSecond;
		//	node
		auto totalFrames = aniPart.Duration * aniPart.TickPerSec;

		for (const auto& bone : m_BoneParts) {
			auto node = scene->FindNodeByName(bone.Name.c_str());
			if (!node) { continue; }

			AnimaNode aniNode;
			aniNode.Name = bone.Name;
			FbxTime frameTime;
			frameTime.SetSecondDouble(1.0 / aniPart.TickPerSec);

			for (uint32_t f = 0; f <= totalFrames; ++f) {
				FbxTime currentTime = frameTime * f;
				FbxAMatrix transform = node->EvaluateLocalTransform(currentTime);
				auto pos = transform.GetT();
				auto rot = transform.GetQ();
				auto scale = transform.GetS();

				Key key;
				key.Time = currentTime.GetSecondDouble();
				key.Tick = f;
				key.Pos = DirectX::SimpleMath::Vector3(
					static_cast<float>(pos[0]),
					static_cast<float>(pos[1]),
					static_cast<float>(pos[2]));
				key.Rot = DirectX::SimpleMath::Quaternion(
					static_cast<float>(rot[0]),
					static_cast<float>(rot[1]),
					static_cast<float>(rot[2]),
					static_cast<float>(rot[3]));
				key.Scale = DirectX::SimpleMath::Vector3(
					static_cast<float>(scale[0]),
					static_cast<float>(scale[1]),
					static_cast<float>(scale[2]));

				aniNode.Keys.push_back(key);
			}
			aniPart.Nodes.push_back(aniNode);
		}
		m_AnimaParts.push_back(aniPart);
	}
}

void Converter::loadMaterial(FbxSurfaceMaterial* material)
{
	MATERIAL tmpM;
	FbxDouble3 colors;
	FbxProperty prop;

	{//	マテリアルの取得
		//	ディフューズ
		prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
		if (prop.IsValid()) {
			colors = prop.Get<FbxDouble3>();
		}
		else {
			colors = FbxDouble3(1.0, 1.0, 1.0);
		}
		tmpM.materials.emplace_back((float)colors[0]);
		tmpM.materials.emplace_back((float)colors[1]);
		tmpM.materials.emplace_back((float)colors[2]);
		tmpM.diffuseName = getTexturePath(material, FbxSurfaceMaterial::sDiffuse);
		//スペキュラー
		prop = material->FindProperty(FbxSurfaceMaterial::sSpecular);
		if (prop.IsValid()) {
			colors = prop.Get<FbxDouble3>();
		}
		else {
			colors = FbxDouble3(0.0, 0.0, 0.0);
		}
		tmpM.materials.emplace_back((float)colors[0]);
		tmpM.materials.emplace_back((float)colors[1]);
		tmpM.materials.emplace_back((float)colors[2]);
		tmpM.specularName = getTexturePath(material, FbxSurfaceMaterial::sSpecular);
		//	アルファ
		float alpha = 1.0f;
		prop = material->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
		if (prop.IsValid()) {
			alpha = 1.f - prop.Get<float>();
		}
		tmpM.materials.emplace_back(alpha);
		//	シャイネス
		double shininess = 10;
		prop = material->FindProperty(FbxSurfaceMaterial::sShininess);
		if (prop.IsValid()) {
			shininess = prop.Get<float>();
		}
		else {
			shininess = 0.0;
		}
		tmpM.materials.emplace_back(shininess);
		tmpM.shinessName = getTexturePath(material, FbxSurfaceMaterial::sShininess);
		//	法線マップ
		prop = material->FindProperty(FbxSurfaceMaterial::sNormalMap);
		if (prop.IsValid()) {
			tmpM.normalName = getTexturePath(material, FbxSurfaceMaterial::sNormalMap);
		}

	}

	m_MaterialMap[material->GetName()] = tmpM;
}

std::string Converter::getTexturePath(fbxsdk::FbxSurfaceMaterial* material, const char* propertyName)
{
	FbxProperty prop = material->FindProperty(propertyName);
	if (!prop.IsValid()) return "";

	int numTexture = prop.GetSrcObjectCount<FbxFileTexture>();
	if (numTexture > 0) {
		auto* pTexture = prop.GetSrcObject<FbxFileTexture>(0);
		if (pTexture) {
			return RemoveDirectoryPath(pTexture->GetRelativeFileName());
		}
	}
	return "";
}

void Converter::loadPositions(FbxMesh* mesh, int k)
{
	//頂点座標の取得
	FbxVector4* pPositions = mesh->GetControlPoints();
	//インデックスの取得
	int* pIndices = mesh->GetPolygonVertices();
	//頂点数の取得
	int vertexCount = mesh->GetPolygonVertexCount();
	//メンバのfloat配列確保
	m_MeshParts[k].Positions.reserve(vertexCount * 3);
	//頂点座標をメンバに保存
	for (int i = 0; i < vertexCount; i++)
	{
		int index = pIndices[i];
		m_MeshParts[k].Positions.emplace_back((float)pPositions[index][m_Idx0]);
		m_MeshParts[k].Positions.emplace_back((float)pPositions[index][m_Idx1]);
		m_MeshParts[k].Positions.emplace_back((float)pPositions[index][m_Idx2]);
	}
}

void Converter::loadNormals(FbxMesh* mesh, int k)
{
	FbxArray<FbxVector4> normals;
	mesh->GetPolygonVertexNormals(normals);
	m_MeshParts[k].Normals.reserve(normals.Size() * 3);
	for (int i = 0; i < normals.Size(); i++)
	{
		m_MeshParts[k].Normals.emplace_back((float)normals[i][m_Idx0]);
		m_MeshParts[k].Normals.emplace_back((float)normals[i][m_Idx1]);
		m_MeshParts[k].Normals.emplace_back((float)normals[i][m_Idx2]);
	}
}

void Converter::loadTangents(fbxsdk::FbxMesh* mesh, int k)
{
	mesh->GenerateTangentsDataForAllUVSets(true);
	if (mesh->GetElementTangentCount() == 0) { return; }
	FbxLayerElementTangent* pTangent = mesh->GetElementTangent(0);
	const auto& tangents = pTangent->GetDirectArray();
	m_MeshParts[k].Tangents.reserve(tangents.GetCount() * 3);
	for (int i = 0; i < tangents.GetCount(); i++)
	{
		m_MeshParts[k].Tangents.emplace_back((float)tangents[i][m_Idx0]);
		m_MeshParts[k].Tangents.emplace_back((float)tangents[i][m_Idx1]);
		m_MeshParts[k].Tangents.emplace_back((float)tangents[i][m_Idx2]);
	}
}

void Converter::loadTexcoords(FbxMesh* mesh, int k)
{
	FbxStringList uvset_names;
	mesh->GetUVSetNames(uvset_names);
	//	先頭だけ見て、取り出す（複数テクスチャは非対応)
	FbxArray<FbxVector2> texcoords;
	mesh->GetPolygonVertexUVs(uvset_names.GetStringAt(0), texcoords);
	m_MeshParts[k].Texcoords.reserve(texcoords.Size() * 2);
	for (int i = 0; i < texcoords.Size(); i++)
	{
		m_MeshParts[k].Texcoords.emplace_back((float)texcoords[i][0]);
		m_MeshParts[k].Texcoords.emplace_back((float)(1.0 - texcoords[i][1]));
	}
}

void Converter::loadIndices(FbxMesh* mesh, int k)
{
	//ポリゴン数の取得
	int polygonCount = mesh->GetPolygonCount();
	//ポリゴンの数だけ連番として保存
	m_MeshParts[k].Indices.reserve(polygonCount * 3);
	for (int i = 0; i < polygonCount; i++)
	{
		m_MeshParts[k].Indices.emplace_back(i * 3 + 0);
		m_MeshParts[k].Indices.emplace_back(i * 3 + 1);
		m_MeshParts[k].Indices.emplace_back(i * 3 + 2);
	}
}

void Converter::loadMaterialName(FbxMesh* mesh, int k)
{
	//マテリアルが無ければ終わり
	if (mesh->GetElementMaterialCount() == 0) {
		m_MeshParts[k].MaterialName = "";
		return;
	}

	//Mesh側のマテリアル情報を取得(mapのkeyとして使う)
	FbxLayerElementMaterial* material = mesh->GetElementMaterial(0);
	int index = material->GetIndexArray().GetAt(0);

	auto node = mesh->GetNode();
	auto src = node->GetSrcObject<FbxSurfaceMaterial>(index);


	FbxSurfaceMaterial* pSurface_material =
		mesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(index);
	if (pSurface_material) {
		m_MeshParts[k].MaterialName = pSurface_material->GetName();
		m_MeshParts[k].MaterialID = index;
	}
	else {
		m_MeshParts[k].MaterialName = "";
		m_MeshParts[k].MaterialID = -1;
	}
}

bool Converter::hasMesh(fbxsdk::FbxScene* scene)
{
	return scene->GetSrcObjectCount<FbxMesh>() > 0;
}

bool Converter::hasBone(fbxsdk::FbxScene* scene)
{
	return scene->GetSrcObjectCount<FbxSkeleton>() > 0;
}

bool Converter::hasAnimation(fbxsdk::FbxScene* scene)
{
	return scene->GetSrcObjectCount<FbxAnimStack>() > 0;
}

void Converter::loadBone(fbxsdk::FbxMesh* mesh, int k)
{
	int skinCount = mesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
	if (skinCount == 0) return;

	int controlPointCount = mesh->GetControlPointsCount();

	std::vector<WeightData> cpWeights(controlPointCount);

	for (int i = 0; i < skinCount; ++i) {
		auto* skin = static_cast<fbxsdk::FbxSkin*>(mesh->GetDeformer(i, fbxsdk::FbxDeformer::eSkin));
		int clusterCount = skin->GetClusterCount();

		for (int j = 0; j < clusterCount; ++j) {
			fbxsdk::FbxCluster* cluster = skin->GetCluster(j);
			fbxsdk::FbxNode* boneNode = cluster->GetLink();
			if (!boneNode) continue;

			int boneID = getBoneIndex(cluster);
			// 影響を受けるindexとウェイトを取得
			int indexCount = cluster->GetControlPointIndicesCount();
			int* indices = cluster->GetControlPointIndices();
			double* weights = cluster->GetControlPointWeights();

			for (int w = 0; w < indexCount; ++w) {
				int cpIndex = indices[w];
				float weight = static_cast<float>(weights[w]);

				auto& slot = cpWeights[cpIndex];

				//  現在のボーンidが開いてる場所を探す
				int findIdx = -1;
				for (int s = 0; s < 4; ++s) {
					if (slot.weights[s] == 0.0f) {
						findIdx = s;
						break;
					}
				}
				//  最も重みの小さいものを探す
				if (findIdx == -1) {
					float minW = weight;
					for (int s = 0; s < 4; ++s) {
						if (slot.weights[s] < minW) {
							minW = slot.weights[s];
							findIdx = s;
						}
					}
				}
				//  index, weightのセット
				if (findIdx != -1) {
					slot.boneIDs[findIdx] = static_cast<uint16_t>(boneID);
					slot.weights[findIdx] = weight;
				}
			}
		}
	}
	//  ウェイト正規化
	for (auto& slot : cpWeights) {
		float totalWeight = slot.weights[0] + slot.weights[1] + slot.weights[2] + slot.weights[3];
		if (totalWeight > 0.0f) {
			slot.weights[0] /= totalWeight;
			slot.weights[1] /= totalWeight;
			slot.weights[2] /= totalWeight;
			slot.weights[3] /= totalWeight;
		}
	}
	int polygonVertexCount = mesh->GetPolygonVertexCount();
	int* pIndices = mesh->GetPolygonVertices();
	m_MeshParts[k].BoneIDs.reserve(polygonVertexCount * 4);
	m_MeshParts[k].BoneWeights.reserve(polygonVertexCount * 4);

	for (int v = 0; v < polygonVertexCount; ++v) {
		int cpIndex = pIndices[v];
		const auto& slot = cpWeights[cpIndex];
		for (int s = 0; s < 4; ++s) {
			m_MeshParts[k].BoneIDs.push_back(slot.boneIDs[s]);
			m_MeshParts[k].BoneWeights.push_back(slot.weights[s]);
		}
	}
}

void Converter::loadNode(fbxsdk::FbxNode* node, int parentIndex)
{
	if (!node) return;

	int currentIndex = static_cast<int>(m_NodeParts.size());

	NodePart parts;
	std::string name = node->GetName();
	parts.Name = name;
	parts.ParentIndex = parentIndex;
	parts.Node = node;
	//	transform
	FbxAMatrix localTransform = node->EvaluateLocalTransform();
	FbxVector4 pos = localTransform.GetT();
	FbxQuaternion rot = localTransform.GetQ();
	FbxVector4 scale = localTransform.GetS();

	parts.Pos.x = static_cast<float>(pos[0]);
	parts.Pos.y = static_cast<float>(pos[1]);
	parts.Pos.z = static_cast<float>(pos[2]);

	parts.Rot = DirectX::SimpleMath::Quaternion(
		static_cast<float>(rot[0]),
		static_cast<float>(rot[1]),
		static_cast<float>(rot[2]),
		static_cast<float>(rot[3])
	);

	parts.Scale.x = static_cast<float>(scale[0]);
	parts.Scale.y = static_cast<float>(scale[1]);
	parts.Scale.z = static_cast<float>(scale[2]);

	parts.Index = m_NextIdx;
	m_NodeParts.push_back(parts);
	if (!m_NodeMap.contains(name)) {
		m_NodeMap[name] = m_NextIdx;
	}
	m_NextIdx++;

	if (parentIndex >= 0 && parentIndex < static_cast<int>(m_NodeParts.size())) {
		m_NodeParts[parentIndex].ChildrenIndices.push_back(currentIndex);
	}

	// 子ノードへ再帰
	int childCount = node->GetChildCount();
	for (int j = 0; j < childCount; ++j) {
		loadNode(node->GetChild(j), currentIndex);
	}
}


int Converter::getBoneIndex(fbxsdk::FbxCluster* cluster)
{
	assert(cluster);
	fbxsdk::FbxNode* boneNode = cluster->GetLink();
	assert(boneNode);
	std::string name = boneNode->GetName();

	for (const auto& bone : m_BoneParts) {
		if (bone.Name == name) {
			return bone.ID;
		}
	}
	BonePart adder;
	adder.Name = name;
	adder.ID = allocBoneID();
	int result = adder.ID;

	auto it = m_NodeMap.find(name);
	if (it != m_NodeMap.end()) {
		int idx = it->second;
		adder.NodeIndex = m_NodeParts[idx].Index;
		m_NodeParts[idx].BoneIndex.push_back(adder.ID);
	}

	m_BoneParts.push_back(adder);

	return result;
}

void Converter::addBone(std::string boneName)
{
	// 重複登録のチェック
	for (const auto& bone : m_BoneParts) {
		if (bone.Name == boneName) {
			return;
		}
	}

	BonePart adder;
	adder.Name = boneName;
	adder.ID = allocBoneID();

	auto it = m_NodeMap.find(boneName);
	if (it != m_NodeMap.end()) {
		int idx = it->second;
		adder.NodeIndex = m_NodeParts[idx].Index;
		m_NodeParts[idx].BoneIndex.push_back(adder.ID);
	}

	m_BoneParts.push_back(adder);
}



void Converter::createTextConbineVertex(const char* txtFilename,
	float fx, float fy, float fz, fbxsdk::Type type)
{
	std::ofstream ofs(txtFilename);

	ofs << "[" << std::endl;


	switch (type)
	{
	case fbxsdk::Type::staticMesh:
	{
		createTextVertex_Static(ofs, txtFilename, fx, fy, fz);
		break;
	}
	case fbxsdk::Type::skinnedMesh:
	{
		createTextVertex_Skinned(ofs, txtFilename, fx, fy, fz);
		break;
	}
	case fbxsdk::Type::Animation:
	{
		createTextAnimation(ofs, txtFilename, fx, fy, fz);
		break;
	}

	}
	ofs << "]" << std::endl;

}

void Converter::createTextVertex_Static(std::ofstream& ofs,
	const char* txtFilename, float fx, float fy, float fz)
{
	for (int k = 0; k < m_NumMeshParts; k++) {
		ofs << " {" << std::endl;
		ofs << std::format("\"{}\" : [{}, {}, {}],\n", "Invert", fx, fy, fz);
		ofs << std::format("\"{}\" : \"{}\",\n", "MeshName", m_MeshParts[k].Name);

		{//	マテリアルID
			ofs << std::format("\"{}\" : {},\n", "MaterialID", m_MeshParts[k].MaterialID);
		}

		{//	座標
			size_t numVertices = m_MeshParts[k].Positions.size() / 3;
			ofs << std::format("\"{}\" : {},\n", "Num_Vertices", numVertices);
			ofs << std::format("\"{}\" : [\n", "Vertices");
			size_t i = 0;
			size_t j = 0;//Texcoordsのインデックス
			while (i < m_MeshParts[k].Positions.size()) {
				auto nx = m_MeshParts[k].Normals[i];
				auto ny = m_MeshParts[k].Normals[i + 1];
				auto nz = m_MeshParts[k].Normals[i + 2];

				if (fx < 0) {
					nx *= -1.0f;
				}
				if (fy < 0) {
					ny *= -1.0f;
				}
				if (fz < 0) {
					nz *= -1.0f;
				}

				bool hasUV = !m_MeshParts[k].Texcoords.empty();
				float u = hasUV ? m_MeshParts[k].Texcoords[j + 0] : 0.0f;
				float v = hasUV ? m_MeshParts[k].Texcoords[j + 1] : 0.0f;


				ofs << std::format("   [{}, {}, {}, {}, {}, {}, {}, {}]",
					ClampEps(fx * m_MeshParts[k].Positions[i]),		//	x
					ClampEps(fy * m_MeshParts[k].Positions[i + 1]),	//	y
					ClampEps(fz * m_MeshParts[k].Positions[i + 2]),	//	z
					ClampEps(nx), ClampEps(ny), ClampEps(nz),		//	normal
					ClampEps(u),		//	uv.x
					ClampEps(v)		//	uv.y
				);

				i += 3;
				j += 2;
				if (i < m_MeshParts[k].Positions.size()) {
					ofs << ",\n";
				}
				else {
					ofs << "\n";
				}
			}
			ofs << "  ]," << "\n";
		}

		{//	indices
			size_t numIndices = m_MeshParts[k].Indices.size();//要素数がインデックス数。頂点系とは違います。
			ofs << std::format("\"{}\" : [\n", "Indices");
			size_t i = 0;
			while (i < m_MeshParts[k].Indices.size()) {
				ofs << "   [";
				ofs << m_MeshParts[k].Indices[i++] << ", ";
				ofs << m_MeshParts[k].Indices[i++] << ", ";
				ofs << m_MeshParts[k].Indices[i++] << "]";

				if (i < m_MeshParts[k].Indices.size()) {
					ofs << ",\n";
				}
				else {
					ofs << "\n";
				}
			}
			ofs << "  ]\n";
		}
		if (k + 1 < m_NumMeshParts) {
			ofs << " }," << std::endl;
		}
		else {
			ofs << " }" << std::endl;
		}
	}
}

void Converter::createTextVertex_Skinned(std::ofstream& ofs, const char* txtFilename, float fx, float fy, float fz)
{
	ofs << " {" << std::endl;

	ofs << std::format("\"{}\" : [{}, {}, {}],\n", "Invert", fx, fy, fz);
	ofs << std::format("\"{}\" : {},\n", "Num_Mesh", m_NumMeshParts);
	ofs << std::format("\"{}\" : [\n", "Mesh");
	for (int k = 0; k < m_NumMeshParts; k++) {
		ofs << " {" << std::endl;
		ofs << std::format("\"{}\" : \"{}\",\n", "MeshName", m_MeshParts[k].Name);

		{//	マテリアルID
			ofs << std::format("\"{}\" : {},\n", "MaterialID", m_MeshParts[k].MaterialID);
		}

		{//	座標
			size_t numVertices = m_MeshParts[k].Positions.size() / 3;
			ofs << std::format("\"{}\" : {},\n", "Num_Vertices", numVertices);
			ofs << std::format("\"{}\" : [\n", "Vertices");
			size_t i = 0;
			size_t j = 0;//Texcoordsのインデックス
			while (i < m_MeshParts[k].Positions.size()) {
				auto nx = m_MeshParts[k].Normals[i];
				auto ny = m_MeshParts[k].Normals[i + 1];
				auto nz = m_MeshParts[k].Normals[i + 2];

				if (fx < 0) {
					nx *= -1.0f;
				}
				if (fy < 0) {
					ny *= -1.0f;
				}
				if (fz < 0) {
					nz *= -1.0f;
				}

				const auto& IDs = m_MeshParts[k].BoneIDs;
				const auto& weights = m_MeshParts[k].BoneWeights;
				size_t bIdx = i / 3 * 4;
				bool hasUV = !m_MeshParts[k].Texcoords.empty();
				float u = hasUV ? m_MeshParts[k].Texcoords[j + 0] : 0.0f;
				float v = hasUV ? m_MeshParts[k].Texcoords[j + 1] : 0.0f;

				ofs << std::format("   [{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}]",
					ClampEps(fx * m_MeshParts[k].Positions[i]),		//	x
					ClampEps(fy * m_MeshParts[k].Positions[i + 1]),	//	y
					ClampEps(fz * m_MeshParts[k].Positions[i + 2]),	//	z
					ClampEps(nx), ClampEps(ny), ClampEps(nz),		//	normal
					ClampEps(u),		//	uv.x
					ClampEps(v),		//	uv.y
					//tanx, tany, tanz,						//	tangent
					IDs[bIdx], IDs[bIdx + 1], IDs[bIdx + 2], IDs[bIdx + 3],	//	IDs
					ClampEps(weights[bIdx]), ClampEps(weights[bIdx + 1]),
					ClampEps(weights[bIdx + 2]), ClampEps(weights[bIdx + 3])	//	weights
				);

				i += 3;
				j += 2;
				if (i < m_MeshParts[k].Positions.size()) {
					ofs << ",\n";
				}
				else {
					ofs << "\n";
				}
			}
			ofs << "  ]," << "\n";
		}

		{//	indices
			size_t numIndices = m_MeshParts[k].Indices.size();
			ofs << std::format("\"{}\" : [\n", "Indices");
			size_t i = 0;
			while (i < m_MeshParts[k].Indices.size()) {
				ofs << "   [";
				ofs << m_MeshParts[k].Indices[i++] << ", ";
				ofs << m_MeshParts[k].Indices[i++] << ", ";
				ofs << m_MeshParts[k].Indices[i++] << "]";

				if (i < m_MeshParts[k].Indices.size()) {
					ofs << ",\n";
				}
				else {
					ofs << "\n";
				}
			}
			ofs << "  ]\n";
		}
		if (k + 1 < m_NumMeshParts) {
			ofs << "  }," << std::endl;
		}
		else {
			ofs << "  }" << std::endl;
		}
	}

	ofs << "  ],\n";

	//	マテリアル一括出力
	createTextMaterial(ofs, txtFilename);

	bool hasNodes = (m_NumNodeParts > 0);
	bool hasBones = (m_NumBoneParts > 0);

	if (hasNodes || hasBones) {
		ofs << ",\n";
	}
	else {
		ofs << "\n";
	}

	if (hasNodes) {	//	ノード
		ofs << std::format("\"{}\" : {},\n", "Num_Node", m_NumNodeParts);
		ofs << std::format("\"{}\" : [\n", "ModelNode");
		for (size_t i = 0; i < m_NodeParts.size(); ++i) {
			const auto& n = m_NodeParts[i];
			ofs << std::format("    {{\"Name\" : \"{}\", \"Matrix\" : [{}, {}, {}, {}, {}, {}, {}, {}, {}, {}], \"Parent\" : {}",
				n.Name,
				ClampEps(n.Pos.x, 10e-6f), ClampEps(n.Pos.y, 10e-6f), ClampEps(n.Pos.z, 10e-6f),
				ClampEps(n.Rot.x), ClampEps(n.Rot.y), ClampEps(n.Rot.z), ClampEps(n.Rot.w),
				ClampEps(n.Scale.x), ClampEps(n.Scale.y), ClampEps(n.Scale.z),
				n.ParentIndex);
			bool hasMesh = !n.MeshIndex.empty();
			bool hasBone = !n.BoneIndex.empty();
			bool hasChild = !n.ChildrenIndices.empty();

			if (hasMesh || hasBone || hasChild) {
				ofs << ", ";
				if (hasChild) {
					ofs << "\"Child\":[";
					for (size_t c = 0; c < n.ChildrenIndices.size(); ++c) {
						ofs << n.ChildrenIndices[c] << (c + 1 < n.ChildrenIndices.size() ? "," : "");
					}
					ofs << "]";
				}
				if (hasChild && (hasMesh || hasBone)) ofs << ", ";
				if (hasMesh) {
					ofs << "\"Mesh\":[";
					for (size_t m = 0; m < n.MeshIndex.size(); ++m) {
						ofs << n.MeshIndex[m] << (m + 1 < n.MeshIndex.size() ? "," : "");
					}
					ofs << "]";
				}
				if (hasMesh && hasBone) ofs << ", ";
				if (hasBone) {
					ofs << "\"Bone\":[";
					for (size_t b = 0; b < n.BoneIndex.size(); ++b) {
						ofs << n.BoneIndex[b] << (b + 1 < n.BoneIndex.size() ? "," : "");
					}
					ofs << "]";
				}
			}
			ofs << "}";
			if (i + 1 < m_NodeParts.size()) ofs << ",\n";
			else ofs << "\n";
		}
		if (hasBones) {
			ofs << "	],\n";
		}
		else {
			ofs << "	]\n";
		}
	}

	if (hasBones) {	//	ボーン
		ofs << std::format("\"{}\" : {},\n", "Num_Bone", m_NumBoneParts);
		ofs << std::format("\"{}\" : [\n", "Bone");
		for (size_t i = 0; i < m_BoneParts.size(); ++i) {
			const auto& b = m_BoneParts[i];
			ofs << std::format("    {{\"Name\" : \"{}\", \"ID\" : {}, \"Node\" : {}}}",
				b.Name, b.ID, b.NodeIndex);
			if (i + 1 < m_BoneParts.size()) ofs << ",\n";
			else ofs << "\n";
		}
		ofs << "  ]\n";
	}

	ofs << " }" << std::endl;
}
void Converter::createTextMaterial(std::ofstream& ofs, const char* txtFilename)
{
	ofs << std::format("\"{}\" : {},\n", "Num_Material", m_MaterialMap.size());
	ofs << std::format("\"{}\" : [\n", "Materials");

	size_t count = 0;
	for (auto& [matName, matData] : m_MaterialMap) {
		ofs << "  {\n";
		{//	マテリアル
			ofs << std::format("   \"{}\" : \"{}\",\n", "MaterialName", matName);

			ofs << std::format("   \"{}\" : \n", "Materials");
			float* d = matData.materials.data();
			ofs << std::format("    [{}, {}, {}, \n     {}, {}, {}, \n     {}, {}],\n",
				ClampEps(d[0]), ClampEps(d[1]), ClampEps(d[2]), ClampEps(d[3]),
				ClampEps(d[4]), ClampEps(d[5]), ClampEps(d[6]), ClampEps(d[7]));
		}
		{//	テクスチャ
			ofs << std::format("   \"{}\" : ", "Diffuse");
			//メッシュ側のマテリアル名でマテリアルマップから引っ張ってくる
			std::string pathName = txtFilename;
			std::string dir = GetDirectoryPath(txtFilename);
			assert(!dir.empty());
			//	テクスチャの保存先へパス変更
			std::string replaceDir = Replace(dir, "/Mesh/", "/Textures/");

			if (matData.diffuseName == "") {
				ofs << std::format("\"{}\"", "res/Util/white1x1.dds");
			}
			else {
				std::string fileName = RemoveExtension(matData.diffuseName) + ".dds";
				ofs << std::format("\"{}\"", replaceDir + fileName);
			}
			if (matData.specularName != "") {
				std::string fileName = RemoveExtension(matData.specularName) + ".dds";
				ofs << std::format(",\n   \"{}\" : ", "Specular");
				ofs << std::format("\"{}\"", replaceDir + fileName);
			}

			if (matData.shinessName != "") {
				std::string fileName = RemoveExtension(matData.shinessName) + ".dds";
				ofs << std::format(",\n   \"{}\" : ", "Shininess");
				ofs << std::format("\"{}\"", replaceDir + fileName);
			}

			if (matData.normalName != "") {
				std::string fileName = RemoveExtension(matData.normalName) + ".dds";
				ofs << std::format(",\n   \"{}\" : ", "NormalMap");
				ofs << std::format("\"{}\"", replaceDir + fileName);
			}
			ofs << "\n";
		}

		count++;
		if (count < m_MaterialMap.size()) {
			ofs << "  },\n";
		}
		else {
			ofs << "  }\n";
		}
	}
	ofs << " ]\n";
}
void Converter::createTextAnimation(std::ofstream& ofs, const char* txtFilename, float fx, float fy, float fz)
{
	ofs << " {" << std::endl;

	ofs << std::format("\"{}\" : [{}, {}, {}],\n", "Invert", fx, fy, fz);
	ofs << std::format("\"{}\" : {},\n", "Num_Anima", m_AnimaParts.size());
	ofs << std::format("\"{}\" : [\n", "Animation");

	for (size_t i = 0; i < m_AnimaParts.size(); ++i) {
		const auto& ani = m_AnimaParts[i];
		ofs << "  {\n";
		ofs << std::format("   \"{}\" : \"{}\",\n", "Name", ani.Name);
		ofs << std::format("   \"{}\" : {},\n", "Duration", ani.Duration);
		ofs << std::format("   \"{}\" : {},\n", "TickPerSec", ani.TickPerSec);
		ofs << std::format("   \"{}\" : {},\n", "Num_Node", ani.Nodes.size());
		ofs << std::format("   \"{}\" : [\n", "Node");

		for (size_t j = 0; j < ani.Nodes.size(); ++j) {
			const auto& node = ani.Nodes[j];
			ofs << "    {\n";
			ofs << std::format("     \"{}\" : \"{}\",\n", "Name", node.Name);
			ofs << std::format("     \"{}\" : {},\n", "Num_Key", node.Keys.size());
			ofs << std::format("     \"{}\" : [\n", "Key");

			for (size_t k = 0; k < node.Keys.size(); ++k) {
				const auto& key = node.Keys[k];

				// [Time, Tick, px, py, pz, rx, ry, rz, rw, sx, sy, sz]
				ofs << std::format("      [{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}]",
					key.Time, key.Tick,
					ClampEps(fx * key.Pos.x, 10e-6f), ClampEps(fy * key.Pos.y, 10e-6f), ClampEps(fz * key.Pos.z, 10e-6f),
					ClampEps(key.Rot.x), ClampEps(key.Rot.y), ClampEps(key.Rot.z), ClampEps(key.Rot.w),
					ClampEps(key.Scale.x), ClampEps(key.Scale.y), ClampEps(key.Scale.z)
				);

				if (k + 1 < node.Keys.size()) ofs << ",\n";
				else ofs << "\n";
			}

			ofs << "     ]\n";
			if (j + 1 < ani.Nodes.size()) ofs << "    },\n";
			else ofs << "    }\n";
		}

		ofs << "   ]\n";
		if (i + 1 < m_AnimaParts.size()) ofs << "  },\n";
		else ofs << "  }\n";
	}

	ofs << "  ]\n";
	ofs << " }" << std::endl;
}