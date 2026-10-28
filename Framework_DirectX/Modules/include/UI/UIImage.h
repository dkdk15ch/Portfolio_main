#pragma once
#include "UI/UIBase.h"
#include <memory>
#include <string>

class Image;
class DescriptorPool;
class Texture;
struct ID3D12Device;
namespace DirectX { class ResourceUploadBatch; }

namespace UI {

class Image : public Base
{
public:
	Image(ID3D12Device* pDevice,
		DescriptorPool* pPool,
		DirectX::ResourceUploadBatch& batch,
		const std::wstring& path,
		RECT destinationRect,
		DirectX::FXMVECTOR color);
	~Image();

	virtual void render(DirectX::SpriteBatch& batch) override final;

private:
	std::unique_ptr<::Image> m_Image;
	RECT m_DestinationRect;
	DirectX::FXMVECTOR m_Color;
};
	
}//	namespace UI

