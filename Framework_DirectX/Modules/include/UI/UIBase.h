#pragma once
#include <memory>
#include <Windows.h>
#include <SimpleMath.h>
#include <SpriteBatch.h>

namespace UI {

class Base
{
public:
	Base();
	~Base();

	//	1.画像表示する or 文字
	virtual void render(DirectX::SpriteBatch& batch) = 0;

	void setVisible(bool value) { m_BIsVisible = value; }

private:
	bool m_BIsVisible;
};

}// namespace UI


