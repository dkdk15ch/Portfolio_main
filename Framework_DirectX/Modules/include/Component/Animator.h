#pragma once
#include <memory>
#include <string>
#include "ComponentBase.h"
#include <vector>
#include <SimpleMath.h>
#include <Interface/IAsset.h>
#include <PointerHelper.h>

class GameBase;
class ConstantBuffer;
class Model;
class Bone;
class ModelNode;
class ModelRenderComp;
class AnimationNode;
class Animation;
struct FlipMode;



class Animator : public ComponentBase, public IAsset
{
	friend ModelRenderComp;
public:
	Animator(GameBase* owner);

	CompType compType() override final { return CompType::Animator; }
	
	//void Start()override final;
	void preUpdate(uint32_t frameIndex, double deltaTime)	override final;
	void update(uint32_t frameIndex, double deltaTime)	override final;
	void postUpdate(uint32_t frameIndex, double deltaTime)	override final;
	virtual void preRender(const uint32_t frameIndex) override final;
	void term() override final;
	

	void setNextAnimation(const std::string& key);
	void setNextAnimation(Animation* next) { m_NextAnimation = next; }
	Animation* currentAnimation()const { return m_CurrentAnimation; }
	uint32_t currentFrame()const { return m_CurrentFrame; }
	
	/**
	 * @brief		中身だけコピーして所有する
	 * @param model
	 */
	void setModel(const std::string& targetKey);
	std::vector<ModelNode*> cloneNodes() {
		return GetPointers(m_CloneNode);}
	ModelNode* findCloneNode(const std::string& name);
	std::vector<Bone*> cloneBones() {
		return m_RefcloneBoneList; }
	
	const std::string& meshName();
	Mesh* mesh();
	ConstantBuffer* boneMatrixBuffer(const uint32_t frameIndex)const {
		return m_BoneMatrixBuffer[frameIndex].get();}
	Model* targetModel() { return m_TargetModel; }
	//	時間関連
	double duration()const;
	bool isPlaying()const;
	double timeScale()const { return m_TimeScale; }
	void setTimeScale(double value) { m_TimeScale = value; }
	//	反転再生用
	void setFlipX(bool bIsEnabel);

private:
	void updateAnimation(double deltaTime);
	void updateMatrices();
	
	Animation* m_CurrentAnimation;
	Animation* m_NextAnimation;
	std::vector<LogPtr<ConstantBuffer>> m_BoneMatrixBuffer;

	Model* m_TargetModel;
	std::vector<LogPtr<Bone>> m_cloneBoneList;
	std::vector<Bone*> m_RefcloneBoneList;
	uint32_t ROOT_BONE_INDEX;
	std::vector<LogPtr<ModelNode>> m_CloneNode;

	double m_AnimationTime;
	uint32_t m_CurrentFrame;
	double m_Duration;
	double m_TimeScale;
	LogPtr<FlipMode>m_Mode;
};

