#pragma once
#include <string>
#include <memory>
#include "Util/Font.h"
#include <format>	//	sendMessageでformat使うので、インクルードしておく
#include <SpriteFont.h>
#include <map>

/**
 * @brief	HUD用クラス
 */
class HUD
{
public:
	static HUD* m_Instance;

	HUD();
	~HUD();
	void term();
	

	void update(double deltaTime);
	void render(DirectX::SpriteFont& spriteFont);

private:
	bool m_BIsVisible;

public:
	struct DebugElement
	{
		bool bIsVisble;
		Font::Desc font;
		DebugElement(bool bIsVisible, Font::Desc desc) :
		bIsVisble(false)
		{}
		~DebugElement() {}
	};

	static void sendDebugMessage(const std::wstring& key, const std::wstring& text);
	static void changeColor(const std::wstring& key, DirectX::XMVECTOR color);

	static void addDebugElement(
		const std::wstring& name, const std::wstring& text,
		DirectX::XMVECTOR color);
	static void toggleVisibleElement(const std::wstring& key);
private:
	std::map<std::wstring, DebugElement*> elements;
};
