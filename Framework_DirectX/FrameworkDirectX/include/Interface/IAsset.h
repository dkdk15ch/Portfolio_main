#pragma once
#include "Animation/IAssetAccessor.h"

enum class AssetType : int {
	NONE,
	MESH,
	MODEL,
	TEXTURE,
	ANIMATION,
	Sound,
};

class IAsset {
public:
	virtual ~IAsset() = default;

	static void setAccessor(IAssetAccessor* assetManager) {
		m_PAssetManager = assetManager;
	}
	static void destroyAccessor();
	virtual AssetType type() { return AssetType::NONE; }

protected:
	static IAssetAccessor* m_PAssetManager;
public:
#ifdef _DEBUG
	static constexpr const char* m_ClassName = "IAsset";
#endif
};