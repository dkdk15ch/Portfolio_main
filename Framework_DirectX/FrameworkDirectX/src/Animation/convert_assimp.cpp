#include "Animation/convert_assimp.h"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/ai_assert.h>

#pragma region convert_assimp

DirectX::SimpleMath::Matrix convert_assimp::Format(const aiMatrix4x4& mat)
{
    DirectX::SimpleMath::Matrix result(mat[0]);
    result._11 = mat.a1;
    result._21 = mat.a2;
    result._31 = mat.a3;
    result._41 = mat.a4;
    result._12 = mat.b1;
    result._22 = mat.b2;
    result._32 = mat.b3;
    result._42 = mat.b4;
    result._13 = mat.c1;
    result._23 = mat.c2;
    result._33 = mat.c3;
    result._43 = mat.c4;
    result._14 = mat.d1;
    result._24 = mat.d2;
    result._34 = mat.d3;
    result._44 = mat.d4;
    return result;
}


DirectX::SimpleMath::Matrix convert_assimp::Format(const aiMatrix3x3& mat)
{
    DirectX::SimpleMath::Matrix result = DirectX::SimpleMath::Matrix::Identity;
    result._11 = mat.a1;
    result._21 = mat.a2;
    result._31 = mat.a3;
    result._12 = mat.b1;
    result._22 = mat.b2;
    result._32 = mat.b3;
    result._13 = mat.c1;
    result._23 = mat.c2;
    result._33 = mat.c3;
    return result;
}

DirectX::SimpleMath::Vector3 convert_assimp::Format(const aiVector3D& from)
{
	return DirectX::SimpleMath::Vector3(from.x, from.y, from.z);
}

DirectX::SimpleMath::Vector2 convert_assimp::Format(const aiVector2D& from)
{
	return DirectX::SimpleMath::Vector2(from.x, from.y);
}

DirectX::SimpleMath::Quaternion convert_assimp::Format(const aiQuaternion& pOrientation)
{
	return DirectX::SimpleMath::Quaternion(
	    pOrientation.x,
	    pOrientation.y,
	    pOrientation.z,
	    pOrientation.w
	);
}

DirectX::SimpleMath::Color convert_assimp::Format(const aiColor4D& color)
{
	return DirectX::SimpleMath::Color(
	    color.r, color.g, color.b, color.a
	);
}

DirectX::SimpleMath::Ray convert_assimp::Format(const aiRay& ray)
{
    return DirectX::SimpleMath::Ray(Format(ray.pos), Format(ray.dir));
}

DirectX::SimpleMath::Plane convert_assimp::Format(const aiPlane& plane)
{
	return DirectX::SimpleMath::Plane(
	plane.a, plane.b, plane.c, plane.d);
}

std::string convert_assimp::Format(const aiString& string)
{
    
    std::string result = string.C_Str();
    return result;
}

#pragma endregion


bool MakeLeftHandedProcess::isActive(unsigned int pFlags) const
{
    return 0 != (pFlags & aiProcess_MakeLeftHanded);
}


void MakeLeftHandedProcess::execute(aiScene* pScene)
{
    // Check for an existent root node to proceed
    ai_assert(pScene->mRootNode != nullptr);
    ASSIMP_LOG_DEBUG("MakeLeftHandedProcess begin");

    // recursively convert all the nodes
    processNode(pScene->mRootNode, aiMatrix4x4());

    // process the meshes accordingly
    for (unsigned int a = 0; a < pScene->mNumMeshes; ++a) {
        processMesh(pScene->mMeshes[a]);
    }

    // process the materials accordingly
    for (unsigned int a = 0; a < pScene->mNumMaterials; ++a) {
        processMaterial(pScene->mMaterials[a]);
    }

    // transform all animation channels as well
    for (unsigned int a = 0; a < pScene->mNumAnimations; a++) {
        aiAnimation* anim = pScene->mAnimations[a];
        for (unsigned int b = 0; b < anim->mNumChannels; b++) {
            aiNodeAnim* nodeAnim = anim->mChannels[b];
            processAnimation(nodeAnim);
        }
    }

    // process the cameras accordingly
    for (unsigned int a = 0; a < pScene->mNumCameras; ++a)
    {
        processCamera(pScene->mCameras[a]);
    }
    ASSIMP_LOG_DEBUG("MakeLeftHandedProcess finished");
}


void MakeLeftHandedProcess::processNode(aiNode* pNode, const aiMatrix4x4& pParentGlobalRotation)
{
    // mirror all base vectors at the local Z axis
    pNode->mTransformation.c1 = -pNode->mTransformation.c1;
    pNode->mTransformation.c2 = -pNode->mTransformation.c2;
    pNode->mTransformation.c3 = -pNode->mTransformation.c3;
    pNode->mTransformation.c4 = -pNode->mTransformation.c4;

    // now invert the Z axis again to keep the matrix determinant positive.
    // The local meshes will be inverted accordingly so that the result should look just fine again.
    pNode->mTransformation.a3 = -pNode->mTransformation.a3;
    pNode->mTransformation.b3 = -pNode->mTransformation.b3;
    pNode->mTransformation.c3 = -pNode->mTransformation.c3;
    pNode->mTransformation.d3 = -pNode->mTransformation.d3; // useless, but anyways...

    // continue for all children
    for (size_t a = 0; a < pNode->mNumChildren; ++a) {
        processNode(pNode->mChildren[a], pParentGlobalRotation * pNode->mTransformation);
    }
}


void MakeLeftHandedProcess::processMesh(aiMesh* pMesh)
{
    if (nullptr == pMesh) {
        ASSIMP_LOG_ERROR("Nullptr to mesh found.");
        return;
    }
    // mirror positions, normals and stuff along the Z axis
    for (size_t a = 0; a < pMesh->mNumVertices; ++a) {
        pMesh->mVertices[a].z *= -1.0f;
        if (pMesh->HasNormals()) {
            pMesh->mNormals[a].z *= -1.0f;
        }
        if (pMesh->HasTangentsAndBitangents()) {
            pMesh->mTangents[a].z *= -1.0f;
            pMesh->mBitangents[a].z *= -1.0f;
        }
    }

    // mirror anim meshes positions, normals and stuff along the Z axis
    for (size_t m = 0; m < pMesh->mNumAnimMeshes; ++m) {
        for (size_t a = 0; a < pMesh->mAnimMeshes[m]->mNumVertices; ++a) {
            pMesh->mAnimMeshes[m]->mVertices[a].z *= -1.0f;
            if (pMesh->mAnimMeshes[m]->HasNormals()) {
                pMesh->mAnimMeshes[m]->mNormals[a].z *= -1.0f;
            }
            if (pMesh->mAnimMeshes[m]->HasTangentsAndBitangents()) {
                pMesh->mAnimMeshes[m]->mTangents[a].z *= -1.0f;
                pMesh->mAnimMeshes[m]->mBitangents[a].z *= -1.0f;
            }
        }
    }

    // mirror offset matrices of all bones
    for (size_t a = 0; a < pMesh->mNumBones; ++a) {
        aiBone* bone = pMesh->mBones[a];
        bone->mOffsetMatrix.a3 = -bone->mOffsetMatrix.a3;
        bone->mOffsetMatrix.b3 = -bone->mOffsetMatrix.b3;
        bone->mOffsetMatrix.d3 = -bone->mOffsetMatrix.d3;
        bone->mOffsetMatrix.c1 = -bone->mOffsetMatrix.c1;
        bone->mOffsetMatrix.c2 = -bone->mOffsetMatrix.c2;
        bone->mOffsetMatrix.c4 = -bone->mOffsetMatrix.c4;
    }

    // mirror bitangents as well as they're derived from the texture coords
    if (pMesh->HasTangentsAndBitangents()) {
        for (unsigned int a = 0; a < pMesh->mNumVertices; a++)
            pMesh->mBitangents[a] *= -1.0f;
    }
}


void MakeLeftHandedProcess::processMaterial(aiMaterial* pMat)
{
    if (nullptr == pMat) {
        ASSIMP_LOG_ERROR("Nullptr to aiMaterial found.");
        return;
    }

    aiMaterial* mat = (aiMaterial*)pMat;
    for (unsigned int a = 0; a < mat->mNumProperties; ++a) {
        aiMaterialProperty* prop = mat->mProperties[a];

        // Mapping axis for UV mappings?
        if (!::strcmp(prop->mKey.data, "$tex.mapaxis")) {
            ai_assert(prop->mDataLength >= sizeof(aiVector3D)); // something is wrong with the validation if we end up here
            aiVector3D* pff = (aiVector3D*)prop->mData;
            pff->z *= -1.f;
        }
    }
}


void MakeLeftHandedProcess::processAnimation(aiNodeAnim* pAnim)
{
    // position keys
    for (unsigned int a = 0; a < pAnim->mNumPositionKeys; a++)
        pAnim->mPositionKeys[a].mValue.z *= -1.0f;

    // rotation keys
    for (unsigned int a = 0; a < pAnim->mNumRotationKeys; a++) {
        pAnim->mRotationKeys[a].mValue.x *= -1.0f;
        pAnim->mRotationKeys[a].mValue.y *= -1.0f;
    }
}


void MakeLeftHandedProcess::processCamera(aiCamera* pCam)
{
    pCam->mLookAt = ai_real(2.0f) * pCam->mPosition - pCam->mLookAt;
}
