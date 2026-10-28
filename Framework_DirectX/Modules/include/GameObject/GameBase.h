#pragma once
#include <string>
#include <PointerHelper.h>
#include "Component/ComponentType.h"
#include "Component/TransformComp.h"
#include <d3d12.h>
#include "Tag.h"

class Camera;
class ComponentBase;
class ObjectParam;
class RenderCompBase;
class ColliderCompBase;
class Spawner;
struct HitInfo;
namespace CB {
	struct InstanceParamBuffer;
}

struct InstanceData {
	TransformComp* m_Transform;
	DirectX::XMVECTOR m_Color;
};

struct PrimitiveObjDesc {
	PrimitiveObjDesc();

	std::string paramKey;
	bool hasRender;
	uint32_t instanceNum;
	std::string psoKey;
	bool hasCollider;
	CollisionTag self;
	uint32_t targetLayer;
	DirectX::SimpleMath::Vector3 ColliderOffsetT;
	DirectX::SimpleMath::Vector3 ColliderOffsetR;
	DirectX::SimpleMath::Vector3 ColliderOffsetS;
	bool hasMover;
};

class GameBase
{
public:
	GameBase(std::string name , uint32_t instanceNum = 1, const std::string jsonActorKey = "");
	virtual ~GameBase();

	virtual void term();
	virtual void restart();

#pragma region Frame Lifecycle(public)
	//	フレーム間での呼び出し関数
	//	更新->コリジョン応答->描画の順
	//	更新
	void preUpdate(uint32_t frameIndex, double deltaTime);
	void start(); // 初回のみ preUpdate後　各オブジェクトの生成後初回
	void update(uint32_t frameIndex, double deltaTime);
	void postUpdate(uint32_t frameIndex, double deltaTime);
	void onHit(const HitInfo& info);
	void preRender(const uint32_t frameIndex);
	//// renderはPSO毎に実行するので、RenderManagerで行う
	void postRender(ID3D12GraphicsCommandList* pCmdList, const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer);
#pragma endregion


#pragma region Component
	void addComponent(LogPtr<ComponentBase> component);
	bool removeComponent(CompType type);
	ComponentBase* getComponent(CompType type)const;

	template <typename T>
	T* getComponent(CompType type)const {
		if (auto comp = getComponent(type)) {
			return dynamic_cast<T*>(comp);
		}
		assert(false);
		return nullptr;
	}
#pragma endregion

#pragma region InstanceData
	//	複数描画しない場合は配列の先頭のみ使用
	virtual const std::vector<InstanceData>& instanceDataList()const;
	const TransformComp* transform(uint32_t instIdx = 0)const { return m_InstanceData[instIdx].m_Transform; }
	TransformComp* transform(uint32_t instIdx = 0) { return m_InstanceData[instIdx].m_Transform; }
	const DirectX::XMVECTOR color(uint32_t instIdx = 0)const { return m_InstanceData[instIdx].m_Color; }
	void setColor(DirectX::XMVECTOR color, uint32_t instIdx = 0) {
		m_InstanceData[instIdx].m_Color = color;}
	void setInstanceNum(uint32_t num) { m_InstanceNum = num; }
	uint32_t instanceNum()const { return m_InstanceNum; }
#pragma endregion

#pragma region Parameter
	const ObjectParam* param()const { return m_Param; }
	void setParam(const ObjectParam* p);
	virtual CB::InstanceParamBuffer* getInstanceBuffer(uint32_t frameIndex);
#pragma endregion


#pragma region Setter/Getter
	const std::string& name()const { return m_Name; }

	/**
	 * @brief			旧親を解除して、親のセット
	 *					nullptrを渡すと解除のみ行う
	 */
	void setParent(GameBase* parent);
	GameBase* parent()const { return m_Parent; }
	const std::vector<GameBase*>& children()const {
		return m_Children;
	}

	bool isEnable()const { return m_BIsEnable; }
	virtual void enable();
	virtual void disable();
	void disableNextFrame();
	void enableNextFrame();

	void setSpawner(Spawner* spawner) { m_Spawner = spawner;}

	void setSlomotion(double slowSclae) { m_TimeScale = slowSclae; };
	void resetSlomotion() { m_TimeScale = 1.0; };

#pragma endregion

	/**
 * @brief	Static用のTransform初期化
 *			Mover取り除かれるので非Staticでは×
 */
	void initStaticTransform(
		DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 rot, DirectX::SimpleMath::Vector3 scale);

protected:
#pragma region Frame Lifecycle(imple)
	virtual void implPreupdate(uint32_t frameIndex, double deltaTime);
	virtual void implStart();
	virtual void implupdate(uint32_t frameIndex, double deltaTime);
	virtual void implPostupdate(uint32_t frameIndex, double deltaTime);
	virtual void implOnHit(const HitInfo& info) {};
	virtual void implPreRender(const uint32_t frameIndex);
	//// renderはRenderCompから実行 ////
	virtual void implPostRender(ID3D12GraphicsCommandList* pCmdList,
		const uint32_t frameIndex, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer);
#pragma endregion

	std::string m_Name;
	double m_TimeScale;
	const ObjectParam* m_Param;
	std::vector<LogPtr<ComponentBase>> m_Components;
	std::vector<InstanceData> m_InstanceData;
	Spawner* m_Spawner;

private:
	bool m_BIsEnable = true;
	bool m_BIsDisableNextFrame = false;
	uint32_t m_InstanceNum;
	bool bIsStarted = false;
	
	GameBase* m_Parent;
	std::vector<GameBase*> m_Children;
};


