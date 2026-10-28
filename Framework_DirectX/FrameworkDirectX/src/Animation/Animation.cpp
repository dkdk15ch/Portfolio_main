#include "Animation/Animation.h"
#include "Animation/convert_assimp.h"
#include "Animation/AnimationNode.h"
#include "Animation/ModelNode.h"
#include "assimp/scene.h"
#include <cmath>
#include "Animation/Model.h"
#include "Logger.h"
#include "StringHelper.h"
#include "PointerHelper.h"
#include "Animation/IAssetAccessor.h"
#include <iostream>
Animation::Animation()
	: m_Duration(0.0)
	, m_TicksPerSecond(30)
	, m_TotalTime(0.0)
{
}

Animation::~Animation()
{
	m_NodeList.clear();
}

void Animation::init(Model* model, aiAnimation* anima, aiNode* root, const std::string& fileName)
{
	assert(m_PAssetManager && "Animation::init - m_pAssetAccessor null");
	if (!model || !anima || !root || !model->m_RootBoneNode) {
		ELOG("Error Animation::init argsList nullptr");
		return;
	}

	setModelName(model->name());
	auto name = convert_assimp::Format(anima->mName);
	if (name.empty() || name == "Scene") {
		name = m_ModelName + "|" + fileName;
	}
	setName(name);
	//	時間関連
	m_Duration = anima->mDuration;
	m_TicksPerSecond = (anima->mTicksPerSecond != 0) ?
		anima->mTicksPerSecond : 25.0;
	m_TotalTime = m_Duration / m_TicksPerSecond;
	//	チャンネルごとにAnimationNodeに変換
	for (u_int i = 0; i < anima->mNumChannels; i++) {
		auto name = convert_assimp::Format(anima->mChannels[i]->mNodeName);
		std::string originalName = name;
		if (originalName.empty() || originalName == "") continue;
		//	対応するボーンを探して、アニメーションノードの追加
		auto it = m_NodeMap.find(originalName);
		AnimationNode* part;
		if(it == m_NodeMap.end()){
			auto p = MAKE_UNIQUE_LOG<AnimationNode>();
			p->setName(originalName);
			p->init(this);
			m_NodeMap[originalName] = p.get();
			part = p.get();
			m_NodeList.push_back(std::move(p));
		}
		else {
			part = it->second;
		}		
		part->setKeyFrames(anima, anima->mChannels[i], static_cast<uint32_t>(m_Duration) ,m_TicksPerSecond);
	}
}

double Animation::calcTime(double time) const
{
	if (m_TotalTime == 0) {
		return 0;
	}

	if (m_BIsLoop) {
		return std::fmod(time, m_TotalTime);
	}

	return std::clamp(time, 0.0, m_TotalTime);
}

bool Animation::isEndTime(double time)
{
	return (!m_BIsLoop) && m_TotalTime <= time;
}

std::vector<AnimationNode*> Animation::nodeList()
{
	return GetPointers(m_NodeList);
}
