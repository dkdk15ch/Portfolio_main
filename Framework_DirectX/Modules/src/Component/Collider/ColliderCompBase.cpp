#include "Component/Collider/ColliderCompBase.h"
#include "Component/TransformComp.h"
#include "GameObject/GameBase.h"
#include "Component/MovementComp.h"
#include "Manager/ColliderManager.h"
#ifdef _DEBUG
#include "Component/Render/RenderCompBase.h"
#include "GraphicContext.h"
#include "StringHelper.h"
#include "Util/Loader.h"
#include "ConstantBuffer.h"
#endif // _DEBUG

ColliderCompBase::ColliderCompBase(GameBase* owner)
	: ComponentBase(owner)
	, m_SelfLayer(CollisionTag::disable)
	, m_TargetLayers{}
	, m_Offset(DirectX::SimpleMath::Matrix::Identity)
{
	auto& colliderMgr = ColliderManager::m_Instance;
	colliderMgr->add(owner->name(), this);
}

bool ColliderCompBase::detect(
	ColliderCompBase& other, 
	uint32_t frameIndex, 
	HitInfo& outputInfo,
	int InstanceIndex)
{
	if (shape() == Shape::OBB || 
		other.shape() == Shape::OBB )
	{
		//detectOrient(other, frameIndex, InstanceIndex);
	}

    return	detectAligned(other, frameIndex, outputInfo, InstanceIndex);
}

bool ColliderCompBase::detectRay(
	const RayDesc& ray, 
	HitInfo& info, 
	uint32_t instIdx, 
	uint32_t frameIndex)
{
	return intersectRay(ray, info, instIdx, frameIndex);
}

void ColliderCompBase::callOnHit(const HitInfo& other)
{
	owner().onHit(other);
}

void ColliderCompBase::setOffset(DirectX::SimpleMath::Vector3 pos,
	DirectX::SimpleMath::Vector3 rot,
	DirectX::SimpleMath::Vector3 scale) {
	m_Offset = DirectX::SimpleMath::Matrix::CreateScale(scale) *
		DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(rot.y, rot.x, rot.z) *
		DirectX::SimpleMath::Matrix::CreateTranslation(pos);
}

