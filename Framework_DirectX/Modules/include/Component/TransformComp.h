#pragma once
#include "Component/ComponentBase.h"
#include "Util/XMathHelper.h"

class MovementComp;
class ModelNode;

enum class TransformLock : uint16_t {
	None = 0,
	PosX = 1 << 0,
	PosY = 1 << 1,
	PosZ = 1 << 2,
	PosAll = PosX | PosY | PosZ,
	RotX = 1 << 3,
	RotY = 1 << 4,
	RotZ = 1 << 5,
	RotAll = RotX | RotY | RotZ,
	ScaleX = 1 << 6,
	ScaleY = 1 << 7,
	ScaleZ = 1 << 8,
	ScaleAll = ScaleX | ScaleY | ScaleZ,
	//	attach local Rock
	LocalRot = 1 << 9,
	LocalScale = 1 << 10,
	LocalAll = LocalRot | LocalScale,
};

class TransformComp : public ComponentBase
{
public:
	friend MovementComp;

	TransformComp(GameBase* owner)
		: ComponentBase(owner)
		, m_Position{ 0.0f,0.0f,0.0f }
		, m_Rotation{ 0.0f,0.0f,0.0f }
		, m_Scale{ 1.0f,1.0f,1.0f }
		, m_LockTransform{static_cast<uint16_t>(TransformLock::None)}
		, m_WorldMatrix{}
		, m_AttachTarget(nullptr)
	{
	}
	virtual ~TransformComp() = default;

	CompType compType() override final {
		return CompType::TransformComp;
	}

	void start() override final;
	void postUpdate(uint32_t frameIndex, double deltaTime)override final;

	//	ローカル
	const DirectX::SimpleMath::Vector3& position() const { return m_Position; }
	const DirectX::SimpleMath::Vector3& rotation() const { return m_Rotation; }
	const DirectX::SimpleMath::Vector3& scale()    const { return m_Scale; }
	XMathHelper::EulerTRS eulerTRS()const {
		XMathHelper::EulerTRS trs;
		trs.translation = m_Position;
		trs.rot = m_Rotation;
		trs.scale = m_Scale;
		return trs;
	}
	//	ワールド
	DirectX::SimpleMath::Vector3 worldPos() const { return m_WorldMatrix.Translation(); }
	DirectX::SimpleMath::Vector3 worldRot() const;
	DirectX::SimpleMath::Quaternion worldQuat() const;
	DirectX::SimpleMath::Vector3 worldScale() const;
	float worldScaleX() const;
	float worldScaleY() const;
	float worldScaleZ() const;

	
	//	ロック用メソッド
	void lock(TransformLock target) {
		m_LockTransform |= static_cast<uint16_t>(target);
	}
	void unLock(TransformLock target) {
		m_LockTransform &= ~static_cast<uint16_t>(target);
	}
	bool isLocked(TransformLock target)const {
		return (m_LockTransform & static_cast<uint16_t>(target));
	}

	void attachNode(const ModelNode* target) { m_AttachTarget = target; }

	DirectX::SimpleMath::Matrix calcMatrix()const;

	/**
	 * @brief 子オブジェクトを含むマトリックス更新
	 * @param parentWorldMatrix 基本的にひきすうなしで使う。再帰的に呼び出す用。 
	 */
	void updateMatrix(const DirectX::SimpleMath::Matrix* parentWorldMatrix = nullptr);
	DirectX::SimpleMath::Matrix worldMatrix()const { return m_WorldMatrix; }

private:
	//	親あり=>ローカル , 親無し=>ワールド
	DirectX::SimpleMath::Vector3 m_Position;
	DirectX::SimpleMath::Vector3 m_Rotation;
	DirectX::SimpleMath::Vector3 m_Scale;
	uint16_t m_LockTransform;
	//	アタッチ先
	const ModelNode* m_AttachTarget;

	//	ワールド
	DirectX::SimpleMath::Matrix m_WorldMatrix;

	void updateAttach();
};

