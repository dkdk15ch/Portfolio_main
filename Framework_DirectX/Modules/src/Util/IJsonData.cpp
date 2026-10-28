#include "Util/IJsonData.h"
#include "FileUtil.h"
#include <fstream>
#include <string>
#include "SafeDelete.h"
#include "Logger.h"
#include "Manager/ObjectParamManager.h"
#include "Interface/IAsset.h"

IActorData* IActorData::m_Instance = nullptr;
ISceneData* ISceneData::m_Instance = nullptr;

namespace {
    class JsonArchiveAccessor: IAsset {
    public:
        AssetType type() { return AssetType::NONE; }

        std::vector<char> ReadJson(const char* key)const {
            return m_PAssetManager->readDataTable(key);
        };
    };
}

#pragma region IJsonData

#pragma region getter from Json

DirectX::SimpleMath::Vector3 IJsonData::getVector3(const nlohmann::json& j)
{
    std::string str = j.get<std::string>();
    float x = 0.0f, y = 0.0f, z = 0.0f;
    sscanf_s(str.c_str(), "%f %f %f", &x, &y, &z);
    return DirectX::SimpleMath::Vector3(x, y, z);
}


DirectX::SimpleMath::Vector4 IJsonData::getVector4(const nlohmann::json& j)
{
    std::string str = j.get<std::string>();
    float x, y, z, w;
    sscanf_s(str.c_str(), "%f %f %f %f", &x, &y, &z, &w);
    return DirectX::SimpleMath::Vector4(x, y, z, w);
}


std::wstring IJsonData::getWString(const nlohmann::json& j)
{
    return StringToWString(j.get<std::string>());
}

std::string IJsonData::getString(const nlohmann::json& j)
{
    return j.get<std::string>();
}

DirectX::SimpleMath::Vector3 IJsonData::ActorParam::getVector3Safe(const std::string& key) const
{
    auto it = DataList.find(key);

    if (it == DataList.end()) {
        return DirectX::SimpleMath::Vector3::Zero;
    }
    if (!it->second.is_string()) {
        ELOG("Warning: Key '%s' is not a string", key.c_str());
        return DirectX::SimpleMath::Vector3::Zero;
    }
    return getVector3(it->second);
}

int IJsonData::ActorParam::getIntSafe(const std::string& key) const
{
    auto it = DataList.find(key);

    if (it == DataList.end()) {
        return 0;
    }
    if (!it->second.is_number_integer()) {
        ELOG("Warning: Key '%s' is not a number", key.c_str());
        return 0;
    }
    return getUInt(it->second);
}

bool IJsonData::ActorParam::getBoolSafe(const std::string& key) const
{
    auto it = DataList.find(key);

    if (it == DataList.end()) {
        return 0;
    }
    if (!it->second.is_boolean()) {
        ELOG("Warning: Key '%s' is not a number", key.c_str());
        return 0;
    }
    return getBool(it->second);
}

uint32_t IJsonData::ActorParam::getUInt32Safe(const std::string& key) const
{
    auto it = DataList.find(key);

    if (it == DataList.end()) {
        return 0;
    }
    if (!it->second.is_number_unsigned()) {
        ELOG("Warning: Key '%s' is not a number", key.c_str());
        return 0;
    }
    return getUInt32(it->second);
}

float IJsonData::ActorParam::getFloatSafe(const std::string& key) const
{
    auto it = DataList.find(key);

    if (it == DataList.end()) {
        return 0.0f;
    }
    if (!it->second.is_number()) {
        ELOG("Warning: Key '%s' is not a number", key.c_str());
        return 0.0f;
    }
    return getFloat(it->second);
}

double IJsonData::ActorParam::getDoubleSafe(const std::string& key) const
{
    auto it = DataList.find(key);

    if (it == DataList.end()) {
        return 0.0;
    }
    if (!it->second.is_number()) {
        ELOG("Warning: Key '%s' is not a number", key.c_str());
        return 0.0;
    }
    return getDouble(it->second);
}

std::wstring IJsonData::ActorParam::getWStringSafe(const std::string& key) const
{
    auto it = DataList.find(key);

    if (it == DataList.end()) {
        return std::wstring();
    }
    if (!it->second.is_string()) {
        ELOG("Error: Key '%s' is not a string", key.c_str());
        return std::wstring();
    }
    return getWString(it->second);
}

std::string IJsonData::ActorParam::getStringSafe(const std::string& key) const
{
    auto it = DataList.find(key);

    if (it == DataList.end()) {
        return std::string();
    }
    if (!it->second.is_string()) {
        ELOG("Error: Key '%s' is not a string", key.c_str());
        return std::string();
    }
    return getString(it->second);
}
#pragma endregion


const IJsonData::ActorParam& IActorData::actorParam(const std::string& key)
{
    auto m_Instance = IActorData::m_Instance;

    auto it = m_Instance->actorParamList.find(key);
    if (it == m_Instance->actorParamList.end()) {
        assert(false && L"Error: IActorData::actorParam");
        return IJsonData::ActorParam();
    }
    return it->second;
}


const IJsonData::SceneData& ISceneData::sceneData(const std::string& key)
{
    auto m_Instance = ISceneData::m_Instance;

    auto it = m_Instance->sceneDataList.find(key);
    if (it == m_Instance->sceneDataList.end()) {
        assert(false && L"Error: ISceneData::sceneData");
        return IJsonData::SceneData();
    }
    return it->second;
}


const IJsonData::ActorParam& IJsonData::SceneData::actorParam(const std::string& key) {

    auto it = DataList.find(key);
    if (it == DataList.end()) {
        assert(false && L"Error: SceneData::actorParam");
        return IJsonData::ActorParam();
    }
    return it->second;
}

#pragma endregion


#pragma region IActorData
IActorData::IActorData()
{
    if (m_Instance) return;

    m_Instance = this;

}


IActorData::~IActorData()
{
}


void IActorData::loadFromJson(const std::wstring& filepath)
{
    assert(m_Instance && "IActorData::LoadFromJson");

    auto accessor = std::make_unique<JsonArchiveAccessor>();
    auto data = accessor->ReadJson(WStringToString(filepath).c_str());
    nlohmann::json input;

    std::wstring path;
    if (data.empty()) {
        //  ファイルチェック
        if (!SearchFilePath(filepath.c_str(), path))
        {
            ELOG("Error : IActorData::LoadFromJson - File Not Found.");
            return;
        }
        auto stringPath = WStringToString(path);

        std::ifstream ifs(stringPath);

        if (!ifs.is_open()) {
            ELOG("Error : IActorData::LoadFromJson Cannot open file: %s", stringPath.c_str());
            return;
        }

        ifs >> input;
    }
    else {
        input = nlohmann::json::parse(data.begin(),data.end());
    }

    auto actorObj = input[0];

    //  nameをリスト用キーとして使用
    auto listKey = actorObj.value("name", "");
    //  各valueを読み取り
    IJsonData::ActorParam param;
    for (auto it = actorObj.begin(); it != actorObj.end(); ++it) {
        if (it.key() == "name") continue;
        param.DataList[it.key()] = it.value();
    }
    m_Instance->actorParamList[listKey] = param;
}


void IActorData::destroy() {

    m_Instance->actorParamList.clear();
    m_Instance = nullptr;
}

#pragma endregion


#pragma region ISceneData

ISceneData::ISceneData()
{
    if (m_Instance) return;

    m_Instance = this;
}


ISceneData::~ISceneData()
{
}


void ISceneData::loadFromJson(const std::wstring& filepath)
{
    assert(m_Instance && "ISceneData::LoadFromJson");

    auto accessor = std::make_unique<JsonArchiveAccessor>();
    auto data = accessor->ReadJson(WStringToString(filepath).c_str());
    nlohmann::json input;
    std::wstring path;

    if (data.empty()) {
        //  ファイルチェック
        if (!SearchFilePath(filepath.c_str(), path))
        {
            ELOG("Error : IActorData::LoadFromJson - File Not Found.");
            return;
        }
        std::ifstream ifs(path);

        ifs >> input;
        // 入力が配列かどうかチェック
        if (!input.is_array() || input.size() == 0) {
            ELOG("Error : ISceneData::LoadFromJson - Input is not array or empty.");
            return;
        }
    }
    else {
        input = nlohmann::json::parse(data.begin(), data.end());
    }

    //  Sceneの場合は、先頭にnameだけSceneNameとする
    const auto& firstObj = input[0];
    auto sceneName = firstObj.value("name", "");
    IJsonData::SceneData sceneData;
    for (size_t i = 1; i < input.size(); ++i) {
        auto listKey = input[i].value("name", "");
        IJsonData::ActorParam param;
        for (auto j = input[i].begin(); j != input[i].end(); ++j) {
            if (j.key() == "name") continue;
            param.DataList[j.key()] = j.value();
        }
        sceneData.DataList[listKey] = param;
    }
    m_Instance->sceneDataList[sceneName] = sceneData;
}


void ISceneData::destroy() {
    m_Instance->sceneDataList.clear();
    m_Instance = nullptr;
}
#pragma endregion

