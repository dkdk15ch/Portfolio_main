#pragma once
#include <Audio.h>
#include <memory>
#include <map>
#include "Interface/IAsset.h"
#include "PointerHelper.h"

class ShotSound;
class RoopSound;
class Sound;


class Audio : IAsset
{
public:
	static Audio* m_Instance;
	static DirectX::AudioEngine* m_AudioEngine;
	~Audio();
	
	static void create();
	static void destroy();
	static void update();
	
	/**
	 * @brief		音声のロード
	 * @param path  ロードするパス
	 * @return		マネージャーに登録したkey
	 */
	//static std::string loadSound(
	//	const std::wstring& path);
	static std::string loadSound(
		const std::string& path);

	static void playSE(const std::string& key, 
		float volume = 1.0f, float pitch = 0.0f, float pan = 0.0f);

private:
	Audio();
};

class Sound : public IAsset {
public:
	Sound(const char* key);
	virtual ~Sound() = default;

	AssetType type() override { return AssetType::Sound; }
	DirectX::SoundEffect* sound()const { return m_SoundEffect.get(); }
private:
	std::unique_ptr<DirectX::SoundEffect> createSoundEffectFromMemory(const char* name);
	std::unique_ptr<DirectX::SoundEffect> m_SoundEffect;
};

class RoopSound : public IAsset {//	連続再生用
public:
	//RoopSound(const std::wstring& path);
	RoopSound(const std::string& path);
	~RoopSound();
	void term();

	void playRoop();	//	ループ再生
	void stop();		//	停止
	void pause();		//	一時停止
	void resume();		//	一時停止解除
	void setVolume(float volume);

private:
	DirectX::SoundEffect* m_SoundEffect;
	std::unique_ptr<DirectX::SoundEffectInstance> m_SoundInstance;
};
