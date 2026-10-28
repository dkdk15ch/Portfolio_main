#include "GameObject/PlaneObject.h"
#include "Component/Render/RenderCompBase.h"
#include "Component/MovementComp.h"
#include "Component/Collider/BoxColliderComp.h"

#include "GameObject/ObjectParam.h"
#include "Util/Loader.h"

	const std::string baseName = "Plane";

PlaneObject::PlaneObject(
	const PrimitiveObjDesc& desc, std::string uniqueName)
	: GameBase(std::move(makeUniqueName(uniqueName))
	,desc.instanceNum
	, desc.paramKey)
{
	const auto p = m_Param;
	
	if (desc.hasRender) {
		if (p) {
			addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(this, desc.psoKey));
			m_Render = static_cast<RenderCompBase*>(getComponent(CompType::RenderComp));
			m_Render->setPlaneMesh();
			Loader::texture(
				p->textPath().c_str(),
				m_Render->material(),
				StringToWString(uniqueName),
				DirectX::XMFLOAT2(
					static_cast<float>(p->tilingX()),
					static_cast<float>(p->tilingY()))
			);
		}
		else {
			Loader::texture(
				L"",
				m_Render->material(),
				StringToWString(uniqueName),
				DirectX::XMFLOAT2(1.f, 1.f),
				true
			);
		}
	}

	if (p) {
		if (desc.hasMover) {
			addComponent(MAKE_UNIQUE_LOG<MovementComp>(this));
			auto mover = static_cast<MovementComp*>(getComponent(CompType::MovementComp));
			mover->setTransform(p->startPos(), p->startRot(), p->startScale());
		}
		else {
			initStaticTransform(p->startPos(), p->startRot(), p->startScale());
		}
	}

	if (desc.hasCollider) {
		addComponent(MAKE_UNIQUE_LOG<BoxColliderComp>(this, ColliderCompBase::Shape::AABB));
		auto collider = static_cast<BoxColliderComp*>(getComponent(CompType::ColliderComp));
		collider->setOffset(desc.ColliderOffsetT, desc.ColliderOffsetR, desc.ColliderOffsetS);
		collider->show(DirectX::Colors::Green);
		collider->setLayer(desc.self);
		collider->setTargetLayer(desc.targetLayer);
	}




}

PlaneObject::~PlaneObject()
{
}
