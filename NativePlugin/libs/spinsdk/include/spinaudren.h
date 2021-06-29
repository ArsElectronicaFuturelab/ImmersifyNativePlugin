/**
 * \file    spinaudren.h
 * \brief
 * \author  Chi Ching Chi <chi@spin-digital.com>
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2019-2020, Spin Digital Video Technologies GmbH
 * Berlin, Germany
*/

#pragma once

/** \defgroup audio SpinAudRen
 *  \brief Spin Digital audio rendering API.
 *  @{
 */

#include "spincommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Audio devices types
 */
typedef enum SE_AudioDeviceType
{
  SE_ADEVTYPE_None = -1,
  SE_ADEVTYPE_WASAPI,
  SE_ADEVTYPE_AJA,
  SE_ADEVTYPE_ASIO,         ///< Currently unsupported
  SE_NUM_AudioDeviceTypes,
} SE_AudioDeviceType;

/**
 * Audio output types
 */
typedef enum SE_AudioIOType
{
  SE_AIOTYPE_None = -1,
  SE_AIOTYPE_SDI,
  SE_AIOTYPE_AES,
  SE_NUM_AudioIOTypes,
} SE_AudioIOType;


/**
 * Description of an audio render output. An audio device can have one or more outputs.
 *
 * @see SpinAudRen_Adapter
 */
typedef struct SpinAudRen_Output
{
  char acDescription[32];       ///< First 32 characters of the output name if available.
  SE_AudioIOType eAudOutType;   ///< IO type of audio. Currently only defined for AJA devices.
  int iNumChannels;             ///< Number of channels the output supports.
  int iSampleRate;              ///< Sample rate of the output.
  SE_SampleFormat eSampleFmt;   ///< Sample format of the output.
} SpinAudRen_Output;

/**
 * SpinAudRen_Adapter description.
 */
typedef struct SpinAudRen_Adapter
{
  char acDescription[128];      ///< First 128 characters of the adapter name if available.
  int iDeviceId;                ///< Global audio device id.
  SE_AudioDeviceType eDevType;  ///< Device type.
  int iLocalId;                 ///< ID within device type family.
  int iCapabilties;             ///< Adapter capabilities (currently not set).

  /**
   * Number of render outputs associated to this adapter
   */
  int iNumOutputs;
  SpinAudRen_Output sOutputs[16];
} SpinAudRen_Adapter;

//Fixme replace with Spin_AudioBlock?
typedef struct SpinAudRen_Frames
{
  void* pAudioSamples;
  int iNumFrames;
  int64_t llPts;        ///< Presentation time stamp of first sample
} SpinAudRen_Frames;

/**
 * SpinAudRen configuration struct for initializing the renderer context
 *
 * @see SpinAudRenLib_Open
 */
typedef struct SpinAudRen_Param
{
  int iNumOutputs;          ///< Number of audio outputs to use
  /**
   * Max buffer size in ms.
   * After this buffer size is reached SpinAudRenLib_RenderAudioFrames will block until enough buffer space is available.
   */
  float fBufferSizeMs;
  /**
    * Internal processing maximum block size. Input blocks should be a multiple of iBlkSize for best performance.
    * Minimum size is 128.
    */
  int iBlkSize;
  int iSampleRate;          ///< Sample rate of the input streams
  int iNumChannels;         ///< Num audio channels in the input stream
  SE_SampleFormat eSampleFmt;
  /**
   * Device and audio output configuration for multi device operation.
   */
  struct {
    unsigned char ucDevID;
    unsigned char ucOutID;
    int iNumChannels;
  } asDevs[16];

  int iTimeBase;                  ///< Time base to use to convert from PTS to real-time in seconds.

  SE_RefTimeMeth eRefTimeMode;    ///< Reference time update method.

  typedef double(*ExtTimeFunc)(void* pHandle);  ///< External ref time callback prototype.
  /**
   * Callback function for retrieving an external ref time. Must return the time in seconds.
   * If the callback function is not set, an internal function is used for timing the presentation
   * of video frames.
   */
  ExtTimeFunc fRefTimeSec;
  void* hRefTimeHandle;           ///< Callback handle for fRefTimeSec callback function.


  typedef void(*ProcessAudioFunc)(void* pHandle, int iSamples, int iNumInCh, float* pInData, int iNumOutCh, float* pOutData);
  ProcessAudioFunc fProcessAudio; ///< External function for processing audio
  void* hProcessAudio;            ///< Callback handle for external audio processing

  SD_ExtLogFunc fLogFunc;     ///< Callback function for log messages.
  void* hLogHandle;           ///< Callback handle for fLogFunc callback function.

} SpinAudRen_Param;


/**
 * Audio renderer statistics
 *
 */
typedef struct SpinAudRen_Statistics
{
  int64_t lLastPts;
} SpinAudRen_Statistics;

/**
 * Supported audio render control operations. Control operation are performed asynchronously.
 *
 * @see SpinAudRenLib_Control
 * @see SpinAudRen_CtrlOpDesc
 */
typedef enum SE_AudRenCtrlOp
{
  SE_ARCOP_Nop = 0,
  SE_ARCOP_Exit,           ///< Used internally in SpinRenderLib_Close. It is not recommended to use directly.
  SE_ARCOP_Pause,          ///< Pause/continue rendering
  /**
   * Explicitly set a new reference time. Should be performed after a non-linear operation when the
   * rendering context is configure to use the SE_REFTIME_Explicit reference time mode.
   *
   * @see eRefTimeMode
   */
  SE_ARCOP_SetRefTime,
  SE_NUM_AudRenCtrlOps
} SE_AudRenCtrlOp;


/**
 * Audio control operation descriptor. Control operation are performed asynchronously.
 *
 * @see SpinAudioLib_Control
 * @see SE_AudioCtrlOp
 */
typedef struct SpinAudRen_CtrlOpDesc
{
  SE_AudRenCtrlOp eOpType; ///< Control operation type.
  union Data {
    int bPause;            ///< Set to 0 to continue rendering and to 1 to bPause the rendering. @see SE_RCOP_Pause
    double dRefTime;       ///< The new reference time. @see SE_RCOP_SetRefTime
  } data;
} SpinAudRen_CtrlOpDesc;

typedef void* SpinAudRenLib_Handle;

/**
 * Return the available audio devices.
 *
 * @param ppAdapters Pointer in which the library will return the array pointer of SpinAudio_Devices.
 * @param piNumAdapters Pointer to the number of devices found in the system.
 *
 * @see SpinAudRen_Adapter
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinAudRenLib_GetAdapters(const SpinAudRen_Adapter** ppAdapters, int* piNumAdapters);

/**
 * Open a audio render context.
 *
 * @param phRender The audio context handle is returned in *phAudRen.
 * @param psParam  Pointer to the SpinAudio_Param struct to initialize the audio IO context.
 *
 * @see SpinAudRen_Param
 * @see SpinAudRenLib_Close
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinAudRenLib_Open(SpinAudRenLib_Handle* phAudRen, const SpinAudRen_Param* psParam);

/**
 * Close the audio render context.
 *
 * @param phAudRen The audio render context handle to close. *phAudRen is set to NULL after successfully
 * closing the context.
 *
 * @see SpinAudRenLib_Open
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinAudRenLib_Close(SpinAudRenLib_Handle* phAudRen);

/**
 * Check if there is enough space available in the audio render queue.
 * When enough space is available, SpinAudRenLib_RenderAudioFrames is guarenteed to not block up to iNumFrames audio frames.
 *
 * @param hAudRen The audio render context handle.
 *
 * @return >0 if there is enough space available for iNumFrames audio frames.
 */
SPINLIB_API int SpinAudRenLib_HasFreeAudioFrames(const SpinAudRenLib_Handle hAudRen, int iNumFrames);

/**
 * Render an audio frame to an output. Available in output mode only.
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinAudRenLib_RenderAudioFrames(SpinAudRenLib_Handle hAudRen, SpinAudRen_Frames* pFrames);


/**
 * A blocking call that invalidates all queued samples. Available in output mode only.
 *
 * @param hAudRen The audio context handle.
 *
 * @return Returns SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinAudRenLib_InvalidateAudioQueues(SpinAudRenLib_Handle hAudRen);

/**
 * A blocking call that flushes the internal queued. This call returns after all queued samples
 * have been send to the output.
 *
 * @param hAudRen The renderer context handle.
 *
 * @return Returns SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinAudRenLib_FlushAudioQueues(SpinAudRenLib_Handle hAudRen);

/**
 * Issue asynchronous control operation. To achieve high performance, the internal implementation of the video
 * rendering pipeline has asynchronous behavior. To keep the high performance and smooth operation the control
 * operations must also be asynchronous when possible.
 *
 * @param hRender The renderer context handle.
 * @param pOpDesc The rendering control operation.
 *
 * @see SpinAudRen_CtrlOpDesc
 *
 */
SPINLIB_API int SpinAudRenLib_Control(SpinAudRenLib_Handle hAudRen, const SpinAudRen_CtrlOpDesc* pOpDesc);

/**
 * Fills SpinAudio_Param* psParam with the default values.
 * Typical usage is to fill the default parameters and override specific variables only.
 *
 * @param psParam Pointer to the to be filled SpinAudRen_Param struct.
 * @see SpinAudRen_Param
 * @return Returns SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinAudRenLib_GetDefaultParams(SpinAudRen_Param* psParam);


/**
 * Return the reference time in seconds of the first audio device
 */
SPINLIB_API double SpinAudRenLib_GetRefTime(SpinAudRenLib_Handle hAudRen);


/**
 * Get latest audio rendering statistics
 */
SPINLIB_API int SpinAudRenLib_GetStats(SpinAudRenLib_Handle hAudRen, SpinAudRen_Statistics* pStat);

#ifdef __cplusplus
}
#endif

/** @}*/

