#pragma once
#include <memory>
#include <DirectXMath.h>
#include <SpriteBatch.h>
#include <string>
#include "Interface/IAsset.h"

class DescriptorPool;
class Texture;
struct ID3D12Device;
namespace DirectX { class ResourceUploadBatch; }

struct ImageDesc {

};

class Image : public IAsset
{
public:
	Image(
		ID3D12Device* pDevice,
		DescriptorPool* pPool,
		DirectX::ResourceUploadBatch& batch,
		const std::wstring& path,
		const char* data = nullptr,
		int size = 0
	);

	//	ダミーテクスチャ使用用
	Image();

	~Image();
	int width()const;
	int height()const;

	/**
	 * @brief				画像描画
	 * @param spriteBatch	スプライトバッチ
	 * @param position		描画位置（左上)
	 * @param srcRect		元画像の切り取り範囲
	 * @param color			色
	 */
	void draw(
		DirectX::DX12::SpriteBatch& spriteBatch,
		const DirectX::XMFLOAT2& position,
		const RECT* srcRect,
		DirectX::FXMVECTOR color)const;

	/**
	 * @brief					引き伸ばし描画
	 * @param destinationRect	描画範囲
	 * @param					その他　drawと共通
	 */
	void drawStretchRect(
		DirectX::DX12::SpriteBatch& spriteBatch,
		const RECT& destinationRect,
		const RECT* sourceRect = nullptr,
		DirectX::FXMVECTOR color = DirectX::Colors::White,
		float rotation = 0,
		DirectX::XMFLOAT2 origin = DirectX::XMFLOAT2(0.f,0.f))const;

	//	フルスクリーン描画
	void drawFullScreen(
		DirectX::DX12::SpriteBatch& spriteBatch
	);

	void drawBackGround(
		DirectX::DX12::SpriteBatch& spriteBatch
	);

private:
	Texture* m_Texture;
	DirectX::XMUINT2 m_Size;
};

