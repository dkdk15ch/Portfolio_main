#include "Manager/AssetManager.h"
#include "GraphicContext.h"
#include "Interface/IAsset.h"
#include "Mesh.h"
#include "Texture.h"
#include "InOutput/Audio.h"
#include "Animation/Model.h"
#include "Animation/Animation.h"
#include "PointerHelper.h"
#include "SafeDelete.h"
#include "FileUtil.h"
#include "binFileLoader.h"

AssetManager* AssetManager::m_Instance = nullptr;

void AssetManager::init() {
	if (m_Instance) { return; }

	m_Instance = this;
	IAsset::setAccessor(this);

	m_ResArchive = MAKE_UNIQUE_LOG<Archive>();
	m_ResArchive->open("bin/res.bin");

	m_DataTableArchive = MAKE_UNIQUE_LOG<Archive>();
	m_DataTableArchive->open("bin/dataTable.bin");

	m_HLSLArchive = MAKE_UNIQUE_LOG<Archive>();
	m_HLSLArchive->open("bin/HLSL.bin");
}

void AssetManager::destory()
{
	GraphicContext::m_Instance->present(0);

	m_MeshMap.clear();
	m_TextureMap.clear();
	m_AnimationMap.clear();
	m_ModelMap.clear();
	m_MeshPathMap.clear();
	m_TexturePathMap.clear();
	m_AnimationPathMap.clear();
	m_ModelPathMap.clear();
}

std::vector<char> AssetManager::Read(const char* key) const
{
	return m_ResArchive->read(key);
}

std::unique_ptr<WavData> AssetManager::readWav(const char* key) const
{
	return m_ResArchive->readWAV(key);
}

std::vector<char> AssetManager::readDataTable(const char* key) const
{
	return m_DataTableArchive->read(key);
}

std::vector<char> AssetManager::readHLSL(const char* key) const
{
	return m_HLSLArchive->read(key);
}

void AssetManager::Add(
	const std::wstring path,
	const std::string& key,
	LogPtr<IAsset> newResource)
{
	if (!newResource) { return; }
	auto type = newResource->type();
	auto cleanPath = NormalizePath(path);
	if (containsFromPath(cleanPath, type)) { return; }


	switch (type)
	{
	case AssetType::MESH :
	{
		LogPtr<Mesh> pMesh = StaticCast_Unique<Mesh>(std::move	(newResource));
		if (pMesh) {
			Add<Mesh>(key, std::move(pMesh));
			m_MeshPathMap[cleanPath] = key;
			return;
		}
		break;
	}
	case AssetType::MODEL:
	{
		LogPtr<Model> pModel = StaticCast_Unique<Model>(std::move (newResource));
		if (pModel) {
			Add<Model>(key, std::move(pModel));
			m_ModelPathMap[cleanPath] = key;
			return;
		}
		break;
	}
	case AssetType::TEXTURE:
	{
		LogPtr<Texture> pTex = StaticCast_Unique<Texture> (std::move(newResource));
		if (pTex) {
			Add<Texture>(key, std::move(pTex));
			m_TexturePathMap[cleanPath] = key;
			return;
		}
		break;
	}
	case AssetType::ANIMATION:
	{
		LogPtr<Animation> pAnima = StaticCast_Unique<Animation>   (std::move(newResource));
		if (pAnima) {
			Add<Animation>(key, std::move(pAnima));
			m_AnimationPathMap[cleanPath] = key;
			return;
		}
		break;
	}
	case AssetType::Sound:
	{
		LogPtr<Sound> pSound = StaticCast_Unique<Sound>(std::move(newResource));
		if (pSound) {
			Add<Sound>(key, std::move(pSound));
			m_SoundPathMap[cleanPath] = key;
			return;
		}
		break;
	}

	default:
		assert(false && "Error AssetManager::Add -  type NONE");
	}
	assert(false && "Error AssetManager::Add -  failed Cast");
}

Mesh* AssetManager::getMesh(const std::string& key)
{
	return Get<Mesh>(key);;
}

Model* AssetManager::getModel(const std::string& key)
{
	return Get<Model>(key);;
}

Texture* AssetManager::getTexture(const std::string& key)
{
	return Get<Texture>(key);;
}

Animation* AssetManager::getAnimation(const std::string& key)
{
	return Get<Animation>(key);
}

Sound* AssetManager::getSound(const std::string& key)
{
	return Get<Sound>(key);;
}

LogPtr<Model> AssetManager::cloneModel(const std::string& key)
{
	if (!m_ModelMap.contains(key)) { return nullptr; }
	
	LogPtr<Model> clone = MAKE_UNIQUE_LOG<Model>();
	*clone = *m_ModelMap[key];
	return clone;
}

bool AssetManager::containsFromPath(const std::wstring path, AssetType type)
{
	auto cleanPath = NormalizePath(path);

switch (type)
{
	case AssetType::NONE:
		break;
	case AssetType::MESH:
	{
		return m_MeshPathMap.contains(path);
	}
	case AssetType::MODEL:
	{
		return m_ModelPathMap.contains(path);
	}
	case AssetType::TEXTURE:
	{
		return m_TexturePathMap.contains(path);
	}
	case AssetType::ANIMATION:
	{
		return m_AnimationPathMap.contains(path);
	}
	case AssetType::Sound:
	{
		return m_SoundPathMap.contains(path);
	}
}
	return false;
}

std::string AssetManager::getKeyFromPath(const std::wstring path, AssetType type)
{
	auto cleanPath = NormalizePath(path);

	switch (type)
	{
	case AssetType::NONE:
		break;
	case AssetType::MESH:
	{
		auto it = m_MeshPathMap.find(cleanPath);
		if (it != m_MeshPathMap.end()) return it->second;
		break;
	}
	case AssetType::MODEL:
	{
		auto it = m_ModelPathMap.find(cleanPath);
		if (it != m_ModelPathMap.end()) return it->second;
		break;
	}
	case AssetType::TEXTURE:
	{
		auto it = m_TexturePathMap.find(cleanPath);
		if (it != m_TexturePathMap.end()) return it->second;
		break;
	}
	case AssetType::ANIMATION:
	{
		auto it = m_AnimationPathMap.find(cleanPath);
		if (it != m_AnimationPathMap.end()) return it->second;
		break;
	}
	case AssetType::Sound:
	{
		auto it = m_SoundPathMap.find(cleanPath);
		if (it != m_SoundPathMap.end()) return it->second;
		break;
	}
	}
	return std::string();
}

