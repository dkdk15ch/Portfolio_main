#include "InOutput/Audio.h"
#include <FileUtil.h>
#include "SafeDelete.h"
#include "Logger.h"
#include "StringHelper.h"
#include "Interface/IAsset.h"
#include "binFileLoader.h"

Audio* Audio::m_Instance = nullptr;
DirectX::AudioEngine* Audio::m_AudioEngine = nullptr;

namespace {
	constexpr float MAX_VOLUME = 4.0f;
}

Audio::Audio()
{
}


Audio::~Audio()
{
}


void Audio::create()
{
	if (m_Instance)return;

	m_Instance = new Audio;
	m_AudioEngine = new DirectX::AudioEngine();
}


void Audio::destroy()
{
	SAFE_DELETE(m_AudioEngine);
	SafeDelete(m_Instance);
}


void Audio::update() {
	if (m_AudioEngine && !m_AudioEngine->Update()) {
		assert(false && "Error - Audio::update()");
	}
}


//std::string Audio::loadSound(const std::wstring& path)
//{
//	assert(audioEngine && "Error Audio instance nullptr");
//	auto& mgr = m_pAssetManager;
//
//	if(mgr->containsFromPath(path, AssetType::Sound)) {
//		const auto& key = mgr->getKeyFromPath(path, AssetType::Sound);
//		return key;
//	}
//	std::wstring filePath;
//	if (!SearchFilePath(path.c_str(), filePath))	{
//		return std::string();
//	}
//
//	auto sound = MAKE_UNIQUE_LOG<Sound>(filePath.c_str());
//	std::string key = WStringToString(path);
//	mgr->Add(path, key, std::move(sound));
//	return key;
//}

std::string Audio::loadSound(const std::string& path)
{
	assert(m_AudioEngine && "Error Audio instance nullptr");
	auto& mgr = m_PAssetManager;

	auto pathW = StringToWString(path);
	if (mgr->containsFromPath(pathW, AssetType::Sound)) {
		const auto& key = mgr->getKeyFromPath(pathW, AssetType::Sound);
		return key;
	}

	auto sound = MAKE_UNIQUE_LOG<Sound>(path.c_str());
	mgr->Add(pathW, path, std::move(sound));
	return path;
}

void Audio::playSE(const std::string& key, float volume, float pitch, float pan)
{
	auto& mgr = *m_PAssetManager;
	auto s = mgr.getSound(key);
	if (s) {
		float v = std::clamp(volume, 0.0f, MAX_VOLUME);
		s->sound()->Play(v, pitch, pan);
	
		return;
	}
	ELOG("Error: Audio::playSE - not found Sound");
}

Sound::Sound(const char* key)
	: m_SoundEffect(createSoundEffectFromMemory(key)) {
	if (!m_SoundEffect) {
		std::wstring filePath;
		if (!SearchFilePath(StringToWString(key).c_str(), filePath)) {
			assert(false && "Error: Sound::Sound - failed");
			return;
		}

		m_SoundEffect = std::make_unique<DirectX::SoundEffect>(
			Audio::m_AudioEngine, filePath.c_str());
	}
}

std::unique_ptr<DirectX::SoundEffect> Sound::createSoundEffectFromMemory(const char* name) {
	assert(Audio::m_AudioEngine && "Error Audio instance nullptr");

	auto wav = m_PAssetManager->readWav(name);
	if (!wav) { return nullptr; }
	const auto* wfx = reinterpret_cast<const WAVEFORMATEX*>(wav->fullData.get() + wav->fxOffset);
	const uint8_t* startAudio = wav->fullData.get() + wav->dataOffset;

	auto fullData = std::move(wav->fullData);
	if (!fullData) { 
		return nullptr; 
	}

	return std::unique_ptr<DirectX::SoundEffect>(
		new DirectX::SoundEffect(
			Audio::m_AudioEngine,
			fullData,
			wfx,
			startAudio,
			wav->dataSize
		)
	);
}

#pragma region RoopSound

//RoopSound::RoopSound(const std::wstring& path)
//	: m_SoundEffect(nullptr)
//{
//	auto& mgr = *m_pAssetManager;
//	auto key = mgr.getKeyFromPath(path, AssetType::Sound);
//	if (key.empty()) {
//		key = Audio::loadSound(path);
//	}
//	auto s = mgr.getSound(key);
//	if (s) {
//		m_SoundEffect = s->sound();
//		m_SoundInstance = m_SoundEffect->CreateInstance();
//		return;
//	}
//	ELOG("Error: RoopSound::RoopSound - not found Sound");
//}

RoopSound::RoopSound(const std::string& path)
	: m_SoundEffect(nullptr)
{
	auto& mgr = *m_PAssetManager;
	auto key = mgr.getKeyFromPath(StringToWString(path), AssetType::Sound);
	if (key.empty()) {
		key = Audio::loadSound(path);
	}
	auto s = mgr.getSound(key);
	if (s) {
		m_SoundEffect = s->sound();
		m_SoundInstance = m_SoundEffect->CreateInstance();
		return;
	}
	ELOG("Error: RoopSound::RoopSound - not found Sound");
}

RoopSound::~RoopSound()
{
	term();
}


void RoopSound::term() {
	m_SoundInstance->Stop();
	m_SoundInstance.reset();
}


void RoopSound::playRoop()
{
	m_SoundInstance->Play(true);
}


void RoopSound::stop()
{
	m_SoundInstance->Stop();
}


void RoopSound::pause()
{
	m_SoundInstance->Pause();
}


void RoopSound::resume()
{
	m_SoundInstance->Resume();
}

void RoopSound::setVolume(float volume)
{
	float newVolume = std::clamp(volume, 0.0f, MAX_VOLUME);
	m_SoundInstance->SetVolume(newVolume);
}

#pragma endregion



