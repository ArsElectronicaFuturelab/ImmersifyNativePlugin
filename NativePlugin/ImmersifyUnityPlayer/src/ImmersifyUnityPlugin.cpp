#include "Unity/Rendering/PlatformBase.h"
#include "Unity/Rendering/RenderAPI.h"
#include "Unity/IUnityGraphics.h"

#include "Utils.h"
#include <spincommon.h>
#include "Console.h"
#include "Sequencer.h"
#include "glTextureAccess.h"
#include "DxTextureAccess.h"

using namespace std;

static RenderAPI* s_CurrentAPI = NULL;
static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;

static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;

// ======================
// Unity Event Functions:

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
	if (eventType == kUnityGfxDeviceEventInitialize)
	{
		s_DeviceType = s_Graphics->GetRenderer();

		if (s_DeviceType == kUnityGfxRendererOpenGLCore)
		{
			extern RenderAPI* CreateRenderAPI_OpenGLCore();
			s_CurrentAPI = CreateRenderAPI_OpenGLCore();
		}
		else if (s_DeviceType == kUnityGfxRendererD3D11)
		{
			extern RenderAPI* CreateRenderAPI_D3D11();
			s_CurrentAPI = CreateRenderAPI_D3D11();
		}
	}

	if (s_CurrentAPI)
	{
		s_CurrentAPI->ProcessDeviceEvent(eventType, s_UnityInterfaces);
	}
}

void UNITY_INTERFACE_API OnRenderEventFunc(int eventID, void *sequencer)
{
	try
	{
		if (sequencer != nullptr)
		{
			if (eventID < 0)
			{
				Sequencer *s = static_cast<Sequencer*>(sequencer);
				delete s;
				return;
			}

			static_cast<Sequencer*>(sequencer)->update();
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "caught exception in " << __func__ << ": " << e.what() << std::endl;
	}
}

// =================================
// Unity Plugin Interface Functions:

extern "C" intptr_t UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API CreateSequencer(char* path, int numberOfThreads = -1, int sizeOfVideoBuffer = -1, int maxQueueSize = 16, bool shouldLog = false)
{
	Sequencer* sequencer = new Sequencer();
	sequencer->setDecoderOptions(sizeOfVideoBuffer, numberOfThreads, maxQueueSize, shouldLog);
	VideoInformation videoInformation = sequencer->loadMP4(path);

	return (intptr_t)sequencer;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API InitPlayer(Sequencer *sequencer, uintptr_t texturePtr1, uintptr_t texturePtr2, uintptr_t texturePtr3, int format)
{
	uintptr_t texturePtrArr[3];
	texturePtrArr[0] = texturePtr1;
	texturePtrArr[1] = texturePtr2;
	texturePtrArr[2] = texturePtr3;

	const VideoInformation& vi = sequencer->getVideoInformation();
	BaseTextureAccess* textureAccess = s_CurrentAPI->InitTexture(texturePtrArr, vi.width, vi.height, format, vi.chroma_subsampling);
	sequencer->setTextureAccess(textureAccess);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API Play(Sequencer *sequencer, float framerate, bool shouldLoop)
{
	// Depending on type:
	BaseTextureAccess* baseTextureAccess = sequencer->getTextureAccess();
	if (s_DeviceType == kUnityGfxRendererOpenGLCore)
	{
		// We already have PBO's initialized. so we have to skip the last frame
		((GlTextureAccess*)baseTextureAccess)->clearBuffer();
	}

	const VideoInformation& videoInformation = sequencer->getVideoInformation();

	if (framerate < 0.f) // e.g. because framerate was set to -1
	{
		framerate = (float)videoInformation.fps;
	}

	sequencer->play(framerate, shouldLoop);
}

extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API IsReady(Sequencer *sequencer)
{
	return sequencer->isReady();
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SeekToMSec(Sequencer *sequencer, int targetTimeMS)
{
	if (sequencer->getSeekingIsSupported() && targetTimeMS >= 0 && targetTimeMS < sequencer->getVideoInformation().durationMS)
	{
		sequencer->seekToMSec(targetTimeMS);
	}
}

extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetSeekingIsSupported(Sequencer *sequencer)
{
	return sequencer->getSeekingIsSupported();
}

extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API IsFinished(Sequencer *sequencer)
{
	return sequencer->isFinished();
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetPause(Sequencer *sequencer, bool pause)
{
	sequencer->setPause(pause);
}

extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API IsPaused(Sequencer *sequencer)
{
	return sequencer->isPaused();
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API PauseAfterFirstFrame(Sequencer *sequencer, bool pauseAfFrame)
{
	sequencer->pauseAfterFirstFrame(pauseAfFrame);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API OverrideTargetPlayingTime(Sequencer *sequencer, float targetTime)
{
	sequencer->setTargetPlayingTime(targetTime);
}

extern "C" float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetCurrentPlayingTime(Sequencer *sequencer)
{
	return (float)sequencer->getCurrentPlayingTime();
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API OverrideTargetFPS(Sequencer *sequencer, float fps)
{
	sequencer->overrideTargetFPS(fps);
}

extern "C" float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetTargetFPS(Sequencer *sequencer)
{
	return (float)sequencer->getTargetFPS();
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetVideoWidth(Sequencer *sequencer)
{
	return sequencer->getVideoInformation().width;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetVideoHeight(Sequencer *sequencer)
{
	return sequencer->getVideoInformation().height;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetVideoChromaSubsampling(Sequencer *sequencer)
{
	return sequencer->getVideoInformation().chroma_subsampling;
}

extern "C" int64_t UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetVideoDurationInMS(Sequencer* sequencer)
{
	return sequencer->getVideoInformation().durationMS;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetMaxQueueSize(Sequencer *sequencer)
{
	return sequencer->getMaxQueueSize();
}

extern "C" UnityRenderingEventAndData UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc()
{
	return OnRenderEventFunc;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetCurrentErrorCode(Sequencer *sequencer)
{
	return (int)sequencer->getCurrentErrorCode();
}

extern "C" void	UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
	s_UnityInterfaces = unityInterfaces;
	s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
	s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
	// Run OnGraphicsDeviceEvent(initialize) manually on plugin load
	OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
	s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}
