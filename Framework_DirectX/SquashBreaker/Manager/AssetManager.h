#pragma once
#include <string>
#include <unordered_map>	
#include <memory>
#include "Animation/IAssetAccessor.h"
#include <assert.h>
#include <type_traits>
#include <iostream>
#include <PointerHelper.h>

class Mesh;
class Model;
class Texture;
class Animation;
class Sound;
class Archive;

class AssetManager :
	public IAssetAccessor
{
public:
    friend class DrawManager;
	static AssetManager* m_Instance;

	void init();
    void destory();

    std::vector<char> Read(const char* key)const override final;
    std::unique_ptr<WavData> readWav(const char* key)const override final;
    std::vector<char> readDataTable(const char* key)const override final;
    std::vector<char> readHLSL(const char* key)const;


	void Add(
        const std::wstring path, 
        const std::string& key, 
        LogPtr<IAsset> newResource) override final;
    
    Mesh* getMesh(const std::string& key)				override final;
	Model* getModel(const std::string& key) 			override final;
	Texture* getTexture(const std::string& key) 		override final;
	Animation* getAnimation(const std::string& key) 	override final;
    Sound* getSound(const std::string& key)             override final;

    virtual LogPtr<Model> cloneModel(const std::string& key) override final;
    bool containsFromPath(const std::wstring path, AssetType type) override final;

    std::string getKeyFromPath(const std::wstring path, AssetType type) override final;
    
private:
    template<typename T>
    T* Get(const std::string& key);
	template<typename T>
	void Add(const std::string& key, LogPtr<T> obj);
    
    template<typename T>
	std::unordered_map<std::string, LogPtr<T>>& 
        getResourceMap();

    LogPtr<Archive> m_ResArchive;
    LogPtr<Archive> m_DataTableArchive;
    LogPtr<Archive> m_HLSLArchive;

    std::unordered_map<std::string, LogPtr<Mesh>> m_MeshMap;
	std::unordered_map<std::string, LogPtr<Model>> m_ModelMap;
	std::unordered_map<std::string, LogPtr<Texture>> m_TextureMap;
	std::unordered_map<std::string, LogPtr<Animation>> m_AnimationMap;
    std::unordered_map<std::string, LogPtr<Sound>> m_SoundMap;

    std::unordered_map<std::wstring, std::string> m_MeshPathMap;
    std::unordered_map<std::wstring, std::string> m_ModelPathMap;
    std::unordered_map<std::wstring, std::string> m_TexturePathMap;
    std::unordered_map<std::wstring, std::string> m_AnimationPathMap;
    std::unordered_map<std::wstring, std::string> m_SoundPathMap;


public:
#ifdef _DEBUG
    static constexpr const char* m_ClassName = "AssetManager";
#endif
};

template<typename T>
inline void AssetManager::Add(const std::string& key, LogPtr<T> obj)
{
    getResourceMap<T>()[key] = std::move(obj);
}

template<typename T>
inline T* AssetManager::Get(const std::string& key)
{
    auto it = getResourceMap<T>().find(key);
    if (it != getResourceMap<T>().end()) {
        return it->second.get();
    }

    return nullptr;
}


template<typename T>
inline std::unordered_map<std::string, LogPtr<T>>& AssetManager::getResourceMap()
{

    if constexpr (std::is_same_v<T, Mesh>)
    {
        return m_MeshMap;
    }
    else if constexpr (std::is_same_v<T, Model>)
    {
        return m_ModelMap;
    }
    else if constexpr (std::is_same_v<T, Texture>)
    {
        return m_TextureMap;
    }
    else if constexpr (std::is_same_v<T, Animation>)
    {
        return m_AnimationMap;
    }
    else if constexpr (std::is_same_v<T, Sound>)
    {
        return m_SoundMap;
    }

    assert(false);
}