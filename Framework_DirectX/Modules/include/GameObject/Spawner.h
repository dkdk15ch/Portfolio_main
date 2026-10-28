#pragma once
#include "GameObject/GameBase.h"
#include "Manager/LevelManager.h"
#include <vector>
#include <typeindex>
#include "Util/XMathHelper.h"

class Spawner : public GameBase {
	template <typename U, typename... Args>
	friend LogPtr<U> MAKE_UNIQUE_LOG(Args&&... args);
public:
	virtual ~Spawner() = default;

	template <typename T>
	bool isType() const {
		return m_Type == std::type_index(typeid(T));
	}
	template <typename T>
	static void create(std::string name, uint32_t maxNum);

	void spawn(DirectX::SimpleMath::Vector3 pos,
		DirectX::SimpleMath::Vector3 rot = { 0.f,0.f,0.f },
		DirectX::SimpleMath::Vector3 scale = { 1.f,1.f,1.f });
	void spawn(XMathHelper::EulerTRS trs);
	
	void despawn(GameBase* target);

private:
	Spawner(std::string name = "Spawner", uint32_t maxNum = 1);

	std::type_index m_Type = std::type_index(typeid(void));
	uint32_t m_MaxNum;
	uint32_t m_NextUseIndex = 0;
	std::vector<GameBase*> m_List;

};

template<typename T>
inline void Spawner::create(std::string name, uint32_t maxNum)
{
	auto spawner = MAKE_UNIQUE_LOG<Spawner>(name, maxNum);
	spawner->m_Type = std::type_index(typeid(T));

	auto& list = spawner->m_List;
	list = std::vector<GameBase*>(maxNum);
	std::vector<LogPtr<GameBase>> addObjects;
	for (auto& element : list) {
		auto obj = MAKE_UNIQUE_LOG<T>();
		obj->disable();
		obj->setSpawner(spawner.get());
		element = obj.get();
		addObjects.push_back(StaticCast_Unique<GameBase>(std::move(obj)));
	}

	auto levelMgr = LevelManager::m_Instance;
	levelMgr->requestAddObjects(std::move(addObjects));
	levelMgr->requestAddObject(std::move(spawner));
	
}
