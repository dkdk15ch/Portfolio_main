#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include "StringHelper.h"
#include <Interface/IAsset.h>

class AnimationNode;
class Model;
struct aiAnimation;
struct aiNode; 
class IAssetAccessor;

class Animation : public IAsset
{
	friend class Animator;
	friend class JsonImporter;
public:
	Animation();
	virtual ~Animation();

	/**
	 * @brief 
	 * 名前違うと失敗、エクスポート時に同じにしておく必用あり
	 * @param model 
	 * @param anima 
	 * @param root 
	 * @param fileName 
	 */
	void init(Model* model, aiAnimation* anima, aiNode* root, const std::string& fileName);
	double calcTime(double time) const;
	bool isEndTime(double time);

	void setName(const std::string& name) { m_Name = name; }
	std::string& name() { return m_Name; }
	void setModelName(const std::string& name) { m_ModelName = name; }
	std::string& modelName() { return m_ModelName; }
	AssetType type() override { return AssetType::ANIMATION; }

	std::unordered_map<std::string, AnimationNode*>& nodeMap(){ return m_NodeMap; }
	std::vector<AnimationNode*> nodeList();

	void setTimeScale(double newScale) { m_TimeScale = newScale; }
	double timeScale() const { return m_TimeScale; }	
	void setIsLoop(bool newValue) { m_BIsLoop = newValue; }
	bool isLoop()const { return m_BIsLoop; }

private:
	std::string m_Name;
	std::string m_ModelName;

	std::unordered_map<std::string, AnimationNode*> m_NodeMap;
	std::vector <LogPtr<AnimationNode>> m_NodeList;

	double m_TicksPerSecond;
	double m_Duration;
	double m_TotalTime;
	double m_TimeScale = 1.0;
	bool m_BIsLoop = false;

public:
#ifdef _DEBUG
	static constexpr const char* m_ClassName = "Animation";
#endif
};

