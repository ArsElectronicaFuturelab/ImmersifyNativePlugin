#include <Unity/Rendering/RenderAPI.h>
#include <Unity/Rendering/PlatformBase.h>

// Direct3D 11 implementation of RenderAPI.

#if SUPPORT_D3D11

#include <assert.h>
#include <d3d11.h>
#include <Unity/IUnityGraphicsD3D11.h>
#include <spindec.h>
#include "DxTextureAccess.h"


class RenderAPI_D3D11 : public RenderAPI
{
public:
	RenderAPI_D3D11();
	virtual ~RenderAPI_D3D11() { }

	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces) override;
	virtual BaseTextureAccess* InitTexture(uintptr_t texturePtr[], unsigned int width, unsigned int height, int format, CHROMA_SUBSAMPLING chroma_subsampling) override;
	//virtual void SafeDeleteTexture() override;
	//virtual void ResetBufferIndex() override;
	//virtual bool TextureAccessIsReady() override;
	//virtual void SetPicIsStrided(bool isStrided) override;
	//virtual bool ApplyPictureData(const Spin_Picture* pic) override;

	// Test:
	//void DoRendering(UnityGfxDeviceEventType type, unsigned int texturePtr[]);

	//virtual bool GetUsesReverseZ() { return (int)m_Device->GetFeatureLevel() >= (int)D3D_FEATURE_LEVEL_10_0; }

	//virtual void DrawSimpleTriangles(const float worldMatrix[16], int triangleCount, const void* verticesFloat3Byte4);

	//virtual void* BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch);
	//virtual void EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr);

	//virtual void* BeginModifyVertexBuffer(void* bufferHandle, size_t* outBufferSize);
	//virtual void EndModifyVertexBuffer(void* bufferHandle);

private:
	void CreateResources();
	void ReleaseResources();

	ID3D11Device* m_Device;
	//DxTextureAccess* m_DxTextureAccess;

	//ID3D11Texture2D* m_Texture;

	//ID3D11Buffer* m_VB; // vertex buffer
	//ID3D11Buffer* m_CB; // constant buffer
	//ID3D11VertexShader* m_VertexShader;
	//ID3D11PixelShader* m_PixelShader;
	//ID3D11InputLayout* m_InputLayout;
	//ID3D11RasterizerState* m_RasterState;
	//ID3D11BlendState* m_BlendState;
	//ID3D11DepthStencilState* m_DepthState;
};

RenderAPI* CreateRenderAPI_D3D11()
{
	return new RenderAPI_D3D11();
}


RenderAPI_D3D11::RenderAPI_D3D11()
	: m_Device(nullptr)
	//, m_DxTextureAccess(nullptr)
	//, m_Texture(nullptr)
	//, m_VB(NULL)
	//, m_CB(NULL)
	//, m_VertexShader(NULL)
	//, m_PixelShader(NULL)
	//, m_InputLayout(NULL)
	//, m_RasterState(NULL)
	//, m_BlendState(NULL)
	//, m_DepthState(NULL)
{
}


void RenderAPI_D3D11::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	switch (type)
	{
	case kUnityGfxDeviceEventInitialize:
	{
		IUnityGraphicsD3D11* d3d = interfaces->Get<IUnityGraphicsD3D11>();
		m_Device = d3d->GetDevice();
		CreateResources();
		break;
	}
	case kUnityGfxDeviceEventShutdown:
		ReleaseResources();
		break;
	}
}

//// Test only:
//void RenderAPI_D3D11::DoRendering(UnityGfxDeviceEventType type, unsigned int texturePtr[])
//{
//	if (type == kUnityGfxRendererD3D11)
//	{
//		//get the ID3D11DeviceContext
//		ID3D11DeviceContext* ctx = nullptr;
//		//g_D3D11Device is type of ID3D11Device
//		m_Device->GetImmediateContext(&ctx);
//
//		// update native texture from code
//		if (texturePtr)
//		{
//			//get the pointer from the unity texture.
//			//g_TexturePointer is receive from unity via setTexturePtr (void *)
//			//cast it in a ID3D11Texture2D
//			ID3D11Texture2D* d3dtex = (ID3D11Texture2D*)texturePtr;
//			d3dtex->GetDevice(&m_Device);
//
//			// TODO: Is this sufficient to change the texture format? If yes, just do it when receiving a new texture, not every frame!
//			// Or is it enought to get the format from unity?
//			//D3D11_TEXTURE2D_DESC desc;
//			//d3dtex->GetDesc(&desc);
//			//desc.Format = DXGI_FORMAT_BC4_UNORM;
//
//			//use ID3D11DeviceContext::UpdateSubresource to fill the default texture with data from a pointer provided by the application.
//			//void* dataPtr; 
//			//unsigned int rowPitch;
//			ctx->UpdateSubresource(d3dtex, 0, nullptr, dataPtr, rowPitch, 0);
//			ctx->Release();
//		}
//	}
//}


BaseTextureAccess* RenderAPI_D3D11::InitTexture(uintptr_t texturePtr[], unsigned int width, unsigned int height, int format, CHROMA_SUBSAMPLING chroma_subsampling)
{
	// TODO
	//ID3D11Texture2D* d3dtex = (ID3D11Texture2D*)texturePtr;
	//D3D11_TEXTURE2D_DESC desc;
	//d3dtex->GetDesc(&desc);


	// This is done within Streaming Plugin:
	DxTextureAccess* dxTextureAccess = new DxTextureAccess(texturePtr, width, height, chroma_subsampling);
	dxTextureAccess->setDxDevice(m_Device);

	return dxTextureAccess;
}

//void RenderAPI_D3D11::SafeDeleteTexture()
//{
//	// TODO -> SAFE_RELEASE(texture)
//}

//void RenderAPI_D3D11::ResetBufferIndex()
//{
//	// TODO
//}

//bool RenderAPI_D3D11::TextureAccessIsReady()
//{
//	return (m_DxTextureAccess != nullptr); // TODO is this sufficient?
//}

//void RenderAPI_D3D11::SetPicIsStrided(bool isStrided)
//{
//	if (m_DxTextureAccess != nullptr)
//	{
//		m_DxTextureAccess->setPicIsStrided(isStrided);
//	}
//}

//bool RenderAPI_D3D11::ApplyPictureData(const Spin_Picture* pic)
//{
//	return m_DxTextureAccess->applyPictureData(pic);
//	//return m_DxTextureAccess->applyPictureData(m_Device, pic);
//}

void RenderAPI_D3D11::CreateResources()
{
	//D3D11_BUFFER_DESC desc;
	//memset(&desc, 0, sizeof(desc));

	//// vertex buffer
	//desc.Usage = D3D11_USAGE_DEFAULT;
	//desc.ByteWidth = 1024;
	//desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//m_Device->CreateBuffer(&desc, NULL, &m_VB);

	//// constant buffer
	//desc.Usage = D3D11_USAGE_DEFAULT;
	//desc.ByteWidth = 64; // hold 1 matrix
	//desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//desc.CPUAccessFlags = 0;
	//m_Device->CreateBuffer(&desc, NULL, &m_CB);

	//// shaders
	//HRESULT hr;
	//hr = m_Device->CreateVertexShader(kVertexShaderCode, sizeof(kVertexShaderCode), nullptr, &m_VertexShader);
	//if (FAILED(hr))
	//	OutputDebugStringA("Failed to create vertex shader.\n");
	//hr = m_Device->CreatePixelShader(kPixelShaderCode, sizeof(kPixelShaderCode), nullptr, &m_PixelShader);
	//if (FAILED(hr))
	//	OutputDebugStringA("Failed to create pixel shader.\n");

	//// input layout
	//if (m_VertexShader)
	//{
	//	D3D11_INPUT_ELEMENT_DESC s_DX11InputElementDesc[] =
	//	{
	//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	};
	//	m_Device->CreateInputLayout(s_DX11InputElementDesc, 2, kVertexShaderCode, sizeof(kVertexShaderCode), &m_InputLayout);
	//}

	//// render states
	//D3D11_RASTERIZER_DESC rsdesc;
	//memset(&rsdesc, 0, sizeof(rsdesc));
	//rsdesc.FillMode = D3D11_FILL_SOLID;
	//rsdesc.CullMode = D3D11_CULL_NONE;
	//rsdesc.DepthClipEnable = TRUE;
	//m_Device->CreateRasterizerState(&rsdesc, &m_RasterState);

	//D3D11_DEPTH_STENCIL_DESC dsdesc;
	//memset(&dsdesc, 0, sizeof(dsdesc));
	//dsdesc.DepthEnable = TRUE;
	//dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	//dsdesc.DepthFunc = GetUsesReverseZ() ? D3D11_COMPARISON_GREATER_EQUAL : D3D11_COMPARISON_LESS_EQUAL;
	//m_Device->CreateDepthStencilState(&dsdesc, &m_DepthState);

	//D3D11_BLEND_DESC bdesc;
	//memset(&bdesc, 0, sizeof(bdesc));
	//bdesc.RenderTarget[0].BlendEnable = FALSE;
	//bdesc.RenderTarget[0].RenderTargetWriteMask = 0xF;
	//m_Device->CreateBlendState(&bdesc, &m_BlendState);
}


void RenderAPI_D3D11::ReleaseResources()
{
	//SAFE_RELEASE(m_VB);
	//SAFE_RELEASE(m_CB);
	//SAFE_RELEASE(m_VertexShader);
	//SAFE_RELEASE(m_PixelShader);
	//SAFE_RELEASE(m_InputLayout);
	//SAFE_RELEASE(m_RasterState);
	//SAFE_RELEASE(m_BlendState);
	//SAFE_RELEASE(m_DepthState);
}


//void RenderAPI_D3D11::DrawSimpleTriangles(const float worldMatrix[16], int triangleCount, const void* verticesFloat3Byte4)
//{
//	ID3D11DeviceContext* ctx = NULL;
//	m_Device->GetImmediateContext(&ctx);
//
//	// Set basic render state
//	ctx->OMSetDepthStencilState(m_DepthState, 0);
//	ctx->RSSetState(m_RasterState);
//	ctx->OMSetBlendState(m_BlendState, NULL, 0xFFFFFFFF);
//
//	// Update constant buffer - just the world matrix in our case
//	ctx->UpdateSubresource(m_CB, 0, NULL, worldMatrix, 64, 0);
//
//	// Set shaders
//	ctx->VSSetConstantBuffers(0, 1, &m_CB);
//	ctx->VSSetShader(m_VertexShader, NULL, 0);
//	ctx->PSSetShader(m_PixelShader, NULL, 0);
//
//	// Update vertex buffer
//	const int kVertexSize = 12 + 4;
//	ctx->UpdateSubresource(m_VB, 0, NULL, verticesFloat3Byte4, triangleCount * 3 * kVertexSize, 0);
//
//	// set input assembler data and draw
//	ctx->IASetInputLayout(m_InputLayout);
//	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	UINT stride = kVertexSize;
//	UINT offset = 0;
//	ctx->IASetVertexBuffers(0, 1, &m_VB, &stride, &offset);
//	ctx->Draw(triangleCount * 3, 0);
//
//	ctx->Release();
//}
//
//
//void* RenderAPI_D3D11::BeginModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int* outRowPitch)
//{
//	const int rowPitch = textureWidth * 4;
//	// Just allocate a system memory buffer here for simplicity
//	unsigned char* data = new unsigned char[rowPitch * textureHeight];
//	*outRowPitch = rowPitch;
//	return data;
//}
//
//
//void RenderAPI_D3D11::EndModifyTexture(void* textureHandle, int textureWidth, int textureHeight, int rowPitch, void* dataPtr)
//{
//	ID3D11Texture2D* d3dtex = (ID3D11Texture2D*)textureHandle;
//	assert(d3dtex);
//
//	ID3D11DeviceContext* ctx = NULL;
//	m_Device->GetImmediateContext(&ctx);
//	// Update texture data, and free the memory buffer
//	ctx->UpdateSubresource(d3dtex, 0, NULL, dataPtr, rowPitch, 0);
//	delete[] (unsigned char*)dataPtr;
//	ctx->Release();
//}
//
//
//void* RenderAPI_D3D11::BeginModifyVertexBuffer(void* bufferHandle, size_t* outBufferSize)
//{
//	ID3D11Buffer* d3dbuf = (ID3D11Buffer*)bufferHandle;
//	assert(d3dbuf);
//	D3D11_BUFFER_DESC desc;
//	d3dbuf->GetDesc(&desc);
//	*outBufferSize = desc.ByteWidth;
//
//	ID3D11DeviceContext* ctx = NULL;
//	m_Device->GetImmediateContext(&ctx);
//	D3D11_MAPPED_SUBRESOURCE mapped;
//	ctx->Map(d3dbuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
//	ctx->Release();
//
//	return mapped.pData;
//}
//
//
//void RenderAPI_D3D11::EndModifyVertexBuffer(void* bufferHandle)
//{
//	ID3D11Buffer* d3dbuf = (ID3D11Buffer*)bufferHandle;
//	assert(d3dbuf);
//
//	ID3D11DeviceContext* ctx = NULL;
//	m_Device->GetImmediateContext(&ctx);
//	ctx->Unmap(d3dbuf, 0);
//	ctx->Release();
//}

#endif // #if SUPPORT_D3D11
