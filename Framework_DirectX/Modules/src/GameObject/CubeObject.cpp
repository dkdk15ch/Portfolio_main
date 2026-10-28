#include "GameObject/CubeObject.h"
#include "Component/Render/RenderCompBase.h"
#include "GameObject/ObjectParam.h"
#include "Util/Loader.h"

CubeObject::CubeObject(std::string uniqueName, uint32_t instanceNum, const std::string jsonActorKey)
	: GameBase(std::move(makeUniqueName(uniqueName))
		, instanceNum, jsonActorKey)
{
	const auto p = m_Param;
	addComponent(MAKE_UNIQUE_LOG<RenderCompBase>(this));
	m_Render = static_cast<RenderCompBase*>(getComponent(CompType::RenderComp));
	m_Render->setCubeMesh();

	if (p) {
		initStaticTransform(
			p->startPos(), p->startRot(), p->startScale());
		Loader::texture(
			p->textPath().c_str(),
			m_Render->material(),
			StringToWString(name()),
			DirectX::XMFLOAT2(
				static_cast<float>(p->tilingX()), 
				static_cast<float>(p->tilingY()))
		);
		return;
	}

	Loader::texture(
		L"",
		m_Render->material(),
		StringToWString(name()),
		DirectX::XMFLOAT2(1.f, 1.f),
		true
	);

}
