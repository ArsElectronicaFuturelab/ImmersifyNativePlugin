/**
 * \file    spindec.h
 * \brief   Spin Digital HEVC Decoder header file
 * \author  Chi Ching Chi <chi@spin-digital.com>
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2015-2020, Spin Digital Video Technologies GmbH
 * Berlin, Germany
 */

#pragma once

/** \defgroup decoder SpinDec
 *  \brief Spin Digital decoder library API.
 *  @{
 */

#include "spincommon.h"
#include "spinhevc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This struct specifies the sequence specific parameters. It can be retrieved from the
 * decoder using SpinDecLib_GetDescription after an SPS NAL unit is successfully decoded.
 *
 * @see SpinDecLib_GetDescription
 */
typedef struct SpinDec_Descriptor
{
  /** Profile space, ranges from 0 to 3. See Recommendation ITU-T H.265 AnnexA for more information. */
  uint8_t profileSpace;

  uint8_t tierFlag;                ///< Tier flag (MAIN tier = 0, HIGH tier = 1).
  /** Profile IDC, ranges from 0 to 32. See Recommendation ITU-T H.265 AnnexA for more information. */
  uint8_t profileIDC;
  /** Level IDC, ranges from 0 to 254. See Recommendation ITU-T H.265 AnnexA for more information. */
  uint8_t levelIDC;

  uint32_t uiRateNum;     ///< Framerate numerator. Set to 0 when not available.
  uint32_t uiRateDen;     ///< Framerate denominator. Set to max uint when not available.

  /**
  * Picture description of the video format. This is identical to sPicDesc except in the SE_PBM_ExtInt
  * picture mode. In this case the picture format produced by the decoder might be different than the
  * original decoding format.
  */
  Spin_Picture sVideoDesc;

  /**
   * Picture description of the decoded picture. Only pictures conforming to this description can
   * be used to interact with the SpinDecLib_* library calls that return or copy pictures out of the
   * decoder context. All fields except the iStride of the plane are filled. The user is responsible for
   * assigning the stride conforming to the margin and alignment requirements of the picture.
   * When using SpinLib_AllocFrame or SpinRenderLib_MapVideoFrame the stride does not need to filled as
   * these API calls fill it automatically.
   *
   * The ePixFormat might be initialized to a PixelFormat of a different category than the decoder instance
   * was initialized with. This happens in cases where the ChromaFormat is not compatible with the pixel
   * format category. A compatible fallback format is used instead.
   */
  Spin_Picture sPicDesc;

  /**
   * Additional delay in frames due to processing (equal to internal iNumConcurrentPics)
   */
  int uiExtraFrameDelay;

  SpinHEVC_VPS sVPS;
  SpinHEVC_SPS sSPS;
  SpinHEVC_PPS sPPS;

  SpinSEI_MasteringDisplayColourVolume seiDisp;
  SpinSEI_ContentLightLevel seiCLL;

} SpinDec_Descript;


/**
 * This struct holds the decoded picture information.
 *
 * @see SpinDecLib_DecodeNALU
 * @see SpinDecLib_DecodeAU
 * @see SpinDecLib_GetDecPicture
 *
 */
typedef struct SpinDec_Picture
{
  Spin_Picture sPic;            ///< Contains the pixel data.

  uint8_t uiNalType;            ///< Nal unit type of the picture slices.
  SE_FrameType ePicType;        ///< Frame type.
  int iPOC;                     ///< Picture order count.
  bool bNoOutput;               ///< No output flag. When set, the picture should not to be considered for further processing.
  int iTLayer;                  ///< Temporal layer.
  int iQp;                      ///< Quantization parameter of first slice.
  uint32_t uiCodedPicSize;      ///< Total size in bytes of the compressed NAL units associated to this picture.
  double dDecodingTime;         ///< Duration of the decoding process.
  double dFrameLatency;         ///< Latency from start of decoding until the final frame output.

  SpinSEI_DecodedPictureHash sPicHash; ///< Reference and reconstructed hashes of each picture plane.

  /**
   *  Error code of the picture. Set error codes between -300 and -310 for more information. When multiple errors are
   *  detected the lowest error code reported. FIXME At the moment in case of error always SD_E_RBSP_GENERIC is returned.
   */
  enum SE_ReturnCodes eErrCode;

} SpinDec_Picture;

/**
 * This struct holds the decoded sei payload
 */
typedef struct SpinDec_SEIMessage
{
  SE_SEI eSEIType;               ///< SEI message type.
  union SU_SEIData {
    SpinSEI_UserDataUnregistered user;  ///< User SEI message payload.
  } data ;
} SpinDec_SEIMessage;


/**
 *  Multi-threading modes
 */
typedef enum SE_DecMTMode
{
  SE_DECMT_Off = 0,     ///< Sequential execution.
  SE_DECMT_Auto,        ///< Automatically selects the best parallelization mode (wpp/frame/decoupled).
  SE_DECMT_FrameLevel,  ///< Force frame parallelism only (1 thread/frame).
  SE_DECMT_Decoupled,   ///< Force decoupled mode (separated symbol parsing and pixel reconstruction).
  SE_NUM_DecMTModes
} SE_DecMTMode;


/**
* Picture buffer modes. The picture buffer mode influences how SpinDecLib_DecodeNALU
* and SpinDecLib_DecodeAU behave regarding the input and output buffer.
*/
typedef enum SE_PicBufMode
{
  SE_PBM_None = -1,
  /**
  * Internal picture mode. The decoding process uses its own picture buffers.
  * A picture is copied to the application buffer when outputting the frames.
  */
  SE_PBM_Internal,
  /**
  * External-internal picture mode. In this mode the decoding process uses its own picture buffers.
  * During the decoding process, the external buffers passed to the decoder are also written at
  * the same time as the internal picture buffers.
  * Because the external picture buffers are not used as reference frames, they can have a different
  * format than the decoding format. The variables iLog2DownScale, bForce8bit, and all BC4 formats
  * instructs the decoder do produce scaled down or compressed picture, and are therefore only
  * possible in this mode.
  */
  SE_PBM_ExtInt,
  SE_NUM_PicBufModes
} SE_PicBufMode;

/**
* Frame discard modes.
*/
typedef enum SE_FrameDiscardMode
{
  SE_FDM_None = -1,   ///< Decode all frames (default)
  SE_FDM_NonRef,      ///< Discard non-ref non-intra frames
  SE_FDM_Temporal,    ///< Discard frames with temporal level higher than 0
  SE_FDM_NonIntra,    ///< Additionally discard all non-intra frames
  SE_FDM_NonKey,      ///< Only decode key frames (IDR, CRA)
  SE_NUM_FrameDiscardModes,
}SE_FrameDiscardMode;

/**
 * SpinDec_Param decoder configuration struct
 *
 * @see SpinDecLib_GetDefaultParams
 * @see SpinDecLib_Open
 */
typedef struct SpinDec_Param
{
  int iMaxTemporalLayer;           ///< Maximum temporal layer to be decoded.
  int iLayerId;                    ///< Layer ID to decode (default is 0).
  int bCalcHash;                   ///< Enable generation of picture plane hashes in case picture hash.
                                   ///< SEIs are available in the stream.
  int bTraceHLS;                   ///< Enable tracing of high level syntax parameters.
  int iOutputOrder;                ///< Set the picture output order (0:presentation order 1:decoding order)

  int bDropMissingRefs;            ///< Drop incoming NALU/AU with missing reference frames
  SE_FrameDiscardMode eDiscardMode; ///< Discard certain frames types to accelerate decoding
  SE_DecMTMode eMtMode;            ///< Multi-threading mode

  int iNumThreadPools;              ///< Number of decoding thread pools. Maximum 4.

  struct {
    /**
    * Number of concurrent pictures for decoding. Higher number of pictures increase parallelism,
    * but also picture delay and memory requirements. The decoder context determines the number
    * of concurrent pictures automatically when set to 0.
    */
    int iNumConcurrentPics;
    int iNumThreads;                ///< Number of decoding threads. Determined automatically when set to 0.
    bool bUseAffinity;
    uint64_t aiCPUAffinityMask[16];    ///< CPU affinity mask.
  } asThreadPoolConfig[4];

  SE_PixFmtCat ePixFmtMeth;         ///< Use the specified SE_PixFmtCat for the output format. Set to -1 for automatic selection.
  SE_PicBufMode ePicBufferMode;     ///< Picture buffer mode.
  /**
  * Output downscaled video frames. Only a downscaling factor of 2 and 4 are supported (valid range 0-2).
  */
  int iLog2DownScale;
  bool bForce8bit;           ///< Force the output picture to 8-bit.

  bool bDisableSao;          ///< Disable SAO filter. Disabling SAO will result in non-conformant decoded frames.
  bool bDisableDeblock;      ///< Disable deblocking filter. Disabling deblocking will result in non-conformant decoded frames.

  SD_ExtLogFunc fLogFunc;     ///< Callback function for log messages.
  void* hLogHandle;           ///< Callback handle for fLogFunc callback function.

} SpinDec_Param;


typedef void* SpinDecLib_Handle;

/**
 * Fills SpinDec_Param* param with the default values.
 * Typical usage is to fill the default parameters and override specific variables only.
 * @param param Pointer to the to be filled SpinDec_Param struct.
 * @see SpinDec_Param
 */
SPINLIB_API void SpinDecLib_GetDefaultParams(SpinDec_Param* psParam);

/**
 * Opens a decoder context.
 * A decoder context is able to decode a video from a single stream.
 * A stream is a sequence of NAL units with the same SPS or SPSs with the same configuration.
 * Multiple decoder contexts can be opened to decode different video streams in parallel.
 * @param phDecoder Pointer to a SpinDecLib_Handle. SpinDecLib_Open return in this field the handle to the created SpinDecLib instance.
 * @param psaram Pointer to the SpinDec_Param struct used to initialize the decoder.
 * @see SpinDec_Param
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinDecLib_Open(SpinDecLib_Handle* phDecoder,
  const SpinDec_Param* psParam);

/**
 * Closes a decoder context opened by SpinDecLib_Open.
 * @param hDecoder The to be closed SpinDecLib_Handle.
 * @see SpinDecLib_Open
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinDecLib_Close( SpinDecLib_Handle* phDecoder );

/**
 * Return if decoding pBuffer would start a new access unit.
 * @param hDecoder The decoder context handle.
 * @param pBuffer Pointer to the NAL unit payload data.
 * @param uiBuffer Byte size of the NAL unit payload data.
 * @return 1 if new access unit is detected. 0 if no new access unit is detected.
 * See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinDecLib_IsNewAccesUnit(SpinDecLib_Handle hDecoder,
  const uint8_t*     pBuffer,
  unsigned int       uiBuffer);


/**
 * Decodes a NAL unit and returns available decoded picture data. This API function is one of the
 * main decoding interface. This function decodes single NAL units and returns when available
 * a fully decoded picture in ppPicOut. A picture is earliest returned when the first slice
 * of the next access unit is decoded to mark the end of the previous access unit.
 *
 * Two ways of using this interface exists, controlled by the ePicBufferMode variable
 * of SpinDec_Params. When ePicBufferMode = 1, internal pictures are used to store the decoded
 * uncompressed pictures. When these pictures are available for output, a copy is performed to
 * **ppPicOut. The parameters psPicIn and pbUsedPicIn are ignored when internal pictures are
 * used.
 *
 * To avoid these resource consuming full picture copy operations, external pictures can be used
 * (ePicBufferMode = 0). In this mode, picture buffers external to the library are used to hold the
 * decoded uncompressed pictures. In the external picture mode psPicIn is used to pass the
 * picture buffer allocated on the application side. The library writes a 1 to *pbUsedPicIn
 * to indicate the SpinDec_Picture* psPicIn has been taken in by the library, and a new pcPicIn
 * should be used in the next call to SpinDecLib_DecodeNALU. When the picture is ready an earlier
 * SpinDec_Picture* psPicIn pointer is returned in *ppPicOut.
 *
 * This API does not return the decoded picture associated with the input NAL unit directly.
 * The actual picture delay is influenced by the configuration of the input bitstream and the
 * decoder configuration. With internal pictures this is the number of reorder pictures of the
 * bitstream + the pictures in flights used for parallel decoding. With external pictures the
 * delay is longer because the picture has to be fully unreferenced before it can be returned
 * to the application.
 *
 * In the event of change of format and/or resolution of the video, both psPicIn and ppPicOut
 * are reformed automatically to the new format if possible. Frames can only be reformed if the
 * iAllocSize of the picture is greater or equal to that required for the new format. Internally
 * the SpinLib_ReformFrame is used to reform the frame in case the picture is not compatible.
 *
 * @param hDecoder The decoder context handle.
 * @param pBuffer Pointer to the NAL unit payload data.
 * @param uiBuffer Byte size of the NAL unit payload data.
 * @param llPts Presentation time stamp of the buffer. This pts value is copied in pts field
 * of the Spin_Picture inside SpinDec_Picture.
 * @param psPicIn Pointer to the SpinDec_Picture holding the externally allocated picture buffer.
 * Only used when ePicBufferMode = 0.
 * @param pbPicInFound The library writes a 1 to *pbPicInFound in case the NAL unit contained the first slice
 * of a picture. In case of external pictures this also indicates that the psPicIn is consumed.
 * @param ppPicOut The library either performs a deep copy of an internal picture to **ppPicOut or
 * returns an external picture pointer to *ppPicOut.
 * @param pbHasPicOut The library writes a 1 to pbHasPicOut when a picture is returned in ppPicOut.
 * @param ppNalTraceHLS When bTraceHLS is enabled in SpinDec_Params and ppNalTraceHLS is not equal to NULL,
 * the library returns a pointer to a printable trace buffer containing the HLS information of the NAL unit.
 * The pointer is usable until the next library call.
 *
 * @return On success the SE_NalUnitType of the NAL unit is returned. See spinretcodes.h for other return codes.
 *
 * @see SE_NalUnitType
 * @see SpinDec_Param
 * @see SpinDecLib_Open
 * @see SpinLib_AllocFrame
 * @see SpinLib_ReformFrame
 */
SPINLIB_API int SpinDecLib_DecodeNALU( SpinDecLib_Handle hDecoder,
  const uint8_t* pBuffer,
  uint32_t       uiBuffer,
  int64_t             llPts,
  SpinDec_Picture*    psPicIn,
  bool*               pbPicInFound,
  SpinDec_Picture**   ppPicOut,
  bool*               pbHasPicOut,
  const char**        ppNalTraceHLS );

/**
 * Decodes an access unit and returns available decoded picture data. This API call is similar
 * to the SpinDecLib_DecodeNALU, but instead multiple NAL units can be decoded in one call as
 * long as the NAL units belong to the same access unit. Internally SpinDecLib_DecodeNALU is
 * called one or more times to decode the NAL units in pBuffer.
 *
 * Different to SpinDecLib_DecodeNALU is that the input buffer must contain NAL units including
 * their NAL unit delimiters. Two NAL unit delimiter types are supported: the AnnexB type using
 * a sequence of 0 0 1, and MP4 type which uses a 4-byte sequence that denotes the size of the
 * NAL unit.
 *
 * NAL units are decoded opportunistically, that means as many NAL units as possible are decoded.
 * The return codes prioritize later decoded slice NAL units. In case errors occur the error code
 * might be masked by a following successfully decoded NAL unit of the same access unit.
 *
 * @param hDecoder The decoder context handle.
 * @param pBuffer Pointer to the NAL unit payload data.
 * @param uiBufferSize Size of the NAL unit payload data in bytes.
 * @param llPts Presentation time stamp of the buffer. This pts value is copied in pts field
 * of the Spin_Picture inside SpinDec_Picture.
 * @param bMP4markers Set to 1 for access units that have MP4 style NAL unit delimiters. Set to
 * 0 for AnnexB NAL unit delimiters.
 * @param puiConsumedBytes Pointer to the number of bytes consumed. The call returns the bytes consumed
 * of successfully decoded NAL units.
 * @param pbPicInFound The library writes a 1 to *pbPicInFound in case the NAL unit contained the first slice
 * of a picture. In case of external pictures this also indicates that the psPicIn is consumed.
 * @param ppPicOut The library either performs a deep copy of an internal picture to **ppPicOut or
 * returns an external picture pointer to *ppPicOut.
 * @param pbHasPicOut The library writes a 1 to pbHasPicOut when a picture is returned in ppPicOut.
 * @param ppAUTraceHLS When bTraceHLS is enabled in SpinDec_Params and ppAUTraceHLS is not equal to NULL,
 * the library returns a pointer to a printable trace buffer containing the HLS information of the access
 * unit. The pointer is usable until the next library call.
 *
 * @return On success the SE_NalUnitType of the NAL unit is returned. See spinretcodes.h for other return codes.
 *
 * @see SE_NalUnitType
 * @see SpinDec_Param
 * @see SpinDecLib_Open
 * @see SpinDecLib_DecodeNALU
 * @see SpinLib_AllocFrame
 */
SPINLIB_API int SpinDecLib_DecodeAU( SpinDecLib_Handle hDecoder,
  const uint8_t*  pBuffer,
  uint32_t        uiBufferSize,
  int64_t             llPts,
  bool                bMP4markers,
  uint32_t*           puiConsumedBytes,
  SpinDec_Picture*    psPicIn,
  bool*               pbPicInFound,
  SpinDec_Picture**   ppPicOut,
  bool*               pbHasPicOut,
  const char**        ppAUTraceHLS);

/**
 * Return last decoded SEI message if the SEI message type is supported via this interface.
 *
 * @param hDecoder The decoder context handle.
 * @param pSEImsg Pointer to the SpinDec_SEIMessage. Some message types require dynamically allocated fields.
 * In this case the decoder context will allocated them, but the caller should call SpinDecLib_FreeSEIMessage
 * to free these fields.
 *
 * @return Returns SD_OK on success and SD_FAIL on failure.
 *
 * @see SpinDec_SEIMessage for SEI message types that can be returned via this interface.
 * @see SpinDecLib_FreeSEIMessage
 */
SPINLIB_API int SpinDecLib_GetSEIMessage( SpinDecLib_Handle hDecoder,
  SpinDec_SEIMessage* pSEImsg);

/**
 * Frees dynamically allocated fields of SpinDecLib_GetSEIMessage. SpinDecLib_FreeSEIMessage should be called
 * after each successful SpinDecLib_GetSEIMessage call.
 *
 * @param pSEImsg Pointer to the SpinDec_SEIMessage.
 *
 * @return Returns SD_OK on success and SD_FAIL on failure.
 *
 * @see SpinDec_SEIMessage
 * @see SpinDecLib_GetSEIMessage
 */
SPINLIB_API void SpinDecLib_FreeSEIMessage( SpinDec_SEIMessage* pSEImsg);

/**
 * Returns the decoder description in pDescript when available. An SPS must be decoded before
 * this function is able to return a SpinDec_Descript successfully. The descript struct holds
 * important information that applies to the entire bitstream, such as resolution and chroma
 * format.
 *
 * @param hDecoder The decoder context handle.
 * @param pDescript Pointer to the to be filled SpinDec_Descript.
 *
 * @return Returns SD_OK on success and SD_FAIL on failure.
 *
 * @see SpinDec_Descript
 */
SPINLIB_API int SpinDecLib_GetDescription( SpinDecLib_Handle hDecoder,
  SpinDec_Descript* pDescript);

/**
 * Returns a decoded picture in ppPicOut when available. The typical use case of this function is
 * to flush the final pictures out of the decoder after an end of stream is detected or signaled.
 * Because of the reordering and multiple pictures in flight the decoder context returns the decoded
 * pictures with a delay. To make sure all in-flight pictures are made available for flushing, call
 * SpinDecLib_FlushInFlightPictures before calls to this function with the bFlush set to 1.
 *
 * Only recommended to use with bFlush set to 1 as otherwise SpinDecLib_DecodeNALU and
 * SpinDecLib_DecodeAU already returns any available picture.
 *
 * @param hDecoder The decoder context handle.
 * @param ppPicOut The library either performs a deep copy of an internal picture to **ppPicOut or
 * returns an external picture pointer to *ppPicOut.
 * @param bFlush Set to 1 in case no new NAL units should be expected by the decoder context.
 *
 * @return Returns 1 when a picture is returned and 0 otherwise.
 *
 * @see SpinDec_Descript
 * @see SpinDecLib_FlushInFlightPictures
 */
SPINLIB_API int SpinDecLib_GetDecPicture( SpinDecLib_Handle hDecoder,
  SpinDec_Picture** ppPicOut,
  int bFlush );

/**
 * A blocking call that flushes the in-flight pictures. This call returns after all in-flight pictures have
 * been decoded. The internal ready picture queue remains intact.
 *
 * @param hDecoder The decoder context handle.
 *
 * @return Returns SD_OK on success and SD_FAIL on failure.
 */
SPINLIB_API int SpinDecLib_FlushInFlightPictures( SpinDecLib_Handle hDecoder);


/**
 * A blocking call that flushes and invalidates the in-flight pictures and all reference pictures. After this
 * call the decoder context is "empty", which means no decoded pictures remain in the decoder. In case of
 * external pictures the application is responsible for making the pictures that were injected into the decoder
 * using SpinDecLib_DecodeNALU or SpinDecLib_DecodeAU available again. This function can be used to implement
 * abrupt seeking.
 *
 * @param hDecoder The decoder context handle.
 *
 * @return Returns SD_OK on success and SD_FAIL on failure.
 */
SPINLIB_API int SpinDecLib_InvalidateInFlightPictures( SpinDecLib_Handle hDecoder);


#ifdef __cplusplus
}
#endif

/** @}*/

