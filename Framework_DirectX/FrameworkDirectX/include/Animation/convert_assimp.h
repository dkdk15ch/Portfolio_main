#pragma once
#include <SimpleMath.h>
#include <assimp/color4.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/types.h>
#include <assimp/vector2.h>
#include <assimp/scene.h>

namespace convert_assimp
{
    DirectX::SimpleMath::Matrix Format(const aiMatrix4x4& mat);
    DirectX::SimpleMath::Matrix Format(const aiMatrix3x3& mat);
    DirectX::SimpleMath::Vector3 Format(const aiVector3D& from);
    DirectX::SimpleMath::Vector2 Format(const aiVector2D& from);
    DirectX::SimpleMath::Quaternion Format(const aiQuaternion& pOrientation);
    DirectX::SimpleMath::Color Format(const aiColor4D& color);
    DirectX::SimpleMath::Ray Format(const aiRay& ray);
    DirectX::SimpleMath::Plane Format(const aiPlane& plane);
    std::string Format(const aiString& string);
};

class MakeLeftHandedProcess {
public:
    MakeLeftHandedProcess() = default;
    ~MakeLeftHandedProcess() = default;

    // -------------------------------------------------------------------
    bool isActive(unsigned int pFlags) const;

    // -------------------------------------------------------------------
    void execute(aiScene* pScene);

protected:
    // -------------------------------------------------------------------
    /** Recursively converts a node and all of its children
     */
    void processNode(aiNode* pNode, const aiMatrix4x4& pParentGlobalRotation);

    // -------------------------------------------------------------------
    /** Converts a single mesh to left handed coordinates.
     * This means that positions, normals and tangents are mirrored at
     * the local Z axis and the order of all faces are inverted.
     * @param pMesh The mesh to convert.
     */
    void processMesh(aiMesh* pMesh);

    // -------------------------------------------------------------------
    /** Converts a single material to left-handed coordinates
     * @param pMat Material to convert
     */
    void processMaterial(aiMaterial* pMat);

    // -------------------------------------------------------------------
    /** Converts the given animation to LH coordinates.
     * The rotation and translation keys are transformed, the scale keys
     * work in local space and can therefore be left untouched.
     * @param pAnim The bone animation to transform
     */
    void processAnimation(aiNodeAnim* pAnim);

    // -------------------------------------------------------------------
    /** Converts a single camera to left handed coordinates.
     * The camera viewing direction is inverted by reflecting mLookAt
     * across mPosition.
     * @param pCam The camera to convert
     */
    void processCamera(aiCamera* pCam);
};