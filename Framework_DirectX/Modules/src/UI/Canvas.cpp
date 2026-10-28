#include "UI/Canvas.h"

namespace UI {
	
Canvas::Canvas(int elementNum):
	m_BIsVisible(true)
{
	m_UIList.reserve(elementNum);
}


Canvas::~Canvas() {
}


void Canvas::render(DirectX::SpriteBatch& batch) {
}


void Canvas::addUI(std::unique_ptr<UI::Base> addUI)
{
	m_UIList.emplace_back(std::move(addUI));
}


}//	namespace UI