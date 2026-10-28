#pragma once
#include "UI/UIBase.h"


/**
 * @brief	UIを複数まとめたクラス
 */
namespace UI {
class Canvas
{
public:
	Canvas(int elementNum);
	~Canvas();

	void render(DirectX::SpriteBatch& batch);
	void addUI(std::unique_ptr<UI::Base> addUI);

private:
	bool m_BIsVisible;
	std::vector<std::unique_ptr<UI::Base>> m_UIList;
};

}// namespace UI


