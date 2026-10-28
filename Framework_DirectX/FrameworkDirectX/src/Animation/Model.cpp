#include "Animation/Model.h"
#include "Animation/convert_assimp.h"
#include "Animation/ModelMesh.h"
#include "Animation/ModelNode.h"
#include "Animation/Animation.h"
#include "Animation/Bone.h"
#include "Interface/IAsset.h"
#include "Mesh.h"
#include "ResMesh.h"
#include "Util/Json.hpp"
#include "GraphicContext.h"
#include <FileUtil.h>
#include <filesystem>
#include <PointerHelper.h>
#include <StringHelper.h>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/postprocess.h>
#include <assimp/metadata.h>
#include <assimp/scene.h>
#include "Util/JsonImporter.h"
#include <ranges>
#include <SimpleMath.h>

#include <iostream>
#include <format>


namespace {
    int boneCount = 0;
    int nodeCount = 0;
}

bool AssimpPack::m_BEnableBakingLoad = false;

AssimpPack::AssimpPack() :
    scene{},
    importer(std::unique_ptr<Assimp::Importer>())
{
}

AssimpPack::~AssimpPack()
{
}

void AssimpPack::init(std::wstring path, const char* data, size_t size)
{
    // 通常ロード
    if (!importer) {
	    importer = std::make_unique<Assimp::Importer>();
    }
    unsigned int flag =
        aiProcess_FlipWindingOrder |         // 頂点の結び順（時計回り/反時計回り）を反転
        aiProcess_FlipUVs |                  // UVのY軸を反転（上下逆さま防止）
        aiProcess_Triangulate |              // すべての多角形面を3角形に分割
        aiProcess_GenNormals |               // 通常の面法線を自動生成
        aiProcess_CalcTangentSpace |         // 法線マップ用の接線・従法線を計算
        aiProcess_FixInfacingNormals |       // 内側を向いてしまっている不正な法線を外側に修正
        aiProcess_TransformUVCoords |        // UVのマトリクス変形（回転・縮小等）を頂点に焼き付ける
        aiProcess_JoinIdenticalVertices |      // 重複する同一頂点を1つに結合してインデックス化
        aiProcess_MakeLeftHanded |          // 左手座標系に変換
        aiProcess_GlobalScale;             // ゲームエンジン側の単位（スケール）に一律変換
#pragma region disable flag
        //aiProcess_ImproveCacheLocality |   // GPUキャッシュに最適化するよう三角形を並び替え
        //aiProcess_GenSmoothNormals |       // 滑らかな法線を自動生成（GenNormalsと競合）
        //aiProcess_GenUVCoords |            // UVがない場合に簡易UVを自動生成
        //aiProcess_SplitLargeMeshes |       // 頂点数が多すぎる巨大メッシュを自動分割
        //aiProcess_Debone |                 // 影響度の低い不要なボーンを削除
        //aiProcess_RemoveComponent |        // カメラ・ライト・アニメーション等を除外
        //aiProcess_PreTransformVertices |   // 階層構造を破棄して頂点にトランスフォームを焼き付ける
        //aiProcess_ValidateDataStructure |  // 読み込みデータの破損チェック
        //aiProcess_RemoveRedundantMaterials |// 重複する無駄なマテリアルを統合
        //aiProcess_FindDegenerates |        // 面積ゼロの不正なポリゴン（点や線になったもの）を削除
        //aiProcess_FindInvalidData |        // NaNなどの無効な頂点やUVデータを検索して削除
        //aiProcess_GenUVCoords  |           // （重複）簡易UVの自動生成
        //aiProcess_FindInstances |          // 同一形状のメッシュを検索してインスタンス化（メモリ節約）
        //aiProcess_OptimizeMeshes |         // 小さなメッシュを結合して描画コール数を削減
        //aiProcess_OptimizeGraph |          // アニメーションに関係ない無駄なノード階層を平坦化
        //aiProcess_SortByPType              // 点・線・面などのプリミティブ別にメッシュを分離
#pragma endregion
    importer->SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.f); 

    if (data && size > 0) {
        scene = importer->ReadFileFromMemory(data, size,flag);
        return;
    }
    
    auto findPath = GetFilePathW(path.c_str());
	if (findPath.empty()) {
		assert(false);
		return;
	}
    auto pathStr = WStringToString(findPath);
    scene = importer->ReadFile(pathStr,flag);
}

Model::~Model()
{
    Destroy();
}

void Model::init(const std::wstring& path, const char* data, size_t size)
{
    if (m_PAssetManager->containsFromPath(path, AssetType::MODEL)) {
        return;
    }

    //if (GetExtension(path) == L".json") {
    //    LoadVertex_Skinned(path);
    //    return;
    //}

    m_AssimpPack = MAKE_UNIQUE_LOG<AssimpPack>();
    m_AssimpPack->init(path, data, size);
    const aiScene* scene = m_AssimpPack->getScene();

    aiMetadata* meta = scene->mMetaData;
    //  座標軸チェック
    for (unsigned int i = 0; i < meta->mNumProperties; ++i) {
        aiString key = meta->mKeys[i];
        aiMetadataEntry entry = meta->mValues[i];
        auto str = key.C_Str();
        if (str == "OriginalUpAxis") {
            if (std::abs(*static_cast<int32_t*>(entry.mData)) != 1) {
                ELOG("Error : Model::init - axis error");
            }
        }
    }
    //  名前セット　ファイル名（拡張子抜き）
    auto wstr = SplitW(path, L"/");
    auto str = WStringToString(SplitW(wstr[wstr.size() - 1], L".")[0]);
    setName(str);
    m_Path = path;
    //  ノードのロード
    uint32_t count = countNodes(scene->mRootNode);
    loadNode(scene->mRootNode, count);
    linkParent(scene->mRootNode);
    //  アマチュアの取得
    auto armatureNode = findArmatureNode(scene->mRootNode);
    std::string armatureName = convert_assimp::Format(armatureNode->mName);
    DirectX::SimpleMath::Matrix armatureWorld;
    auto armature = findNode(armatureName);
    if (armature) {
        armatureWorld = armature->calcWorldMatrix();
    }
    //  モデルリスト
    //  頂点タイプごとにロード
    auto graphicContext = GraphicContext::m_Instance;
    auto device = graphicContext->device();
    std::vector<ResMesh_Static> resMesh_Static;
    std::vector<ResMesh_Skinned> resMesh_Skinned;
    resMesh_Static.resize(scene->mNumMeshes);
    resMesh_Skinned.resize(scene->mNumMeshes);
    m_ModelMeshList.reserve(scene->mNumMeshes);

    for (u_int i = 0; i < scene->mNumMeshes; i++) 
    {
        aiMesh* currentAIMesh = scene->mMeshes[i];
        auto currentModelMesh = MAKE_UNIQUE_LOG<ModelMesh>();
        //  インデックス
        std::vector<uint32_t> indexList;
        loadIndex(currentAIMesh, indexList);
        //  頂点
        loadVertex_Skinned(currentAIMesh, resMesh_Skinned[i], armatureWorld);
        //  ボーン
        m_BoneList.resize(currentAIMesh->mNumBones);
        loadBone(currentAIMesh, resMesh_Skinned[i]);
        setVertColor(resMesh_Skinned[i]);
        //  resMesh
        resMesh_Skinned[i].Indices.resize(indexList.size());
        resMesh_Skinned[i].Indices = indexList;       
        resMesh_Skinned[i].MaterialId = currentAIMesh->mMaterialIndex;
        //  ノード名セット
        std::string meshName = convert_assimp::Format(currentAIMesh->mName);
        auto node = findParentNodeForMesh(scene->mRootNode, i);
        std::string nodeName = convert_assimp::Format(node->mName);
        currentModelMesh->setName(meshName);
        currentModelMesh->setNodeName(nodeName);
        //  初期化
        if (!currentModelMesh->init_Skinned(device, resMesh_Skinned[i])) {
            ELOG("Error : Model::init() - failed Init_Static");
            return;
        }
		currentModelMesh->setIndex(i);
		m_ModelMeshList.push_back(std::move(currentModelMesh));
    }
    //  ノード初期化
    initNodes();
    loadRootBone(const_cast<aiScene*>(scene));
    setNodeData();
    m_ModelNodeList[0]->calcTPoseNode(DirectX::SimpleMath::Matrix::Identity);
    //  テクスチャパス
    std::wstring dir = GetDirectoryPath(path.c_str());
    std::vector<ResMaterial> resMaterial;
    loadMaterial(scene, resMaterial);
    m_TexturePaths.resize(resMaterial.size() - 1);
    m_TexturePaths.clear();

    for (size_t i = 0; i < resMaterial.size(); ++i)
    {
        std::wstring normal;
        std::wstring dir = GetDirectoryPath(path.c_str());
        std::wstring texDir = ReplaceW(dir, L"/Mesh/", L"/Textures/");
        // DiffuseMap
        if (!resMaterial[i].DiffuseMap.empty()) {
            std::wstring filename = RemoveDirectoryPathW(resMaterial[i].DiffuseMap);
            filename = ConvFileExtension(filename, L".dds");
            m_TexturePaths.push_back(texDir + filename);
        }
        // SpecularMap
        if (!resMaterial[i].SpecularMap.empty()) {
            std::wstring filename = RemoveDirectoryPathW(resMaterial[i].SpecularMap);
            filename = ConvFileExtension(filename, L".dds");
            m_TexturePaths.push_back(texDir + filename);
        }
        // ShininessMap
        if (!resMaterial[i].ShininessMap.empty()) {
            std::wstring filename = RemoveDirectoryPathW(resMaterial[i].ShininessMap);
            filename = ConvFileExtension(filename, L".dds");
            m_TexturePaths.push_back(texDir + filename);
        }
        // NormalMap
        if (!resMaterial[i].NormalMap.empty()) {
            std::wstring filename = RemoveDirectoryPathW(resMaterial[i].NormalMap);
            filename = ConvFileExtension(filename, L".dds");
            m_TexturePaths.push_back(texDir + filename);
        }
    }
}

void Model::Destroy() {
    for (auto& node : m_ModelNodeList) {
        node->Destroy();
    }
    m_ModelMeshList.clear();
    m_ModelNodeList.clear();
    m_BoneList.clear();
    m_AssimpPack.reset();
}

void Model::addAnimation(const std::wstring& path, bool bIsLoop, double timeScale)
{
    //  重複チェック
    std::string key = WStringToString(path);
    if (m_AnimationMap.contains(key)) {
        return;
    }
    if (m_PAssetManager->containsFromPath(path, AssetType::ANIMATION)) {
        m_AnimationNameList.push_back(key);
        m_AnimationMap[key] = m_PAssetManager->getAnimation(key);
        return;
    }

    if (m_PAssetManager->containsFromPath(path, AssetType::ANIMATION)) {
        return;
    }

    m_AssimpPack->init(path);
    const aiScene* scene = m_AssimpPack->getScene();
    if (scene->HasAnimations()) {
        loadAnimation(path, scene->mAnimations[0], scene->mRootNode, bIsLoop, timeScale);
    }
}

void Model::addAnimation(const std::wstring& path, char* data, size_t size, bool bIsLoop, double timeScale)
{
    //  重複チェック
    std::string key = WStringToString(path);
    if (m_AnimationMap.contains(key)) {
        return;
    }
    if (m_PAssetManager->containsFromPath(path, AssetType::ANIMATION)) {
        m_AnimationNameList.push_back(key);
        m_AnimationMap[key] = m_PAssetManager->getAnimation(key);
        return;
    }

    if (m_PAssetManager->containsFromPath(path, AssetType::ANIMATION)) {
        return;
    }

    m_AssimpPack->init(path, data, size);
    const aiScene* scene = m_AssimpPack->getScene();
    if (scene->HasAnimations()) {
        loadAnimation(path, scene->mAnimations[0], scene->mRootNode, bIsLoop, timeScale);
    }
}

void Model::addAnimations(const std::wstring& path)
{
    if (m_PAssetManager->containsFromPath(path, AssetType::ANIMATION)) {
        return;
    }

    m_AssimpPack->init(path);
    const aiScene* scene = m_AssimpPack->getScene();
    for (uint32_t i = 0; i < scene->mNumAnimations; i++) {
        loadAnimation(path, scene->mAnimations[i], scene->mRootNode);
    }

}

void Model::exportBinary(const std::wstring& path, const std::wstring& subKey)
{
}

void Model::importBinary(const std::wstring& path, const std::wstring& subKey)
{
}

#pragma region Load

void Model::loadVertex_Skinned(
    aiMesh* assimp_mesh,
    ResMesh_Skinned& meshes,
    const DirectX::SimpleMath::Matrix& meshMatrix
    )
{
    using namespace DirectX::SimpleMath;
    std::vector<Vertex_Skinned> vertices;
    vertices.reserve(assimp_mesh->mNumVertices);

    Vertex_Skinned vert;
    Vector3 min = Vector3(10000, 10000, 10000);
    Vector3 max = -min;


    for (u_int j = 0; j < assimp_mesh->mNumVertices; j++)
    {
        vert = {};
        Vector3 pos = convert_assimp::Format(assimp_mesh->mVertices[j]);
        vert.m_Position = Vector3::Transform(pos, meshMatrix);
        if (assimp_mesh->HasNormals()) {

            Vector3 normal = convert_assimp::Format(assimp_mesh->mNormals[j]);
            Vector3 result = Vector3::Transform(normal, meshMatrix);
            result.Normalize();
            vert.m_Normal = result;
        }
        if (assimp_mesh->HasVertexColors(0))
        {
            vert.m_Color = convert_assimp::Format(assimp_mesh->mColors[0][j]);
        }
        if (assimp_mesh->HasTangentsAndBitangents())
        {
            vert.m_Tangent = convert_assimp::Format(assimp_mesh->mTangents[j]);
        }
        for (int k = 0; k < 2; k++) {
            if (assimp_mesh->HasTextureCoords(k))
            {
                vert.m_TexCoords[k] = Vector2(convert_assimp::Format(assimp_mesh->mTextureCoords[k][j]));
            }
        }
        vert.m_BoneWeight = Vector4::Zero;
        vert.m_BoneID.x = UINT32_MAX;
        vert.m_BoneID.y = UINT32_MAX;
        vert.m_BoneID.z = UINT32_MAX;
        vert.m_BoneID.w = UINT32_MAX;

        vertices.push_back(vert);

        Vector3::Min(min, vert.m_Position , min);
        Vector3::Max(max, vert.m_Position, max);
    }

    meshes.Vertices.reserve(assimp_mesh->mNumVertices);
    meshes.Vertices = vertices;
}

void Model::loadIndex(aiMesh* assimp_mesh, std::vector<uint32_t>& indexs)
{
    indexs.reserve(assimp_mesh->mNumFaces * 3);

    for (u_int i = 0; i < assimp_mesh->mNumFaces; i++) {
        auto& poly = assimp_mesh->mFaces[i];
        if (poly.mNumIndices == 3) {
            indexs.push_back(poly.mIndices[0]);
            indexs.push_back(poly.mIndices[1]);
            indexs.push_back(poly.mIndices[2]);
        }
    }
}

void Model::loadBone(
    aiMesh* assimp_mesh,
    ResMesh_Skinned& resMesh)
{
    auto vertices = resMesh.Vertices;

    //  ボーンを追加し、ボーンの重みを各頂点に設定    
    for (u_int boneIdx = 0; boneIdx < assimp_mesh->mNumBones; boneIdx++) {
        aiBone* aiBone = assimp_mesh->mBones[boneIdx];
        std::string boneName = convert_assimp::Format(aiBone->mName);
        std::string nodeName = boneName;
        aiNode* boneNode = aiBone->mNode;
        if (boneNode != nullptr) {
            nodeName = convert_assimp::Format(aiBone->mNode->mName);
        }

        auto bone = findBone(boneName);
        if (!bone) {
            auto b= MAKE_UNIQUE_LOG<Bone>();
            b->setName(boneName);
            b->setNodeName(nodeName);

            b->setWorldMatrix(DirectX::SimpleMath::Matrix::Identity);
            bone = b.get();

            addBone(std::move(b));
            boneCount++;
        }
        //  ボーンの重みを頂点に設定
        for (uint32_t boneVertexIdx = 0; boneVertexIdx < aiBone->mNumWeights; boneVertexIdx++) {
            auto id = aiBone->mWeights[boneVertexIdx].mVertexId;
            auto& currentVertex = vertices[id];
            auto& currentResVertex = resMesh.Vertices[id];
            
            int findIdx = UINT_MAX;
            uint32_t* idArray = &currentVertex.m_BoneID.x;
            float* weighArray = &currentVertex.m_BoneWeight.x;
            const int MAX_BONE_COUNT = 4;
            //  現在のボーンidが開いてる場所を探す
            for (int i = 0; i < MAX_BONE_COUNT; ++i) {
                if (idArray[i] == UINT_MAX) {
                    findIdx = i;
                    break;
                }
            }
            //  最も重みの小さいものを探す
            if (findIdx == UINT_MAX) {
                float minW = aiBone->mWeights[boneVertexIdx].mWeight;
                for (int j = 0; j < MAX_BONE_COUNT; j++) {
                    if (weighArray[j] < minW) {
                        minW = weighArray[j];
                        findIdx = j;
                    }
                }
            }
            //  index, weightのセット
            if (findIdx != UINT_MAX) {
                int Idx = (bone->index());
                (&currentVertex.m_BoneID.x)[findIdx] = Idx;
                (&currentResVertex.m_BoneID.x)[findIdx] = Idx;
                auto w = aiBone->mWeights[boneVertexIdx].mWeight;
                (&currentVertex.m_BoneWeight.x)[findIdx] = w;
                (&currentResVertex.m_BoneWeight.x)[findIdx] = w;
            }
        }
    }
    //  ウェイト正規化
    for (int i=0;i <resMesh.Vertices.size();i++)
    {
        auto& w = resMesh.Vertices[i].m_BoneWeight;
        float totalWeight = w.x + w.y + w.z + w.w;
        if (totalWeight > 0.0f)
        {
            w.x /= totalWeight;
            w.y /= totalWeight;
            w.z /= totalWeight;
            w.w /= totalWeight;
        }
    }
}

void Model::loadNode(aiNode* root, uint32_t count)
{
    m_ModelNodeList.reserve(count);
    createNodes(root);
}

std::string Model::loadAnimation(
    const std::wstring& path,
    aiAnimation* aiAnim,
    aiNode* root,
    bool bIsLoop,
    double timeScale
)
{
    // モデル名 | ファイル名　をキーとして登録
    auto fileName = GetFileName(path.c_str());
    auto keyName = m_Name + "|" + fileName;
    if (m_AnimationMap.contains(keyName)) {
        return keyName; //  追加済みは登録しない
    }
   
    auto anima = MAKE_UNIQUE_LOG<Animation>();
    anima->init(this, aiAnim, root, fileName);
    anima->setIsLoop(bIsLoop);
    anima->setTimeScale(timeScale);
    m_AnimationNameList.push_back(keyName);
    m_PAssetManager->Add(path,keyName, std::move(anima));
    m_AnimationMap[keyName] = m_PAssetManager->getAnimation(keyName);

    return keyName;
}

void Model::loadRootBone(aiScene* scene)
{
    if (m_BoneList.empty()) return;
    //  適当なノードを取得（ファイルの先頭）
    auto& boneName = m_BoneList[0]->name();
    aiNode* node = scene->mRootNode->FindNode(boneName.c_str());
    if (!node) {
        ELOG("Error : Model::LoadRootBone - node null");
        return;
    }
    //  親を遡ってルートボーンを探す
    aiNode* rootBoneNode = node;
    while (rootBoneNode->mParent) {
        std::string parentName = convert_assimp::Format(rootBoneNode->mParent->mName);
        bool parentIsBone = false;

        for (auto& bone : m_BoneList) {
            if (bone->name() == parentName) {
                parentIsBone = true;
                break;
            }
        }

        if (!parentIsBone) { break; }

        rootBoneNode = rootBoneNode->mParent;
    }

    std::string rootBoneName = convert_assimp::Format(rootBoneNode->mName);
    auto it = m_NodeMap.find(rootBoneName);

    if (it != m_NodeMap.end()) {
        m_RootBoneNode = it->second;
        ROOT_NODE_INDEX = findNodeIndex(m_RootBoneNode);
    }
    else {
        ELOG("Errr : Model::LoadRootBone - not found rootBone");
    }
}

void Model::setNodeData()
{
    for (auto& bone : m_BoneList)
    {
        auto currentNode = findNode(bone->nodeName());
        if (!currentNode) continue;
        currentNode->addBoneIndex(bone->index());
        currentNode->setDynamic(false);
        bone->setNodeIndex(findNodeIndex(currentNode));
    }
}

void Model::loadVertex_Skinned(const std::wstring& filepath)
{
    m_Path = filepath;
    //  名前セット　ファイル名（拡張子抜き）
    auto wstr = SplitW(filepath, L"/");
    auto str = WStringToString(SplitW(wstr[wstr.size() - 1], L".")[0]);
    setName(str);

    const auto& json = JsonImporter::getJson(filepath);

    ResMesh_Skinned resMesh;
    ResMaterial resMat;
    JsonImporter::skinnedMesh(json, resMesh, resMat, *this);

    auto currentModelMesh = MAKE_UNIQUE_LOG<ModelMesh>();
    auto graphicContext = GraphicContext::m_Instance;
    auto device = graphicContext->device();

    if (!currentModelMesh->init_Skinned(device, resMesh)) {
        ELOG("Error : Model::init() - failed Init_Static");
        return;
    }
    currentModelMesh->setIndex(0);

    m_ModelMeshList.push_back(std::move(currentModelMesh));
    initNodes();

    m_TexturePaths.push_back(resMat.DiffuseMap);
    if (!resMat.SpecularMap.empty()) {
        m_TexturePaths.push_back(resMat.SpecularMap);
    }
    if (!resMat.ShininessMap.empty()) {
        m_TexturePaths.push_back(resMat.ShininessMap);
    }
    if (!resMat.NormalMap.empty()) {
        m_TexturePaths.push_back(resMat.NormalMap);
    }

    printf("Finish\n");
}


std::vector<Bone*> Model::getBoneList()
{
    return std::vector<Bone*>();
}

void Model::addBone(LogPtr<Bone> bone)
{
    int boneId = allocBoneID();
    m_BoneMap[bone->name()] = bone.get();
    bone->setIndex(boneId);
    m_BoneList[boneId] = std::move(bone);
}

uint32_t Model::countNodes(aiNode* node)
{   
    uint32_t count = 1;
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        count += countNodes(node->mChildren[i]); // 子を再帰的にカウント
    }
   
    std::string nodeName = node->mName.C_Str();
    if (nodeName.empty() || nodeName == "") {
        return 0;
    }

    return count;
}

void Model::createNodes(aiNode* node)
{
    std::string nodeName = node->mName.C_Str();

    bool bHasIgnoreName = (nodeName.empty() || nodeName == "");
    if (!bHasIgnoreName)
    {
        auto currentNode = MAKE_UNIQUE_LOG<ModelNode>();
        currentNode->init(this, node);
        m_ModelNodeList.push_back(std::move(currentNode));
        nodeCount++;
#ifdef _DEBUG
        std::cout << "Add\t" << "nodeName = " << nodeName << std::endl;
    }
    else {
        std::cout << "Skip\t" << "nodeName = " << nodeName << std::endl;
    }
#else
    }
#endif // _DEBUG

    for (u_int i = 0; i < node->mNumChildren; i++) {
        if (node->mChildren[i]) {
            createNodes(node->mChildren[i]);
        }
    }
}

void Model::initNodes()
{
    for (auto& m : m_ModelNodeList) {
        m->setIndexList(this);
    }
}

void Model::linkParent(aiNode* node)
{
    // 自分のノードを探す
    ModelNode* self = findNode(node->mName.C_Str());

    for (u_int i = 0; i < node->mNumChildren; i++) {
        // 子を探す
        ModelNode* child = findNode(node->mChildren[i]->mName.C_Str());
         if (self && child) {
            child->setParent(self);
        }
        // さらに再帰
        linkParent(node->mChildren[i]);
    }
}

const aiNode* Model::findArmatureNode(const aiNode* node)
{
    if (!node) return nullptr;

    if (std::string(node->mName.C_Str()).find("Armature") != std::string::npos) {
        return node;
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        const aiNode* found = findArmatureNode(node->mChildren[i]);
        if (found) return found;
    }

    return nullptr;
}

const aiNode* Model::findParentNodeForMesh(const aiNode* node, uint32_t meshIndex)
{
    for (uint32_t i = 0; i < node->mNumMeshes; ++i)
    {
        if (node->mMeshes[i] == meshIndex) return node;
    }
    for (uint32_t i = 0; i < node->mNumChildren; ++i)
    {
        const aiNode* found = findParentNodeForMesh(node->mChildren[i], meshIndex);
        if (found) return found;
    }
    return nullptr;
}

void Model::setVertColor(ResMesh_Skinned& resMeshe)
{
#ifdef _DEBUG
    //int lighFootIdx = -100;;
    //if (m_BoneMap.contains("mixamorig6:RightFoot")) {
    //    lighFootIdx = m_BoneMap["mixamorig6:RightFoot"]->index();
    //}

    //for (auto& v : resMeshe.Vertices) {
    //    float maxWeight = v.m_BoneWeight.x;
    //    int maxID = v.m_BoneID.x;

    //    if (v.m_BoneWeight.y > maxWeight) {
    //        maxWeight = v.m_BoneWeight.y;
    //        maxID = v.m_BoneID.y;
    //    }
    //    if (v.m_BoneWeight.z > maxWeight) {
    //        maxWeight = v.m_BoneWeight.z;
    //        maxID = v.m_BoneID.z;
    //    }
    //    if (v.m_BoneWeight.w > maxWeight) {
    //        maxWeight = v.m_BoneWeight.w;
    //        maxID = v.m_BoneID.w;
    //    }

    //    auto NumBone = m_BoneList.size();
    //    float rate = maxID / (float)NumBone;

    //    DirectX::XMFLOAT3 rgb = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    //    if (rate < 1.0f / 3.0f) {
    //        float t = rate / (1.0f / 3.0f);
    //        rgb = DirectX::XMFLOAT3(0.2f + t * 0.8f, 0.0f, 0.0f);
    //    }
    //    else if (rate < 2.0f / 3.0f) {
    //        float t = (rate - 1.0f / 3.0f) / (1.0f / 3.0f);
    //        rgb = DirectX::XMFLOAT3(0.0f, 0.2f + t * 0.8f, 0.0f);
    //    }
    //    else {
    //        float t = (rate - 2.0f / 3.0f) / (1.0f / 3.0f);
    //        rgb = DirectX::XMFLOAT3(0.0f, 0.0f, 0.2f + t * 0.8f);
    //    }

    //    if (maxID == lighFootIdx) {
    //        v.m_Color.x = 1.0f;
    //        v.m_Color.y = 1.0f;
    //        v.m_Color.z = 1.0f;
    //        v.m_Color.w = 1.0f;
    //        continue;
    //    }

    //    v.m_Color.x = rgb.x;
    //    v.m_Color.y = rgb.y;
    //    v.m_Color.z = rgb.z;
    //    v.m_Color.w = 1.0f;
    //}
#endif // _DEBUG
}

#pragma endregion

std::vector<LogPtr<ModelNode>> Model::cloneModelNodeList() {
    std::vector<LogPtr<ModelNode>> clonedList;
    clonedList.reserve(m_ModelNodeList.size());
    std::unordered_map<std::string, ModelNode*> nameToCloneMap;
    // 全ノードをクローンしてマップに登録
    for (const auto& node : m_ModelNodeList) {
        auto clone = node->Clone();
        nameToCloneMap[clone->name()] = clone.get(); 
        clonedList.push_back(std::move(clone));
    }
    // 親子関係のセット
    for (const auto& oldNode : m_ModelNodeList) {
        ModelNode* newSelf = nameToCloneMap[oldNode->name()];
        if (oldNode->parent()) {
            newSelf->setParent(nameToCloneMap[oldNode->parent()->name()]);
        }
    }

    return clonedList;
}

std::vector<LogPtr<Bone>> Model:: cloneBoneList() {
    std::vector<LogPtr<Bone>> clonedList;
    clonedList.reserve(m_BoneList.size()); // メモリ確保を最適化

    for (const auto& bone : m_BoneList) {
        // 各BoneのCloneメソッドを呼んで、新しいインスタンスを作成
        clonedList.push_back(bone->Clone());
    }

    return clonedList;
}


#pragma region Access Map

ModelNode* Model::findNode(const std::string& name)
{
    auto it = m_NodeMap.find(name);
    return it != m_NodeMap.end() ? it->second : nullptr;
}

ModelNode* Model::findNode(int index)
{
    if (m_ModelNodeList.size() <= index || index < 0) { return nullptr; }

    return m_ModelNodeList[index].get();
}

std::vector<ModelMesh*> Model::findMesh(const std::string& name)
{
    return m_MeshListMap[name];
}

ModelMesh* Model::findMeshByIndex(const int& index)
{
    return m_ModelMeshList[index].get();
}

Bone* Model::findBone(const std::string& name)
{
    return m_BoneMap[name];
}

Bone* Model::findBoneFromIndex(int index)
{
    return m_BoneList[index].get();
}

uint32_t Model::findNodeIndex(const ModelNode* target) const{
    for (size_t i = 0; i < m_ModelNodeList.size(); ++i) {
        if (m_ModelNodeList[i].get() == target) {
            return static_cast<int>(i);
        }
    }
    return UINT_MAX;
}

Animation* Model::findAnimation(const std::string& key) const
{
    auto it = m_AnimationMap.find(key);
    return it != m_AnimationMap.end() ? it->second : nullptr;
}

#pragma endregion
