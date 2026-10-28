#pragma once
#include <SimpleMath.h>
#include <vector>
#include <string>
#include <algorithm>

class AnimationKeyFrame;
class Animation;
struct aiAnimation;
struct aiNodeAnim;
class ModelNode;

class AnimationNode
{
public:
	friend class Animation;
	friend class JsonImporter;

	AnimationNode();
	~AnimationNode();

	void init(Animation* animation);

	/**
	 * @brief		指定の再生時間から前後のキーを取得
	 * @param time	再生時間
	 * @return		前後ペアで返す
	 */
	std::tuple<const AnimationKeyFrame&, const AnimationKeyFrame&> findNearestKeys(double time);
	
	void setName(const std::string& name) { m_Name = name;}
	std::string& name() { return m_Name; }

	void setRoot(bool value) { m_BIsRoot = value; }
	bool isRoot() const { return m_BIsRoot; }

	bool bIsStartPoseOnly()const { return m_BIsStartPoseOnly; }

private:
	void setKeyFrames(aiAnimation* anim, aiNodeAnim* animNode, uint32_t maxTick,double tickPerSec);
	void setKeyTransform(aiAnimation* anim, aiNodeAnim* animNode, uint32_t MaxTick, double tickPerSec);
	void checkMoveNode();
	void checkStartPoseOnly();
	static int findKeyFrameIndex(const std::vector<AnimationKeyFrame>& keyFrames, const double& time);
	
	std::string m_Name;
	bool m_BIsRoot = false;
	bool m_BIsStartPoseOnly = false;
	std::vector<AnimationKeyFrame> m_KeyFrames;
	Animation* m_Animation;

public:
#ifdef _DEBUG
	static constexpr const char* m_ClassName = "AnimationNode";
#endif
};

