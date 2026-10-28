#pragma once
#include <memory>
#include "keyboard.h"
#include "GamePad.h"

class Pad
{
public:
	Pad();
	~Pad();

	enum Button {
		W,
		A,
		S,
		D,
		SPACE,
		TAB,
		D1,
		D2,
		D3,
		D4,
		D5,
		D6,
		D7,
		D8,
		D9,
		D0,
		MINUS,
		ESC,
		Count,
	};


	static Pad* m_Instance;

	void update();
	bool isTriggered(Button b);

	std::unique_ptr<DirectX::Keyboard> m_Keyboard;
	DirectX::Keyboard::State m_KeyboardState;		//	キー入力
	DirectX::Keyboard::State m_PrevKeyboardState;	//	前フレームの入力
	DirectX::Keyboard::KeyboardStateTracker m_KeyTracker;

	std::unique_ptr<DirectX::GamePad> m_GamePad;
	DirectX::GamePad::State m_GamepadState;
	DirectX::GamePad::ButtonStateTracker m_PadTracker;
	DirectX::GamePad::ButtonStateTracker m_PrevPadTracker;
	bool hasPadInput()const;
	bool hasMoveInput()const;
private:
};

