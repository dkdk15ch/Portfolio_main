#include "InOutput/Pad.h"
#include "Audio.h"

Pad* Pad::m_Instance = nullptr;

Pad::Pad() :
	m_KeyboardState{},
	m_GamepadState{},
	m_PrevKeyboardState{}
{
	if (!m_Instance)
	{
		
		m_Instance = this;
		m_Keyboard = std::make_unique<DirectX::Keyboard>();
		m_GamePad = std::make_unique<DirectX::GamePad>();
	}
}


Pad::~Pad()
{
}


void Pad::update()
{
 	m_PrevKeyboardState = m_KeyboardState;
	m_KeyboardState = m_Keyboard->GetState();
	m_KeyTracker.Update(m_KeyboardState);

	m_GamepadState = m_GamePad->GetState(0,DirectX::GamePad::DEAD_ZONE_CIRCULAR);
	m_PrevPadTracker = m_PadTracker;
	m_PadTracker.Update(m_GamepadState);
}


bool Pad::isTriggered(Button b) {
	typedef DirectX::GamePad::ButtonStateTracker PadTracker;

	switch (b)
	{
	case Pad::W:{
		if (m_KeyTracker.pressed.W || m_PadTracker.dpadUp == PadTracker::PRESSED)
		{
			return true;
		}
		return false;
	}
	case Pad::A:{
		if (m_KeyTracker.pressed.A || m_PadTracker.dpadLeft == PadTracker::PRESSED)
		{
			return true;
		}
		return false;

	}
	case Pad::S:{
		if (m_KeyTracker.pressed.S || m_PadTracker.dpadDown == PadTracker::PRESSED)
		{
			return true;
		}
		return false;
	}
	case Pad::D:{
		if (m_KeyTracker.pressed.D || m_PadTracker.dpadRight == PadTracker::PRESSED)
		{
			return true;
		}
		return false;
	}
	case Pad::SPACE:{
		if (m_KeyTracker.pressed.Space || m_PadTracker.a == PadTracker::PRESSED)
		{
			return true;
		}
		return false;
	}
	case Pad::TAB: {
		if (m_KeyTracker.pressed.Tab || m_PadTracker.start == PadTracker::PRESSED)
		{
			return true;
		}
		return false;
	}
	case Pad::D1:{

		if (!m_PrevKeyboardState.D1 && m_KeyboardState.D1)
		{
			return true;
		}
		return false;
	}
	case Pad::D2:{
		if (!m_PrevKeyboardState.D2 && m_KeyboardState.D2)
		{
			return true;
		}
		return false;
	}
	case Pad::D3:{
		if (!m_PrevKeyboardState.D3 && m_KeyboardState.D3)
		{
			return true;
		}
		return false;
	}
	case Pad::D4:{
		if (!m_PrevKeyboardState.D4 && m_KeyboardState.D4)
		{
			return true;
		}
		return false;
	}
	case Pad::D5:{
		if (!m_PrevKeyboardState.D5 && m_KeyboardState.D5)
		{
			return true;
		}
		return false;
	}
	case Pad::D6:{
		if (!m_PrevKeyboardState.D6 && m_KeyboardState.D6)
		{
			return true;
		}
		return false;
	}
	case Pad::D7:{
		if (!m_PrevKeyboardState.D7 && m_KeyboardState.D7)
		{
			return true;
		}
		return false;
	}
	case Pad::D8:{
		if (!m_PrevKeyboardState.D8 && m_KeyboardState.D8)
		{
			return true;
		}
		return false;
	}
	case Pad::D9:{
		if (!m_PrevKeyboardState.D9 && m_KeyboardState.D9)
		{
			return true;
		}
		return false;
	}
	case Pad::D0:{
		if (!m_PrevKeyboardState.D0 && m_KeyboardState.D0)
		{
			return true;
		}
		return false;
	}
	case Pad::MINUS: {
		if (!m_PrevKeyboardState.OemMinus && m_KeyboardState.OemMinus)
		{
			return true;
		}
		return false;
	}
	case Pad::ESC: {
		if (!m_PrevKeyboardState.Escape && m_KeyboardState.Escape)
		{
			return true;
		}
		return false;
	}


	default:
	{
		return false;
	}
	}
}

bool Pad::hasPadInput() const
{
	if (!m_GamepadState.connected) { return false; }

	bool hasInput = //	スティック
		(m_GamepadState.thumbSticks.leftX != 0.0f ||
		m_GamepadState.thumbSticks.leftY != 0.0f);
	hasInput |= //	トリガー
		(m_GamepadState.triggers.left != 0.0f ||
			m_GamepadState.triggers.right != 0.0f);
	hasInput |= //	十字
		(m_GamepadState.dpad.up ||
			m_GamepadState.dpad.down ||
			m_GamepadState.dpad.left ||
			m_GamepadState.dpad.right);
	hasInput |= //	残り
		(m_GamepadState.buttons.a ||
			m_GamepadState.buttons.b ||
			m_GamepadState.buttons.x ||
			m_GamepadState.buttons.y ||
			m_GamepadState.buttons.leftShoulder ||
			m_GamepadState.buttons.rightShoulder ||
			m_GamepadState.buttons.leftStick ||
			m_GamepadState.buttons.rightStick ||
			m_GamepadState.buttons.start ||
			m_GamepadState.buttons.back);
	return hasInput;
}

bool Pad::hasMoveInput()const {
	bool isPadActive = (
		m_GamepadState.thumbSticks.leftX != 0.0f) ||
		(m_GamepadState.thumbSticks.leftY != 0.0f);
	bool isKBActive = 
		m_KeyboardState.W ||
		m_KeyboardState.A ||
		m_KeyboardState.S ||
		m_KeyboardState.D;

	return isPadActive || isKBActive;
}
