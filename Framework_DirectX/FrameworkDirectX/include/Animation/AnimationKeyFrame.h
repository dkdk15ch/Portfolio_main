#pragma once
#include <variant>
#include <SimpleMath.h>

class AnimationKeyFrame
{
public:
	friend class AnimationNode;
	friend class JsonImporter;

	AnimationKeyFrame();
	~AnimationKeyFrame() = default;
	bool operator<(const AnimationKeyFrame& v)const {
		return m_Time < v.m_Time;
	}

	void setPosition(const DirectX::SimpleMath::Vector3& pos) {
		m_Position = pos;
	}
	const DirectX::SimpleMath::Vector3& position() const {
		return m_Position;}

	void setRotation(const DirectX::SimpleMath::Quaternion& rotation) {
		m_Rotation = rotation;
	}
	const DirectX::SimpleMath::Quaternion& rotation() const {
		return m_Rotation;
	}


	void setScale(const DirectX::SimpleMath::Vector3& scale) {
		m_Scale = scale;
	}
	const DirectX::SimpleMath::Vector3& scale() const {
		return m_Scale;
	}

	double time() const { return m_Time; }
	double tick() const { return m_Tick; }

private:
	double m_Time;	// 再生時間からの経過時間
	double m_Tick;	//	再生フレーム数

	DirectX::SimpleMath::Vector3 m_Position;
	DirectX::SimpleMath::Quaternion m_Rotation;
	DirectX::SimpleMath::Vector3 m_Scale;

public:
#ifdef _DEBUG
	static constexpr const char* m_ClassName = "AnimationKeyFrame";
#endif
};

