#pragma once

#ifndef DX_TEXTURE_ACCESS
#define DX_TEXTURE_ACCESS

#include <d3d11.h>
#include <spindec.h>
#include <string> // For memcpy
#include "BaseTextureAccess.h"


class DxTextureAccess : public BaseTextureAccess
{
public:
	DxTextureAccess(uintptr_t texturePtr[], unsigned int width, unsigned int height, CHROMA_SUBSAMPLING chroma_subsampling);
	~DxTextureAccess() override;

	virtual bool applyPictureData(const Spin_Picture* pic) override;

	void setDxDevice(ID3D11Device* device);

private:
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_ctx = nullptr;
	ID3D11Texture2D* m_d3dtexY;
	ID3D11Texture2D* m_d3dtexU;
	ID3D11Texture2D* m_d3dtexV;
	//void DxTextureAccess::CopyDataToPtr(const Spin_Picture* pic, ID3D11Texture2D* ptr);
};

#endif