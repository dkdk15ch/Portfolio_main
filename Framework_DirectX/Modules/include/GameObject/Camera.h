#pragma once
#include <vector>
#include <memory>
#include <SimpleMath.h>

class GameBase;

class Camera
{
public:
	struct Desc {
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector3 target;
		DirectX::SimpleMath::Vector3 up;
		float fov;
		float aspect;
		float nearZ;
		float farZ;
		/**
		 * @brief camera設定構造体
		 *			fovはdegree
		 */
		Desc() : 
			position(0.0f, 1000.0f, -800.0f),
			target(0.0f, 0.0f, 100.0f),
			up(DirectX::SimpleMath::Vector3::UnitY),
			fov(37.5f),
			aspect(0.0f),
			nearZ(1.0f),
			farZ(3000.0f){ }
		~Desc() {};
	};

	Camera(Desc desc);
	~Camera();


	void update(uint32_t frameIndex, double deltaTime);

	void follow(const GameBase& target);
	void setFollowOffset(DirectX::SimpleMath::Vector3 offset) { m_FollowOffset = offset; }


	DirectX::SimpleMath::Matrix getView() const;
	DirectX::SimpleMath::Matrix getProj() const;

#pragma region getter
	const DirectX::SimpleMath::Vector3& position() const { return m_Position; }
	const DirectX::SimpleMath::Vector3& target() const { return m_Target; }
	const DirectX::SimpleMath::Vector3& up() const { return m_Up; }
	const DirectX::SimpleMath::Vector3& followOffset() const { return m_FollowOffset; }

#pragma endregion


private:
	DirectX::SimpleMath::Vector3 m_Position;
	DirectX::SimpleMath::Vector3 m_Target;
	DirectX::SimpleMath::Vector3 m_Up;
	float m_Fov;
	float m_Aspect;
	float m_NearZ;
	float m_FarZ;
	float m_Yaw;
	float m_Pitch;
	void calcTarget();
	DirectX::SimpleMath::Vector3 m_FollowOffset;
};

