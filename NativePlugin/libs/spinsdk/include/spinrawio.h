/**
 * \file    spinrawio.h
 * \brief   Spin Digital RawIO API header file
 * \author  Chi Ching Chi <chi@spin-digital.com>
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2020-, Spin Digital Video Technologies GmbH
 * Berlin, Germany
 */

#pragma once

#include "spincommon.h"

/** \defgroup rawio SpinRawIO
 *  \brief Raw video file and device IO.
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
* Render devices types
*/
typedef enum SE_RawIODeviceType
{
  SE_IODEV_None = -1,
  SE_IODEV_FILE,
  SE_IODEV_AJASDI,
  SE_NUM_RawIODeviceType,
} SE_RawIODeviceType;

/**
* File access mode. Used only for file devices.
*/
typedef enum SE_FileAccessMode
{
  SE_FAM_FSTREAM,
  SE_FAM_DIRECTCOPY,  //Direct transfer to temporary buffer and copy and realign from there into the picture planes
  SE_FAM_DIRECT,      //Direct transfer to picture planes. Only usable for pictures without margins and that are aligned to the disk sectory size.
} SE_FileAccessMode;

/**
* Description of a raw IO port.
*
* @see SpinRawIO_Adapter
*/
typedef struct SpinRawIO_Port
{
  char acDescription[32];       ///< First 32 characters of the output name if available.
  bool bInput;                  ///< True if it is an input port

  int iWidth;                   ///< Typical or maximum video width
  int iHeight;                  ///< Typical or maximum video height

  Spin_AudioFormat sAudFmt;     ///< Typical or maximum suported audio format
} SpinRawIO_Port;

/**
* SpinRawIO_Adapter description.
*/
typedef struct SpinRawIO_Adapter
{
  char acDescription[128];      ///< First 128 characters of the adapter name if available.
  int iDeviceId;                ///< Global io device id.
  SE_RawIODeviceType eDevType;  ///< Device type.
  int iLocalId;                 ///< ID within device type family.
  int iCapabilties;             ///< Adapter capabilities (currently not set).

  int iNumPorts;
  SpinRawIO_Port sPorts[16];
} SpinRawIO_Adapter;


/**
 * SpinRawIO configuration struct for initializing the stream context
 *
 * @see SpinRawIOLib_Open
 */
typedef struct SpinRawIO_Param
{
  int iAdapt;     ///< Adapter index in adapter array returned by SpinawIOLib_GetAdapters
  int iPort;      ///< Port index of SpinRawIO_Adapter

  int iAudioBlkSize;    ///< Number of audio frames returned in a block when capturing.

  //// File device only options
  /**
   * List of files that form an virtual file.
   * Each individual file can be followed by a ? and & separated option list.
   * Ex. sample.yuv?seek=100&duration=200
   * Options:
   * seek: Number of frames to skip from the beginning of the file.
   * duration: Number of valid frames of the video. Must be >1 and <= number of frames in the file.
   * seg_duration: Duration of the segment.
   *          Can be larger than duration, which results in looping of the frames covered
   *          by 'duration' starting from the 'seek' position until 'seg_duration' number of
   *          frames is reached.
   */
  const char** ppFileURLs;
  int iNumFileURLs;               ///< Number of files
  Spin_VideoFormat sFileVidFmt;   ///< Raw video format
  SE_FileAccessMode eFAM;         ///< File access mode
  bool bLoop;                     ///< Loop files from beginning after reaching the end
  /**
  * When set, frames are read at the framerate indicated in the sFileVidFmt.
  * In case the storage medium is too slow, the reading of frames are skipped in order to match real time.
  * In this mode frames are also dropped when application does not acquire the available frames in time.
  * When the next frame is ready by the SpinRawIO context, and the internal ready queue is full,
  * the oldest frame is dropped from the queue.
  */
  bool bRealTime;
  int iFramePeriod;               ///< Instead of reading every frame, only every Nth frames is read, where N is the iFramePeriod.
  int iStartFrameOffset;          ///< Frame offset to start reading from. This offset is in the total virtual file.

  /**
 * Callback function for retrieving log messages.
 * If the callback function is not set, the messages are ignored.
 */
  SD_ExtLogFunc fLogFunc;
  void* hLogHandle;   ///< Callback handle for fLogFunc callback function.

} SpinRawIO_Param;

/**
 * Statistics and configuration struct. It can be queried using SpinRawIOLib_GetStatConf.
 *
 * @see SpinRawIOLib_GetStatConf
 */
typedef struct SpinRawIO_StatConf
{
  Spin_VideoFormat sVidFmt;       ///< Current video format
  Spin_AudioFormat sAudFmt;       ///< Current audio format
  int64_t llLastPtsVid;           ///< PTS of last video frame in sequence. Only set for file input.
  int64_t llCurPtsVid;            ///< PTS of most recent video frame.
  int64_t llMissedFrames;         ///< Missed frames in capture/read device. Misses occur when the capture/read is too slow.
  int64_t llDroppedFrames;        ///< Dropped frames due to full ready queue. Drops occur when the application does not retrieve the frames in time.
  float fLatency;                 ///< Maximum capturing/reading latency of the last 60 video frames.
} SpinRawIO_StatConf;

typedef void* SpinRawIOLib_Handle;

/**
 * Return the available raw IO adapters.
 *
 * @param ppAdapters Pointer in which the library will return the array pointer of SpinRawIO_Adapters (library owns array memory).
 * @param piNumAdapters Pointer to the number of adapters found in the system.
 *
 * @see SpinRawIO_Adapter
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRawIOLib_GetAdapters(const SpinRawIO_Adapter** ppAdapters, int* piNumAdapters);

/**
 * Probes the video and audio format on a certain port.
 * It can be used for monitoring purposes
 *
 * @param adapt Adapter index
 * @param port Port index
 * @param pVidFmt Pointer to the to be filled video format struct
 * @param pAudFmt Pointer to the to be filled audio format struct
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRawIOLib_ProbeFormat(int adapt, int port, Spin_VideoFormat* pVidFmt, Spin_AudioFormat* pAudFmt);

/**
 * Returns the default parameters.
 *
 * @param psParam Pointer to SpinRawIO_Param parameter struct
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API void SpinRawIOLib_GetDefaultParams(SpinRawIO_Param* psParam);

/**
 * Open a rawio context.
 *
 * @param phRawIO  The handle to the opened Raw IO context is returned in *phRawIO.
 * @param psParam  Pointer to the SpinStream_Param struct for initializing the stream context
 *
 * @see SpinRawIO_Param
 * @see SpinRawIOLib_Close
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRawIOLib_Open(SpinRawIOLib_Handle* phRawIO, const SpinRawIO_Param* psParam);

/**
 * Close the rawio render context.
 *
 * @param phRawIO Raw IO context handle to close. *phRawIO is set to NULL after successfully
 * closing the context.
 *
 * @see SpinRawIOLib_Open
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRawIOLib_Close(SpinRawIOLib_Handle* phRawIO);

/**
 * Check if a video frame is available.
 *
 * @param hRawIO Raw IO context handle.
 *
 * @see SpinRawIOLib_GetVideoFrame
 *
 * @return 1 if a buffer is available, 0 otherwise.
 */
SPINLIB_API int SpinRawIOLib_HasVideoFrame(const SpinRawIOLib_Handle hRawIO);

/**
 * Get the next video frame.
 *
 * @param hRawIO  Raw IO context handle.
 * @param pPic    Pointer to Spin_Picture to receive the video frame.
 *                The video frame is mapped into pPic, not copied.
 *                This means that only the struct parameters, including the data pointers,
 *                are copied over to pPic. The planes data themselves are not copied.
 * @param timeoutms Timeout in miliseconds.
                  When set to 0 this call blocks until the next frame is available.
 *
 * @see SpinRawIOLib_ReturnVideoFrame
 * @see SpinRawIOLib_HasVideoFrame
 * @return SD_OK on success, SD_FAIL when no frame is available and timeout is reached.
 *         See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRawIOLib_GetVideoFrame(SpinRawIOLib_Handle hRawIO, Spin_Picture* pPic, int timeoutms);

/**
 * Return a video frame that was acquired with SpinRawIOLib_GetVideoFrame before.
 * Accessing the planes data after the frame is returned may lead to undefined behavior.
 *
 * @param hRawIO Raw IO context handle.
 * @param pPic Pointer to the Spin_Picture that is to be returned
 *
 * @see SpinRawIOLib_GetVideoFrame
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRawIOLib_ReturnVideoFrame(SpinRawIOLib_Handle hRawIO, Spin_Picture* pPic);

/**
 * Check if an audio block is available.
 *
 * @param hRawIO Raw IO context handle.
 *
 * @see SpinRawIOLib_GetVideoFrame
 *
 * @return 1 if a buffer is available, 0 otherwise.
 */
SPINLIB_API int SpinRawIOLib_HasAudioBlock(const SpinRawIOLib_Handle hRawIO);

/**
 * Get the next audio block.
 *
 * @param hRawIO  Raw IO context handle.
 * @param pAudBlk Pointer to Spin_AudioBlock to receive the video frame.
 *                The audio block is mapped into pAudBlk, not copied.
 *                This means that only the struct parameters, including the data pointers,
 *                are copied over to pAudBlk. The sample data is not copied.
 * @param timeoutms Timeout in miliseconds. When set to zero
                  When set to 0 this call blocks until the next frame is available.
 *
 * @see SpinRawIOLib_ReturnAudioBlock
 * @see SpinRawIOLib_HasAudioBlock
 * @return SD_OK on success, SD_FAIL when no audio block is available and timeout is reached.
 *         See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRawIOLib_GetAudioBlock(SpinRawIOLib_Handle hRawIO, Spin_AudioBlock* pAudBlk, int timeoutms);

/**
 * Return an audio block that was acquired with SpinRawIOLib_GetAudioBlock before.
 * Accessing the sample data after the block is returned may lead to undefined behavior.
 *
 * @param hRawIO Rawio context handle.
 * @param pAudBlk Pointer to the Spin_AudioBlock that is to be returned
 *
 * @see SpinRawIOLib_GetAudioBlock
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRawIOLib_ReturnAudioBlock(SpinRawIOLib_Handle hRawIO, Spin_AudioBlock* pAudBlk);


/**
 * Seek the entire stream to a target position.
 * This API on works when using files as input.
 * Seeking does not flush the internal picture queues, nor affects the timestamp generation.
 * It only will recalculate the next frame that should be read based on the seek target.
 * The raw IO context will seek the reading to this frame,
 * and will continue generating time stamps gerenation as if this did not occur.
 *
 * The seeking is performed on the virtual file, akin to how iStartFrameOffset functions.
 *
 * @param hRawIO Raw IO context handle.
 * @param eMode Selects relative or absolute position seek modes.
 * @param dTime Seek time parameter in seconds.
 *              In relative mode this will be used as the seek distance.
 *              In absolute mode it is used as the seek target.
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRawIOLib_Seek(SpinRawIOLib_Handle hRawIO, SE_SeekMode eMode, double dTime);

/**
 * Query the raw IO format and runtime statistics.
 *
 * @param hRawIO RawIO context handle.
 * @param pStatConf Pointer to SpinRawIO_StatConf that is to be filled.
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRawIOLib_GetStatConf(const SpinRawIOLib_Handle hRawIO, SpinRawIO_StatConf* pStatConf);

/**
 * Returns the reference time (in seconds) derived from the capture device.
 * In real-time modes the time returned is related to the generated presentation timestamps,
 * and can be used to schedule transmission or renderering of the video frames and audio packets
 */
SPINLIB_API double SpinRawIOLib_GetRefTime(const SpinRawIOLib_Handle hRawIO);

#ifdef __cplusplus
}
#endif

/**@}*/
