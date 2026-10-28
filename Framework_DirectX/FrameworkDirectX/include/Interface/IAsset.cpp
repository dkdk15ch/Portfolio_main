#include "Interface/IAsset.h"
#include "SafeDelete.h"

IAssetAccessor* IAsset::m_PAssetManager = nullptr;

void IAsset::destroyAccessor()
{
	m_PAssetManager = nullptr;
}
