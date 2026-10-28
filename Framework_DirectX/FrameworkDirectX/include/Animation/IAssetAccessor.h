#pragma once
#include <string>
#include <memory>
#include <PointerHelper.h>

class Mesh;
class Model;
class Texture;
class Animation;
class Sound;
class IAsset;
struct WavData;

enum class AssetType : int;

class IAssetAccessor
{
public:
	virtual void Add(
		const std::wstring path,
		const std::string& key,
		LogPtr<IAsset> asset) = 0;

	virtual std::vector<char> Read(const char* key)const = 0;
	virtual std::vector<char> readDataTable(const char* key)const = 0;
	virtual std::unique_ptr<WavData> readWav(const char* key)const = 0;

	virtual Mesh*	getMesh			(const std::string& key) = 0;
	virtual Model*	getModel		(const std::string& key) = 0;
	virtual Texture* getTexture		(const std::string& key) = 0;
	virtual Animation* getAnimation	(const std::string& key) = 0;
	virtual Sound* getSound(const std::string& key) = 0;

	virtual LogPtr<Model> cloneModel(const std::string& key) = 0;

	virtual bool containsFromPath(const std::wstring path, AssetType type) = 0;

	virtual std::string getKeyFromPath(const std::wstring path, AssetType type) = 0;
};

