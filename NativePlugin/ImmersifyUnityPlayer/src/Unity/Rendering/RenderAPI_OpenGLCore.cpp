#include <Unity/Rendering/RenderAPI.h>
#include <Unity/Rendering/PlatformBase.h>

// OpenGL Core implementation of RenderAPI.

#if SUPPORT_OPENGL_CORE

#include <assert.h>
#include <glTextureAccess.h>
#include "TextureFormats.h"
#include "Utils.h"


class RenderAPI_OpenGLCore : public RenderAPI
{
public:
	RenderAPI_OpenGLCore();
	virtual ~RenderAPI_OpenGLCore() { }

	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces) override;
	virtual BaseTextureAccess* InitTexture(uintptr_t texturePtr[], unsigned int width, unsigned int height, int format, CHROMA_SUBSAMPLING chroma_subsampling) override;
	//virtual void SafeDeleteTexture() override;
	//virtual void ResetBufferIndex() override;
	//virtual bool RenderAPI_OpenGLCore::TextureAccessIsReady() override;
	//virtual void SetPicIsStrided(bool isStrided) override;
	//virtual bool ApplyPictureData(const Spin_Picture* pic) override;

private:
	void CreateResources();
	void ReleaseResources();

private:
	GLenum err;
	//unsigned int m_textureDepth = 0;
};


RenderAPI* CreateRenderAPI_OpenGLCore()
{
	return new RenderAPI_OpenGLCore();
}


RenderAPI_OpenGLCore::RenderAPI_OpenGLCore()
{
}

//void RenderAPI_OpenGLCore::SafeDeleteTexture()
//{
//	if (m_textureAccess)
//	{
//		delete m_textureAccess;
//		m_textureAccess = nullptr;
//	}
//}

//void RenderAPI_OpenGLCore::ResetBufferIndex()
//{
//	if (m_textureAccess && m_textureAccess->isReady())
//	{
//		m_textureAccess->clearBuffer();
//	}
//}

//bool RenderAPI_OpenGLCore::TextureAccessIsReady()
//{
//	return (m_textureAccess != nullptr && m_textureAccess->isReady());
//}

//void RenderAPI_OpenGLCore::SetPicIsStrided(bool isStrided)
//{
//	m_textureAccess->setPicIsStrided(isStrided);
//}

//bool RenderAPI_OpenGLCore::ApplyPictureData(const Spin_Picture* pic)
//{
//	return m_textureAccess->applyPictureData(pic);
//}


void RenderAPI_OpenGLCore::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	switch (type)
	{
	case kUnityGfxDeviceEventInitialize:
	{
		glewExperimental = GL_TRUE;
		err = glewInit();
		CreateResources();
		break;
	}
	case kUnityGfxDeviceEventShutdown:
		ReleaseResources();
		break;
	}
}

BaseTextureAccess* RenderAPI_OpenGLCore::InitTexture(uintptr_t texturePtr[], unsigned int width, unsigned int height, int format, CHROMA_SUBSAMPLING chroma_subsampling)
{
	// This is done within StreamingPlugin:
	GlTextureAccess* textureAccess = new GlTextureAccess(texturePtr, width, height, chroma_subsampling);
	//m_textureDepth = depth;

	return textureAccess;
}

void RenderAPI_OpenGLCore::CreateResources()
{
	// Nothing to do here...
}

void RenderAPI_OpenGLCore::ReleaseResources()
{
	// Nothing to do here...
}

#endif // #if SUPPORT_OPENGL_CORE
