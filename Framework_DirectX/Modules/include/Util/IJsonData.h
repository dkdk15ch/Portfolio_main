#pragma once
#include "Util/json.hpp"
#include <string>
#include <SimpleMath.h>
#include "Logger.h"
#include "StringHelper.h"


class IJsonData {
public:
	virtual ~IJsonData() = default;

protected:
	struct ActorParam {
		std::map<std::string, nlohmann::json> DataList;

		DirectX::SimpleMath::Vector3 getVector3Safe(const std::string& key) const;
		int getIntSafe(const std::string& key) const;
		bool getBoolSafe(const std::string& key) const;
		uint32_t getUInt32Safe(const std::string& key) const;
		float getFloatSafe(const std::string& key) const;
		double getDoubleSafe(const std::string& key) const;
		std::wstring getWStringSafe(const std::string& key) const;
		std::string getStringSafe(const std::string& key) const;
	};
	struct SceneData {
		std::map<std::string, ActorParam> DataList;
		const ActorParam& actorParam(const std::string& key);
	};

private:
	//	ラッパー関数
	static int getUInt(const nlohmann::json& j) { return j.get<int>(); }
	static uint32_t getUInt32(const nlohmann::json& j) {
		return j.get<uint32_t>();
	}
	static bool getBool(const nlohmann::json& j) { return j.get<bool>(); }
	static float getFloat(const nlohmann::json& j) { return j.get<float>(); }
	static double getDouble(const nlohmann::json& j) { return j.get<double>(); }
	// "x y z" 形式の文字列 => DirectX::SimpleMath::Vector3
	static DirectX::SimpleMath::Vector3 getVector3(const nlohmann::json& j);
	// "x y z w" 形式 => Vector4
	static DirectX::SimpleMath::Vector4 getVector4(const nlohmann::json& j);
	static std::wstring getWString(const nlohmann::json& j);
	static std::string getString(const nlohmann::json& j);
};

class IActorData : public IJsonData {
public:
	static IActorData* m_Instance;
	IActorData();
	virtual ~IActorData();

	//	ロードしてリスト追加
	static void loadFromJson(const std::wstring& filepath);

	//	ロード済みリストから取得
	static const ActorParam& actorParam(const std::string& key);

	virtual void moveFromJson(const std::string& actorKey = "", const std::string& sceneKey = "") {};
	
	static void destroy();

private:
	//	事前にロードして保存するリスト
	std::map<std::string, ActorParam> actorParamList;
};

class ISceneData : public IJsonData {
public:
	static ISceneData* m_Instance;
	ISceneData();
	virtual ~ISceneData();
	
	//	ロードしてリスト追加
	static void loadFromJson(const std::wstring& filepath);

	//	ロード済みリストから取得
	static const SceneData& sceneData(const std::string& key);

	virtual void moveFromJson(const std::string& sceneKey) {};

	static void destroy();

private:
	//	事前にロードして保存するリスト
	std::map<std::string, SceneData> sceneDataList;
};