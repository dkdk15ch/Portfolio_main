#include "UI/UIImage.h"
#include "DrawHelper/Image.h"

namespace UI {
	
Image::Image(ID3D12Device* pDevice,
	DescriptorPool* pPool,
	DirectX::ResourceUploadBatch& batch,
	const std::wstring& path,
	RECT destinationRect,
	DirectX::FXMVECTOR color) :
	m_DestinationRect(destinationRect),
	m_Color(color)
{
		m_Image = std::make_unique<::Image>(pDevice, pPool, batch, path);
}

	void Image::render(DirectX::SpriteBatch& batch) {
		m_Image->drawStretchRect(batch, m_DestinationRect, nullptr, m_Color);
	}

}//	namespace UI
