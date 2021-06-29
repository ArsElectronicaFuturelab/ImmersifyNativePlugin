#include "RenderAPI.h"
#include "PlatformBase.h"
#include "Unity/IUnityGraphics.h"


const char *UnityGfxRendererToString( UnityGfxRenderer api )
{
	switch( api )
	{
	case kUnityGfxRendererOpenGL:return "Legacy OpenGL";
	case kUnityGfxRendererD3D9: return "Direct3D 9";
	case kUnityGfxRendererD3D11: return "Direct3D 11";
	case kUnityGfxRendererGCM: return "PlayStation 3";
	case kUnityGfxRendererNull: return "NULL device (used in batch mode)";
	case kUnityGfxRendererXenon: return "Xbox 360";
	case kUnityGfxRendererOpenGLES20: return "OpenGL ES 2.0";
	case kUnityGfxRendererOpenGLES30: return "OpenGL ES 3.x";
	case kUnityGfxRendererGXM: return "PlayStation Vita";
	case kUnityGfxRendererPS4: return "PlayStation 4";
	case kUnityGfxRendererXboxOne: return "Xbox One";
	case kUnityGfxRendererMetal: return "iOS Metal";
	case kUnityGfxRendererOpenGLCore: return "OpenGL core";
	case kUnityGfxRendererD3D12: return "Direct3D 12";
	}

	return "UNKNOWN";
}

RenderAPI* CreateRenderAPI( UnityGfxRenderer apiType )
{
#	if SUPPORT_D3D11
	if (apiType == kUnityGfxRendererD3D11)
	{
		extern RenderAPI* CreateRenderAPI_D3D11();
		return CreateRenderAPI_D3D11();
	}
#	endif // if SUPPORT_D3D11

#	if SUPPORT_D3D9
	if (apiType == kUnityGfxRendererD3D9)
	{
		extern RenderAPI* CreateRenderAPI_D3D9();
		return CreateRenderAPI_D3D9();
	}
#	endif // if SUPPORT_D3D9

#	if SUPPORT_D3D12
	if (apiType == kUnityGfxRendererD3D12)
	{
		extern RenderAPI* CreateRenderAPI_D3D12();
		return CreateRenderAPI_D3D12();
	}
#	endif // if SUPPORT_D3D9


#	if SUPPORT_OPENGL_UNIFIED
	if (apiType == kUnityGfxRendererOpenGLCore || apiType == kUnityGfxRendererOpenGLES20 || apiType == kUnityGfxRendererOpenGLES30)
	{
		extern RenderAPI* CreateRenderAPI_OpenGLCoreES(UnityGfxRenderer apiType);
		return CreateRenderAPI_OpenGLCoreES(apiType);
	}
#	endif // if SUPPORT_OPENGL_UNIFIED

#	if SUPPORT_OPENGL_LEGACY
	if (apiType == kUnityGfxRendererOpenGL)
	{
		extern RenderAPI* CreateRenderAPI_OpenGL2();
		return CreateRenderAPI_OpenGL2();
	}
#	endif // if SUPPORT_OPENGL_LEGACY

#	if SUPPORT_METAL
	if (apiType == kUnityGfxRendererMetal)
	{
		extern RenderAPI* CreateRenderAPI_Metal();
		return CreateRenderAPI_Metal();
	}
#	endif // if SUPPORT_METAL
	
	// Unknown or unsupported graphics API
	return NULL;
}
