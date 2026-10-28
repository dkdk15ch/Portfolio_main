#define NOMINMAX
#include "Animation/AnimationNode.h"
#include "Animation/AnimationKeyFrame.h"
#include "assimp/scene.h"
#include "Util/XMathHelper.h"

#include <iostream>

AnimationNode::AnimationNode() 
	: m_Animation(nullptr)
	, m_BIsStartPoseOnly(false)
{
}

AnimationNode::~AnimationNode()
{
}

void AnimationNode::init(Animation* animation)
{
	assert(animation && "Animation::init");
	m_Animation = animation;
}

std::tuple<const AnimationKeyFrame&, const AnimationKeyFrame&> AnimationNode::findNearestKeys(double time)
{
	if (m_KeyFrames.empty()) {
		static const AnimationKeyFrame emptyKey{};
		return { emptyKey, emptyKey };
	}
	AnimationKeyFrame refValue{};
	refValue.m_Time = time;

	auto last = std::upper_bound(
		m_KeyFrames.begin(),
		m_KeyFrames.end(),
		refValue,
		[](const AnimationKeyFrame& a, const AnimationKeyFrame& b) {
			return a.m_Time < b.m_Time;
		}
	);

	if (last == m_KeyFrames.begin()) {
		return { m_KeyFrames.front(), m_KeyFrames.front() };
	}
	auto first = last - 1;
	if (last == m_KeyFrames.end()) {
		last = first;
	}

	return { *first, *last };
}

#pragma region Setter

void AnimationNode::setKeyFrames(aiAnimation* anim, aiNodeAnim* animNode, uint32_t maxTick, double tickPerSec)
{
	setKeyTransform(anim, animNode, maxTick, tickPerSec);
	checkMoveNode();
	checkStartPoseOnly();
}

void AnimationNode::setKeyTransform(aiAnimation* anim, aiNodeAnim* animNode, uint32_t MaxTick, double tickPerSec)
{
	std::vector<AnimationKeyFrame> temp(MaxTick);
	std::vector<bool> bUsed(MaxTick);
	const auto& posFrames = animNode->mPositionKeys;
	const auto& rotFrames = animNode->mRotationKeys;
	const auto& scaleFrames = animNode->mScalingKeys;


	for (uint32_t i = 0; i < MaxTick; ++i) {
		temp[i].m_Tick = i;
		temp[i].m_Time = i / tickPerSec;
	}

	for (uint32_t i = 0; i < animNode->mNumPositionKeys; i++) {
		auto tick = static_cast<uint32_t>(animNode->mPositionKeys[i].mTime);
		if (tick < MaxTick) {
			auto pos = DirectX::SimpleMath::Vector3(
				animNode->mPositionKeys[i].mValue.x,
				animNode->mPositionKeys[i].mValue.y,
				animNode->mPositionKeys[i].mValue.z);
			temp[tick].setPosition(pos);
			bUsed[tick] = true;
		}
	}

	for (uint32_t i = 0; i < animNode->mNumRotationKeys; i++) {
		auto tick = static_cast<uint32_t>(animNode->mRotationKeys[i].mTime);
		if (tick < MaxTick) {
			auto rot = DirectX::SimpleMath::Quaternion(
				animNode->mRotationKeys[i].mValue.x,
				animNode->mRotationKeys[i].mValue.y,
				animNode->mRotationKeys[i].mValue.z,
				animNode->mRotationKeys[i].mValue.w
			);
			temp[tick].setRotation(rot);
			bUsed[tick] = true;
		}
	}

	for (uint32_t i = 0; i < animNode->mNumScalingKeys; i++) {
		auto tick = static_cast<uint32_t>(animNode->mScalingKeys[i].mTime);
		if (tick < MaxTick) {
			auto scale = DirectX::SimpleMath::Vector3(
				animNode->mScalingKeys[i].mValue.x,
				animNode->mScalingKeys[i].mValue.y,
				animNode->mScalingKeys[i].mValue.z);
			temp[tick].setScale(scale);
			bUsed[tick] = true;
		}
	}
	//	有効なキーフレームだけ追加
	m_KeyFrames.clear();
	m_KeyFrames.reserve(MaxTick);

	for (int i = 0; i < temp.size(); ++i) {
		if (!bUsed[i]) { continue; }
		m_KeyFrames.push_back(temp[i]);
	}

	m_KeyFrames.shrink_to_fit();

#ifdef _DEBUG
	std::string nodeName = animNode->mNodeName.C_Str();;
	std::cout << nodeName << " : NumKey = " << m_KeyFrames.size() << std::endl;
	std::cout << "aiNodeNum /Pos : " << animNode->mNumPositionKeys;
	std::cout << "/Rot : " << animNode->mNumRotationKeys;
	std::cout << "/Scale : " << animNode->mNumScalingKeys << std::endl;
#endif // _DEBUG

}

#pragma endregion

/**
 * @brief 最初と最後の座標から動きがあるかチェック
 */
void AnimationNode::checkMoveNode()
{
	if (m_KeyFrames.size() == 0) return;
	
	const float MOVE_THRESHOLD = 0.1f;
	const auto firstPos = m_KeyFrames[0].position();
	const auto lastIndex = m_KeyFrames.size() - 1;
	const auto lastPos = m_KeyFrames[lastIndex].position();

	if ((firstPos - lastPos).Length() > MOVE_THRESHOLD) {
		m_BIsRoot = true;
	}
}

void AnimationNode::checkStartPoseOnly()
{
	using namespace XMathHelper;
	//	キーが開始・終了のみ　＆　同じ　場合フラグを立てる
	if (m_KeyFrames.size() != 2) { return; }
	const auto& sPos = m_KeyFrames[0].position();
	const auto& ePos = m_KeyFrames[1].position();
	if(!IsNearlyEqual(sPos, ePos)) { return; }
	const auto& sRot = m_KeyFrames[0].rotation();
	const auto& eRot = m_KeyFrames[1].rotation();
	if (!IsNearlyEqual(sRot, eRot)) { return; }
	const auto& sScale = m_KeyFrames[0].scale();
	const auto& eScale = m_KeyFrames[1].scale();
	if (!IsNearlyEqual(sScale, eScale)) { return; }

	m_BIsStartPoseOnly = true;
}

int AnimationNode::findKeyFrameIndex(const std::vector<AnimationKeyFrame>& keyFrames, const double& time)
{
	if (keyFrames.empty())
		return -1;

	auto it = std::lower_bound(keyFrames.begin(), keyFrames.end(), time,
		[&](const AnimationKeyFrame& key, const double& t) {
			return key.m_Time < t;
		});

	if (it == keyFrames.begin())
		return -1;

	return static_cast<int>(std::distance(keyFrames.begin(), it)) - 1;
}
