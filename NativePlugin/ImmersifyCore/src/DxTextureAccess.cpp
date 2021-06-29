#include "DxTextureAccess.h"
#include "Console.h"
#include "Utils.h"


DxTextureAccess::DxTextureAccess(uintptr_t texturePtr[], unsigned int width, unsigned int height, CHROMA_SUBSAMPLING chroma_subsampling)
	: BaseTextureAccess()
{
	m_d3dtexY = reinterpret_cast<ID3D11Texture2D*>(texturePtr[0]);
	m_d3dtexU = reinterpret_cast<ID3D11Texture2D*>(texturePtr[1]);
	m_d3dtexV = reinterpret_cast<ID3D11Texture2D*>(texturePtr[2]);

	m_width = width;
	m_height = height;
	m_chroma_subsampling = chroma_subsampling;
	m_ready = false;
	m_picIsStrided = false;
}

DxTextureAccess::~DxTextureAccess()
{
	if (m_ctx != nullptr)
	{
		m_ctx->Release();
	}
}

void DxTextureAccess::setDxDevice(ID3D11Device* device) 
{ 
	m_device = device; 
	// Get the ID3D11DeviceContext
	m_device->GetImmediateContext(&m_ctx);
	m_d3dtexY->GetDevice(&m_device);
	m_d3dtexU->GetDevice(&m_device);
	m_d3dtexV->GetDevice(&m_device);
}

bool DxTextureAccess::applyPictureData(const Spin_Picture * pic)
{
	// Update native texture from code
	if (m_d3dtexY)
	{
		const Spin_Plane *planeY = &pic->asPlanes[0];
		unsigned int width = planeY->iWidth * 8;
		// Use ID3D11DeviceContext::UpdateSubresource to fill the default texture with data from a pointer provided by the application.
		m_ctx->UpdateSubresource(m_d3dtexY, 0, nullptr, planeY->pPlane, width, 1); // planeY->iWidth * 8;
	}

	if (m_d3dtexU)
	{
		const Spin_Plane *planeCb = &pic->asPlanes[1];
		unsigned int width = planeCb->iWidth * 8;
		//if (m_chroma_subsampling == CHROMA_SUBSAMPLING::_420)
		//{
		//	width /= 2;
		//}

		m_ctx->UpdateSubresource(m_d3dtexU, 0, nullptr, planeCb->pPlane, width, 1); // planeCb->iWidth * 8
	}

	if (m_d3dtexV)
	{
		const Spin_Plane *planeCr = &pic->asPlanes[2];

		unsigned int width = planeCr->iWidth * 8;
		//if (m_chroma_subsampling == CHROMA_SUBSAMPLING::_420)
		//{
		//	width /= 2;
		//}

		m_ctx->UpdateSubresource(m_d3dtexV, 0, nullptr, planeCr->pPlane, width, 1); // planeCr->iWidth * 8
	}

	if (m_ready == false)
	{
		m_ready = true;
	}

	return true;
}

//// TODO: This cannot work: Question: What exactly is the pointer ptr in the gl implementation? Why only 1 ptr for 3 textures?
//void DxTextureAccess::CopyDataToPtr(const Spin_Picture* pic, ID3D11Texture2D* ptr)
//{
//	const Spin_Plane *planeY = &pic->asPlanes[0];
//	const Spin_Plane *planeCb = &pic->asPlanes[1];
//	const Spin_Plane *planeCr = &pic->asPlanes[2];
//
//	const unsigned char *dataY = reinterpret_cast<unsigned char*>(planeY->pPlane);
//	const unsigned char *dataCb = reinterpret_cast<unsigned char*>(planeCb->pPlane);
//	const unsigned char *dataCr = reinterpret_cast<unsigned char*>(planeCr->pPlane);
//
//	if (!m_pictureIsStrided)
//	{
//		int szHalf = planeY->iHeight * planeY->iWidth * 8;
//		std::memcpy(ptr, dataY, szHalf);
//		std::memcpy(ptr + szHalf, dataCb, (szHalf / 2));
//		std::memcpy(ptr + szHalf + szHalf / 2, dataCr, szHalf / 2);
//	}
//	else
//	{
//		//cout << "strided!!!" << endl;
//		//height,width=>number of blocks, each block contains 8 bytes
//		for (int i = 0; i < planeY->iHeight; i++)
//		{
//			std::memcpy(ptr, dataY + i * planeY->iStride * 8, planeY->iWidth * 8);
//			ptr += planeY->iWidth * 8;
//		}
//
//		for (int i = 0; i < planeCb->iHeight; i++)
//		{
//			std::memcpy(ptr, dataCb + i * planeCb->iStride * 8, planeCb->iWidth * 8);
//			ptr += planeCb->iWidth * 8;
//		}
//
//		for (int i = 0; i < planeCr->iHeight; i++)
//		{
//			std::memcpy(ptr, dataCr + i * planeCr->iStride * 8, planeCr->iWidth * 8);
//			ptr += planeCr->iWidth * 8;
//		}
//	}
//}