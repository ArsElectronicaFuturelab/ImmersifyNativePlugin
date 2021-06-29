/**
 * \file    spinstream.h
 * \brief   Spin Digital Stream API header file
 * \author  Chi Ching Chi <chi@spin-digital.com>
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2020-, Spin Digital Video Technologies GmbH
 * Berlin, Germany
 */

#pragma once

#include "spincommon.h"

/** \defgroup stream SpinStream
 *  \brief Stream input and output API.
 *
 *  This API allows for compressed file (de)muxing, stream transmission, and stream reception.
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "libavformat/avformat.h"
#include "libavformat/avio.h"

/**
 * Stream devices types
 */
typedef enum SE_StreamDeviceType
{
  SE_SDT_FILE,
  SE_SDT_NIC,           ///< Real-time streaming using OS visible network ports
  SE_SDT_DEKTEC,        ///< Real-time streaming using DekTec device API
  SE_NumStreamDeviceTypes
} SE_StreamDeviceType;

/**
 * Stream port types
 */
typedef enum SE_StreamPortType
{
  SE_SPT_Unknown = -1,
  SE_SPT_FILE,
  SE_SPT_IP,
  SE_SPT_ASI,
  SE_NumStreamPortTypes
} SE_StreamPortType;

/**
* Description of a raw IO port.
*
* @see SpinRawIO_Adapter
*/
typedef struct SpinStream_Port
{
  char acDescription[64];       ///< First 64 characters of the output name if available.
  int iChanType;                ///< input/output/inout
  SE_StreamPortType ePortType;  ///< Port type.

  /**
  * IPv4 addr in network order.
  * Example 192.168.1.10 -> acIp[0] = 192, acIp[1] = 168, acIp[2] = 1, acIp[3] = 10
  */
  uint8_t acIp[4];
  /**
  * MAC addr in network order.
  * Example 00-1B-21-28-07-88 -> acMacAddr[0] = 00, acMacAddr[5] = 88
  */
  uint8_t acMacAddr[6];
} SpinStream_Port;

/**
 * SpinStream_Adapter description.
 */
typedef struct SpinStream_Adapter
{
  char acDescription[128];      ///< Short adapter description.
  int iDeviceId;                ///< Global render device id.
  int64_t iLocalId;             ///< ID within device type family.
  SE_StreamDeviceType eType;    ///< Device type.

  int iNumPorts;
  SpinStream_Port sPorts[16];
} SpinStream_Adapter;

/**
 * Statistics and configuration struct. It can be queried using SpinStreamLib_GetStatConf.
 *
 * @see SpinStreamLib_GetStatConf
 */
typedef struct SpinStream_StatConf
{
  SE_StreamMode eMode;            ///< Streaming mode
  int iDroppedFrames;             ///< Total dropped frames (transmission only)
  float fTransmitLatencyMax;      ///< Maximum transmit latency of last 1s
  float fTransmitLatencyMin;      ///< Min transmit latency of last 1s
  float fAvgTransmitDelayDiff;    ///< Transmit latency difference compared guidance values. Average in last 1s
  float fFpsSid0;                 ///< Fps of first stream id (transmission only)
} SpinStream_StatConf;


/**
 * SpinStream configuration struct for initializing the stream context
 *
 * @see SpinStreamLib_Open
 */
typedef struct SpinStream_Param
{
  int iAdapt;   ///< Adapter index in adapter array returned by SpinStreamLib_GetAdapters
  int iPort;    ///< Port index of SpinStream_Adapter

  /**
   * Callback function for retrieving an external ref time. Must return the time in seconds.
   * When the callback function is not set or when in input mode, an internal time function is used instead.
   */
  Spin_ExtTimeFunc fRefTimeSec;
  void* hRefTimeHandle;         ///< Callback handle for fRefTimeSec callback function.

  /**
   * Callback function for retrieving log messages.
   * If the callback function is not set, the messages are ignored.
   */
  SD_ExtLogFunc fLogFunc;
  /**
   * Callback handle for fLogFunc callback function.
   */
  void* hLogHandle;

  bool bInOut;                  ///< 0:input 1:output

  /**
   * URL/File + options.
   * From this parameter the protocol and containers are derived.
   * Example valid file extension: .mp4 .mkv .ts .hevc
   * Example valid stream url: rtp://x.x.x.x:5004
   *
   * Additional options can be provided behind the url separated with a ? and & for each subsequent option.
   * For instance to mux and send TS over RTP with a 10Mbps muxrate:
   * rtp://x.x.x.x:5004?fmt=mpegts&muxrate=10000000
   */
  const char* pUrlOpt;

  /**
  * Transmission buffer size. Only used for outputs.
  * Also used in TS muxer to set the maximum delay (maximum pcr-dts difference)
  */
  float fBufferSize;
  /**
  * Maximum analyze duration to detect formats. Only used for inputs.
  */
  float fAnalyzeDuration;

};

typedef void* SpinStreamLib_Handle;

/**
 * Return the available stream adapters.
 *
 * @param ppAdapters Pointer in which the library will return the array pointer of SpinStream_Adapters (library owns array memory).
 * @param piNumAdapters Pointer to the number of adapters found in the system.
 *
 * @see SpinStream_Adapter
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinStreamLib_GetAdapters(const SpinStream_Adapter** ppAdapters, int* piNumAdapters);

/**
 * Returns the default parameters.
 *
 * @param psParam Pointer to SpinStream_Param parameter struct
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API void SpinStreamLib_GetDefaultParams(SpinStream_Param* psParam);

/**
 * Open a stream context.
 *
 * @param phStream The stream context handle is returned in *phStream.
 * @param psParam  Pointer to the SpinStream_Param struct for initializing the stream context
 *
 * @see SpinStream_Param
 * @see SpinStreamLib_Close
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinStreamLib_Open(SpinStreamLib_Handle* phStream, const SpinStream_Param* psParam);

/**
 * Initialize the stream render context. Only required when used as outputs.
 * After opening a context, streams have to added to configure the muxer.
 * The context has to be initialized after all streams are added.
 * After the context is intialized streams cannot be added any longer.
 *
 * Streams are added using the FFmpeg libavfmt API.
 * The internal FFmpeg AVFormatContext struct can be retrieved with SpinStreamLib_GetAVFormatContext.
 *
 * @param hStream The stream context to initialize
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */

SPINLIB_API int SpinStreamLib_Init(SpinStreamLib_Handle hStream);

/**
 * Close the stream render context.
 *
 * @param phStream The stream context handle to close. *phStream is set to NULL after successfully
 * closing the context.
 *
 * @see SpinStreamLib_Open
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinStreamLib_Close(SpinStreamLib_Handle* phStream);

/**
 * Get the next AVPacket. For inputs only.
 *
 * @param hStream Stream context handle.
 * @param ppPkt Pointer in which the AVPacket pointer is retrieved (library owns AVPacket)
 *
 * @see SpinStreamLib_ReturnPacket
 * @return SD_OK on success.
 *         AVERROR(EAGAIN) is when no packet is available.
 *         AVERROR_EOF is returned in case the end of file is reached.
 *         See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinStreamLib_GetPacket(SpinStreamLib_Handle hStream, AVPacket** ppPkt);

/**
 * Return an AVPacket that was acquired with SpinStreamLib_GetPacket before. For inputs only.
 *
 * @param hStream Stream context handle.
 * @param ppPkt Pointer to the AVPacket that is to be returned
 *
 * @see SpinStreamLib_GetPacket
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinStreamLib_ReturnPacket(SpinStreamLib_Handle hStream, AVPacket *pPkt);

/**
 * Mux and send/write an AVPacket in the output stream.
 * After completing this action the AVPacket is unreferenced using av_unref_packet.
 * AVPackets have to be initialized and allocated using the FFmpeg avformat API.
 *
 * @param hStream Stream context handle.
 * @param ppPkt Pointer to the AVPacket that has to be send/muxed
 *
 * @see SpinStreamLib_GetPacket
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinStreamLib_SendPacket(SpinStreamLib_Handle hStream, AVPacket* pPkt);

/**
 * Seek the entire stream to a target position.
 *
 * @param hStream Stream context handle.
 * @param eMode Selects relative or absolute position seek modes.
 * @param ts Timestamp for seeking. In relative mode this will be used as the seek distance. In absolute mode it is used as the seek target.
 * @param sid Stream ID to use for seeking. The total stream will seek to the position that corresponds to timestamp of sid.
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinStreamLib_Seek(const SpinStreamLib_Handle hStream, SE_SeekMode eMode, int64_t ts, int sid);

/**
 * Query the stream configuration and runtime statistics.
 *
 * @param hStream Stream context handle.
 * @param pStatConf Pointer to SpinStream_StatConf that is to be filled.
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinStreamLib_GetStatConf(const SpinStreamLib_Handle hStream, SpinStream_StatConf* pStatConf);

/**
 * Returns the internal AVFormatContext. The AVFormatContext is directly mainly for setting up the output streams.
 * See the FFmpeg avformat API documentation and our examples for more information of setting up a stream for output.
 *
 * @param hStream Stream context handle.
 *
 * @return Pointer to AVFormatContext on success, NULL otherwise.
 */
SPINLIB_API AVFormatContext* SpinStreamLib_GetAVFormatContext(const SpinStreamLib_Handle hStream);

/**
 * Returns the reference time derived from the capture device.
 * Useful for inputs and mainly in live streaming scenarios.
 * When receiving a TS stream, this clock is derived from the PCRs
 * and is usable by the player application for proper frame presentation timing.
 *
 * @param hStream Stream context handle.
 *
 * @return Current stream reference time in seconds.
 */
SPINLIB_API double SpinStreamLib_GetRefTime(const SpinStreamLib_Handle hStream);

#ifdef __cplusplus
}
#endif

/**@}*/
