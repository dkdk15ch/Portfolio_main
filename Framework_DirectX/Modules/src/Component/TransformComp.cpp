#include "Component/TransformComp.h"
#include "GameObject/GameBase.h"
#include "Animation/ModelNode.h"

namespace {
	using namespace DirectX::SimpleMath;
}

void TransformComp::start() {
	if (m_AttachTarget) {
		updateAttach();
		return;
	}
	updateMatrix();
}

void TransformComp::postUpdate(uint32_t frameIndex, double deltaTime)
{
	if (m_AttachTarget) {
		updateAttach();
		return;
	}
	updateMatrix();
}

DirectX::SimpleMath::Vector3 TransformComp::worldRot() const
{
	Vector3 scale;
	Quaternion rotation;
	Vector3 translation;
	auto tempMatrix = m_WorldMatrix;
	tempMatrix.Decompose(scale, rotation, translation);
	return rotation.ToEuler();
}

DirectX::SimpleMath::Quaternion TransformComp::worldQuat() const
{
	Vector3 scale;
	Quaternion rotation;
	Vector3 translation;
	auto tempMatrix = m_WorldMatrix;
	tempMatrix.Decompose(scale, rotation, translation);
	return rotation;
}

DirectX::SimpleMath::Vector3 TransformComp::worldScale() const
{
	Vector3 scale;
	const auto& m = m_WorldMatrix;
	scale.x = Vector3(m._11, m._12, m._13).Length();
	scale.y = Vector3(m._21, m._22, m._23).Length();
	scale.z = Vector3(m._31, m._32, m._33).Length();

	return scale;
}

float TransformComp::worldScaleX() const
{
	const auto& m = m_WorldMatrix;
	return Vector3(m._11, m._12, m._13).Length();
}

float TransformComp::worldScaleY() const
{
	const auto& m = m_WorldMatrix;
	return Vector3(m._21, m._22, m._23).Length();
}

float TransformComp::worldScaleZ() const
{
	const auto& m = m_WorldMatrix;
	return Vector3(m._31, m._32, m._33).Length();
}


DirectX::SimpleMath::Matrix TransformComp::calcMatrix() const
{
    return(
        Matrix::CreateScale(m_Scale) *
        Matrix::CreateRotationZ(m_Rotation.z) *
        Matrix::CreateRotationY(m_Rotation.y) *
        Matrix::CreateRotationX(m_Rotation.x) *
        Matrix::CreateTranslation(m_Position)
            );
}

void TransformComp::updateMatrix(const DirectX::SimpleMath::Matrix* parentWorldMatrix)
{
	Matrix localMat = calcMatrix();
	const auto p = owner().parent();

	if (parentWorldMatrix) {
		m_WorldMatrix = localMat * (*parentWorldMatrix);
	}
	else if (p) {
		const auto& parentT = *p->transform();
		m_WorldMatrix = localMat * parentT.m_WorldMatrix;
	}
	else {
		m_WorldMatrix = localMat;
	}

	const auto& children = owner().children();
	for (auto child : children) {
		if (child) {
			child->transform()->updateMatrix(&m_WorldMatrix);

		}
	}
}

void TransformComp::updateAttach()
{
	Matrix offsetMat = calcMatrix();
	auto targetWorld = m_AttachTarget->worldMatrix();
	//	回転やスケールの影響を受けるかチェック
	if (!isLocked(TransformLock::LocalAll)) {
		m_WorldMatrix = offsetMat * targetWorld;
		const auto p = owner().parent();
		if (p) {
			m_WorldMatrix = m_WorldMatrix * p->transform()->worldMatrix();
		}
		return;
	}
	
	Vector3 pos;
	Quaternion rot;
	Vector3 scale;
	targetWorld.Decompose(scale, rot, pos);
	
	if(isLocked(TransformLock::LocalRot)){
		rot = Quaternion::Identity;
	}
	if (isLocked(TransformLock::LocalScale)) {
		scale = Vector3::One;
	}
	Matrix targetFinal =
		Matrix::CreateScale(scale) *
		Matrix::CreateFromQuaternion(rot) *
		Matrix::CreateTranslation(pos);

	m_WorldMatrix = offsetMat * targetFinal;
	const auto p = owner().parent();
	if (p) {
		m_WorldMatrix = m_WorldMatrix * p->transform()->worldMatrix();
	}
}
