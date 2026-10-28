#pragma once
#include "Component/ComponentBase.h"
#include "GameObject/GameBase.h"
#include <SimpleMath.h>
#include <DirectXMath.h>
class ObjectParam;
class ILimitHitResponse;

class MovementComp : public ComponentBase
{
	friend ILimitHitResponse;
public:
	MovementComp(GameBase* owner);

	virtual ~MovementComp() = default;
	CompType compType() override final { return CompType::MovementComp; }

	virtual void init() override;
	void preUpdate(uint32_t frameIndex, double deltaTime) override;
	virtual void postUpdate(uint32_t frameIndex, double deltaTime) override;


#pragma region Pos/Rot/Scale
	void setTransformFromParam();
	void setTransform(
		DirectX::SimpleMath::Vector3 pos,
		DirectX::SimpleMath::Vector3 rot,
		DirectX::SimpleMath::Vector3 scale);

	void setPos(DirectX::SimpleMath::Vector3 pos);
	void addPos(DirectX::SimpleMath::Vector3 pos);
	void setPosX(float x);
	void setPosY(float y);
	void setPosZ(float z);
	void addPosX(float x);
	void addPosY(float y);
	void addPosZ(float z);
	void mulPosX(float x);
	void mulPosY(float y);
	void mulPosZ(float z);
	

	void setRot(DirectX::SimpleMath::Vector3 rot);
	void addRot(DirectX::SimpleMath::Vector3 rot);
	void setRotX(float x);
	void setRotY(float y);
	void setRotZ(float z);
	void addRotX(float x);
	void addRotY(float y);
	void addRotZ(float z);
	void mulRotX(float x);
	void mulRotY(float y);
	void mulRotZ(float z);


	void setScale(DirectX::SimpleMath::Vector3 scale);
	void setUniformScale(float scale);
	void addScale(DirectX::SimpleMath::Vector3 scale);
	void setScaleX(float x);
	void setScaleY(float y);
	void setScaleZ(float z);
	void addScaleX(float x);
	void addScaleY(float y);
	void addScaleZ(float z);
	void mulScaleX(float x);
	void mulScaleY(float y);
	void mulScaleZ(float z);
#pragma endregion

#pragma region Physic
	const DirectX::SimpleMath::Vector3& velocity()const {
		return m_Velocity;	}
	void setVelo(DirectX::SimpleMath::Vector3 v) {
		m_Velocity = v; }
	void setVeloZero() {
		m_Velocity = DirectX::SimpleMath::Vector3();
	}
	void setReverseVelo() {
		m_Velocity *= -1.f;
	}
	void setVeloX(float v) {
		m_Velocity.x = v; }
	void setVeloY(float v) {
		m_Velocity.y = v; }
	void setVeloZ(float v) {
		m_Velocity.z = v; }
	float speed()const {
		return m_Velocity.Length();	}
	DirectX::SimpleMath::Vector3 direction()const;
	void setGrabity(bool value) { m_BIsEnableGravity = value; }
#pragma endregion

#pragma region getter・setter
	bool isOnGround()const { return m_BIsOnGround; }
	void setLimitResponse(LogPtr<ILimitHitResponse> newRes) {
		if (!newRes) { return; }
		m_LimitResponse = std::move(newRes); 
	}
#pragma endregion

	void lookAt(DirectX::SimpleMath::Vector3 target);

protected:
	TransformComp* m_Target;
	const ObjectParam* m_Param;
	LogPtr<ILimitHitResponse> m_LimitResponse;

	DirectX::SimpleMath::Vector3 m_Velocity;
	bool m_BIsOnGround;
	bool m_BHasMovementLimit;
private:
	void messageIsLocked();
	float m_Gravity = 9.81f;
	bool m_BIsEnableGravity = true;
};

class ILimitHitResponse {
	friend MovementComp;
public:
	ILimitHitResponse(MovementComp* mover) : m_Mover(mover) {}
	~ILimitHitResponse() = default;

protected:
	enum class Type {
		LEFT,
		TOP,
		RIGHT,
		BOTTOM,
		FRONT,
		BACK,
	};
	virtual void onHit() = 0;
	MovementComp* m_Mover;
	const ObjectParam* param()const { return m_Mover->m_Param; }
	const DirectX::SimpleMath::Vector3& posision()const { return m_Mover->m_Target->position(); }
	void setOnGround(bool bOnGround) { m_Mover->m_BIsOnGround = bOnGround; }
	DirectX::SimpleMath::Vector3 velocity() const { return m_Mover->m_Velocity; }
};

namespace LimitHitResponse {
	class Stop : public ILimitHitResponse {
		public:
			using ILimitHitResponse::ILimitHitResponse;
		protected:
			void onHit() override final;
	};
	class Reflection : public ILimitHitResponse {
		public:
			using ILimitHitResponse::ILimitHitResponse;
		protected:
			void onHit() override final;
	};
	
}

