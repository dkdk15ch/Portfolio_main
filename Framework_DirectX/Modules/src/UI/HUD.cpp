#include "UI/HUD.h"
#include "InOutput/Pad.h"
#include "SafeDelete.h"

HUD* HUD::m_Instance = nullptr;

namespace {
	using namespace DirectX;
	const float offset = 20.0f;
	const double FPS_UPDATE_INTERVAL = 0.5;
	double timeCount = FPS_UPDATE_INTERVAL;
	int frameCount = 0;;
}


HUD::HUD() : m_BIsVisible(true)
{
	if (m_Instance) {
		return;
	}
	m_Instance = this;

#ifdef _DEBUG
	//	初期設定
	auto key = std::format(L"FPS");
	auto text = std::format(L"FPS: hoge");
	addDebugElement(key, text, Colors::Green);
	
	key = std::format(L"CameraPos");
	text = std::format(L"CameraPos: x = hoge, y = hoge, z = hoge");
	addDebugElement(key, text, Colors::Blue);
	
	key = std::format(L"CameraRot");
	text = std::format(L"CameraRot: yaw = hoge, pitch = hoge");
	addDebugElement(key, text, Colors::Cyan);

	key = std::format(L"ObjectTransform");
	text = std::format(L"NameHoge: xyz/ yaw pitch row");
	addDebugElement(key, text, Colors::GreenYellow);

	key = std::format(L"CollisionTest");
	text = std::format(L"BallDis hoge / not detect ");
	addDebugElement(key, text, Colors::Pink);
#endif // _DEBUG
}


HUD::~HUD()
{
}


void HUD::term() {
	for (auto& e : elements) {
		SafeDelete(e.second);
	}
}


void HUD::update(double deltaTime) {
#ifdef _DEBUG
	if (!m_Instance) return;

	auto pad = Pad::m_Instance;
	// HUDオンオフ
	if (pad->isTriggered(Pad::Button::TAB)) {
		m_BIsVisible = !m_BIsVisible;
	}
	//	FPS
	if (pad->isTriggered(Pad::Button::D5)) {
		toggleVisibleElement(L"FPS");
	}

	timeCount += deltaTime;
	frameCount++;
	if (timeCount >= FPS_UPDATE_INTERVAL) {
		//	更新
		const double fps = frameCount/timeCount;
		elements[L"FPS"]->font.text = std::format(L"FPS: {}", std::round(fps));
		timeCount -= FPS_UPDATE_INTERVAL;
		frameCount = 0;
	}


	//	Camera
	if (pad->isTriggered(Pad::Button::D6)) {
		toggleVisibleElement(L"CameraPos");
		toggleVisibleElement(L"CameraRot");
	}
	//	ObjectTransform
	if (pad->isTriggered(Pad::Button::D7)) {
		toggleVisibleElement(L"ObjectTransform");
	}


#endif // _DEBUG
}


void HUD::render(DirectX::SpriteFont& spriteFont)
{
#ifdef _DEBUG
	if (!m_BIsVisible) { return; }
	float posY = 10.0f;

	for (auto& e : elements) {
		if (!e.second->bIsVisble) continue;
		const auto& text = e.second->font.text;
		auto lineCount = static_cast<int>(std::count(text.begin(), text.end(), L'\n'));
		posY += offset;
		e.second->font.position.y = posY;

		Font::draw(spriteFont,e.second->font);
		posY += offset * lineCount;
	}
#endif
}


void HUD::sendDebugMessage(const std::wstring& key, const std::wstring& text)
{
#ifdef _DEBUG
	if (!m_Instance) { return; }
	auto it = m_Instance->elements.find(key);
	if (it == m_Instance->elements.end()) {
		addDebugElement(key, text, DirectX::Colors::Gray);
		return;
	}

	it->second->font.text = text;
#endif // _DEBUG
}


void HUD::changeColor(const std::wstring& key, DirectX::XMVECTOR color)
{
#ifdef _DEBUG

	if (!m_Instance) { return; }

	m_Instance->elements[key]->font.color = color;
#endif
}


void HUD::toggleVisibleElement(const std::wstring& key)
{
#ifdef _DEBUG

	if (!m_Instance) { return; }

	m_Instance->elements[key]->bIsVisble = !m_Instance->elements[key]->bIsVisble;
#endif
}


void HUD::addDebugElement(const std::wstring& name, const std::wstring& text, DirectX::XMVECTOR color)
{
#ifdef _DEBUG
	if (!m_Instance) { return; }

	auto font = Font::Desc();
	auto e = new DebugElement(true, font);
	e->font.color = color;
	e->font.text = text;
	e->font.scale = XMFLOAT2(0.5f, 0.5f);
	e->bIsVisble = true;
	m_Instance->elements.insert({name, e });
#endif
}
