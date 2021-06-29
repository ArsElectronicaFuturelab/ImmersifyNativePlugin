/**
 * \file    spinenc.h
 * \brief   Spin Digital HEVC/H.265 encoder header file
 * \author  chi@spin-digital.com
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2019-2020, Spin Digital Video Technologies GmbH
 * Berlin, Germany
 */

#pragma once

#include "spincommon.h"
#include "spincompare.h"

/** \defgroup encoder SpinEnc
 *  \brief Spin Digital encoder API
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum SE_EncPreset {
  SE_EP_None      = -1,
  SE_EP_Slow      = 7,
  SE_EP_Balanced  = 10,
  SE_EP_Fast      = 12,
  SE_EP_Faster    = 15
} SE_EncPreset;

typedef enum SE_RCMethod {
  SE_RC_None = -1,
  SE_RC_CQP,        ///< Constant QP pattern, bitrate is ignored
  SE_RC_VBR,        ///< Variable bitrate
  SE_RC_CBR,        ///< Constant bitrate
  SE_RC_VBR2,       ///< 2-pass variable bitrate
  SE_RC_CBR2,       ///< 2-pass constant bitrate
  SE_NUM_RCMethods
} SE_RCMethod ;

typedef struct SpinEnc_Descript
{
  /** Profile space, ranges from 0 to 3. See Recommendation ITU-T H.265 AnnexA for more information. */
  uint8_t profileSpace;

  uint8_t tierFlag;                ///< Tier flag (MAIN tier = 0, HIGH tier = 1).
  /** Profile IDC, ranges from 0 to 32. See Recommendation ITU-T H.265 AnnexA for more information. */
  uint8_t profileIDC;
  /** Level IDC, ranges from 0 to 254. See Recommendation ITU-T H.265 AnnexA for more information. */
  uint8_t levelIDC;

  /**
   * Picture description of the encode format, including margins and cropping.
   */
  Spin_Picture sPicDesc;

  int iNumRefs;               ///< Number of reference frames
  int iFrameDelay;            ///< Maximum frame delay of the encoder

  double dBitrate;            ///< Bitrate in kbps, maximum bitrate for VBR
  float fBufferSize;          ///< Buffer size in seconds

} SpinEnc_Descript;

/**
 * Data buffer for encoded data.
 */
typedef struct SpinEnc_DataBuffer
{
  uint8_t* pData;  //pointer to generic data pointer
  int iBytes;      //size of buffer
} SpinEnc_DataBuffer;

/**
 * Compressed picture filled by the encoder context.
 *
 * @see SpinEncLib_EncodeFrame
 */
typedef struct SpinEnc_CompressedPicture
{
  SpinEnc_DataBuffer sESBuf;

  SE_FrameType eFrameType;
  SE_RefType eRefType;
  int iTLayer;
  float fQP;

  int iPOC;
  int64_t llPts;
  int64_t llDts;
  int64_t llDuration;

  int iEncBufLvl;

  SpinCompare_FrameStat sStats;
  SE_HashMethod eHashMeth;
  uint8_t abRecDigest[4][16];

  double dStartTime;
  double dEndTime;

} SpinEnc_CompressedPicture;

/**
 * SpinEnc configuration struct for initializing the encoder context.
 *
 * @see SpinEncLib_Open
 */
typedef struct SpinEnc_Param
{
  Spin_Picture sEncInFmt;       ///< Encoder input format
  SE_EncPreset ePreset;       ///< Encoder complexity/quality preset

  struct RateControl {
    SE_RCMethod eRCMethod;    ///< Rate control method
    float fQP;                ///< QP for CQP encoding
    double dBitrate;          ///< Target bitrate in bits per second
    /**
     * The bitrate tolerance factor controls the maximum and minimum bitrate when using VBR modes.
     * The maximum bitrate is limited to fBitTolFactor*dBitrate.
     * The minimum bitrate is kept above dBitrate/fBitTolFactor.
     */
    float fBitTolFactor;
    /**
     * Hypothetical reference decoder buffer size for CBR modes.
     * Larger buffers reduce quality fluctuations, at the cost of increased transmission delay.
     * Maximum valid range is 0.3-3sec, but ranges depend on the intraperiod and gop size.
     */
    float fHrdBufSizeSec;
    /**
     * Enable partial update for improved rate control acccuracy.
     * Enabling this options results in non-deterministics bitstreams.
     */
    bool bPartialUpdate;
  } rc;

  struct HLAnalysis{
    /**
     * Number of pictures considered in lookahead. Minimum size is the gop size.
     * Automatically determined when set to 0.
     */
    int iLookAheadFrames;
    /**
     * Number of pictures between IRAP pictures. Must be a multiple of the GOP size. Automatically
     * Automatically determined when set to 0.
     */
    int iIntraPeriod;
    int iGOPSize;                ///< GOP size of hierarchical structure
    bool bClosedGop;             ///< Close GOPs at intra period boundaries. This is commonly required for chunk based streaming.
  } hlas;


  /**
   * Bitstream information/configuration parameters.
   * These options control the bitstream generation that takes place after the main encoding functions.
   */
  struct BitstreamConf {
    int iHeaderMode;      ///< -1: no headers 0: header only at beginning 1:headers before IDR 2:headers before IDR and CRA
    int iStaticSEIMode;   ///< 0: put static SEI before parameter sets (MP4) 1: put static SEI after parameter sets (TS)

    int iSar;             ///< Sample aspect ratio according to
    int iColorPrimaries;  ///< Color primaries according to
    int iTransferFunc;    ///< Transfer function according to
    int iColorMatrix;     ///< Color matrix according to
    int iChromaLoc;       ///< Chroma location according to
    bool bAUDelimiter;    ///< Generate an access unit delimiter NAL unit for every access unit
    bool bTimingInfo;     ///< Add timing info to VUI
    int iHashSEI;         ///< Include MD5 SEI message
    SpinSEI_MasteringDisplayColourVolume seiDisp;   ///< Add mastering display color volume SEI
    SpinSEI_ContentLightLevel seiCLL;               ///< Add content light level SEI
    SpinSEI_UserDataUnregistered seiUser;           ///< Add custom user SEI message

    bool bWpp;            ///< Generate bitstreams with WPP enabled
    int iTempLayers;      ///< Number of temporal layers
    bool bNoIntraPB;      ///< Disable intra mode in inter frames

    bool bAUCrc32;
  } bitconf ;

  /**
  *
  */
  void(*fLumaCoeffCB)(uint32_t tuPelX, uint32_t tuPelY, int tuSize, int sliceType, int predMode, uint8_t lumaIntraDir, int16_t* trCoeff);
  int iCBParMode;                         ///< Callback parallel mode 0:sequential 1:follow mtmode

  int iRateNum;                           ///< Rate numerator
  int iRateDen;                           ///< Rate denominator
  int iTimeBase;                          ///< PTS/DTS time base
  int iMTMode;                            ///< Multi-threading mode (off, wpp+imf, owf, owf+la)
  int iEncThreads;                        ///< Number of encoder threads
  int iLAThreads;                         ///< Number of lookahead threads
  int iFramesInFlightLA;                  ///< Frames-in-flight lookahead
  int iFramesInFlight;                    ///< Frames-in-flight main encoding
  int iFramesInFlight2;                   ///< Frames-in-flight finalization encoding (VBR2, CBR2)

  char acSimdExt[12];                     ///< SIMD extension (developer binaries only)

  SD_ExtLogFunc fLogFunc;     ///< Callback function for log messages.
  void* hLogHandle;           ///< Callback handle for fLogFunc callback function.

} SpinEnc_Param;


typedef void* SpinEncLib_Handle;


SPINLIB_API int SpinEncLib_GetDefaultParams(SpinEnc_Param* param);

/**
 * Opens an encoder context.
 * An encoder context is able to encode a sequence of raw images in planar format.
 * Multiple encoder contexts can be opened to encode sequences/chunks in parallel.
 *
 * @param phEncoder Pointer to a SpinEncLib_Handle, filled on success.
 * @param pParam Pointer to the SpinEnc_Param struct used to initialize the encoder.
 * @param pEncFmt Pointer to the encoder picture format to initialize the encoder.
 *
 * @see SpinEnc_Param
 * @see Spin_Picture
 *
 * @return SD_OK on success.
 */
SPINLIB_API int SpinEncLib_Open(SpinEncLib_Handle* phEncoder, const SpinEnc_Param* pParam);

/**
 * Closes an encoder context opened by SpinEncLib_Open.
 *
 * @param phEncoder The render context handle to close. *phRender is set to NULL after successfully closing the renderer.
 *
 * @see SpinEncLib_Open
 *
 * @return SD_OK on success.
 */
SPINLIB_API int SpinEncLib_Close(SpinEncLib_Handle* phEncoder );

/**
 * Returns the encoder description. The descript can be filled after opening the encoder context.
 *
 * @param hEncoder The encoder context handle.
 * @param pDescript Pointer to the to be filled SpinEnc_Descript.
 *
 * @return Returns SD_OK on success and SD_FAIL on failure.
 *
 * @see SpinEnc_Descript
 */
SPINLIB_API int SpinEncLib_GetDescription(SpinEncLib_Handle hEncoder, SpinEnc_Descript* pDescript);

/**
 * Returns the encoder parameters of the context. The parameters can be filled after opening the context.
 *
 * @param hEncoder The encoder context handle.
 * @param pParam Pointer to the to be filled SpinEnc_Param.
 *
 * @return Returns SD_OK on success and SD_FAIL on failure.
 *
 * @see SpinEnc_Param
 */
SPINLIB_API int SpinEncLib_GetParams(SpinEncLib_Handle hEncoder, SpinEnc_Param* pParam);


/**
 * Maps a picture buffer in pPic. The plane pointers in pPic can be accessed after mapping.
 * After filling the picture buffer the picture can be encoded with SpinEncLib_EncodeFrame,
 * or it can be simply returned without encoding with SpinEncLib_UnmapPicture.
 *
 * @param hEncoder The encoder context handle.
 * @param pPic The library maps a picture and makes it accessible via the the data fields of pPic.
 *
 * @see SpinEncLib_UnmapPicture
 * @see SpinEncLib_EncodeFrame
 *
 * @return SD_OK on success.
 */
SPINLIB_API int SpinEncLib_MapPicture(SpinEncLib_Handle hEncoder, Spin_Picture* pPic);

/**
 * Unmaps a picture buffer mapped with SpinEncLib_MapPicture.
 * After unmapping the plane pointers in pPic cannot be accessed anymore.
 *
 * @param hEncoder The encoder context handle.
 * @param pPic Pointer to a picture previously mapped by SpinEncLib_MapPicture.
 *
 * @see SpinEncLib_MapPicture
 *
 * @return SD_OK on success.
 */
SPINLIB_API int SpinEncLib_UnmapPicture(SpinEncLib_Handle hEncoder, Spin_Picture* pPic);

/**
 * Unmaps a picture buffer and queues it for encoding.
 * A compressed picture is filled by the libary in pCompPic when a picture is ready for output.
 * The bPicReady is set to 1 when a picture is available.
 * The SpinEnc_DataBuffer contained in SpinEnc_CompressedPicture is (re)allocated by the library.
 * The same SpinEnc_CompressedPicture struct can be reused for the next call.
 *
 * @param hEncoder The encoder context handle.
 * @param pPic Pointer to a picture previously mapped by SpinEncLib_MapPicture.
 * @param pCompPic Pointer to compressed picture filled by the library.
 *
 * @see SpinEncLib_MapPicture
 *
 * @return SD_OK on success.
 */
SPINLIB_API int SpinEncLib_EncodeFrame(SpinEncLib_Handle hEncoder, Spin_Picture* pPic, SpinEnc_CompressedPicture* pCompPic, bool* bPicReady);

/**
 * Get a compressed from the encoder context.
 * The pCompPic is filled with the next ready picture in case one is available.
 * If bFlush is set and at least one picture is remaining, the call will flush a picture out of the encoder.
 *
 * @param hEncoder The encoder context handle.
 * @param pCompPic Pointer to compressed picture filled by the library.
 * @param bFlush Flush option to force a picture out.
 *
 * @return Returns 1 if a picture is filled, 0 otherwise.
 */
SPINLIB_API int SpinEncLib_GetEncPicture(SpinEncLib_Handle hEncoder, SpinEnc_CompressedPicture* pCompPic, bool bFlush);

/**
 * Free SpinEnc_CompressedPicture data allocated by calls to SpinEncLib_EncodeFrame when outputting compressed frames.
 *
 * @param hEncoder The encoder context handle.
 * @param pCompPic Compressed picture to be freed and cleared;
 *
 * @return SD_OK on success.
 */
SPINLIB_API int SpinEncLib_FreeCompressedPic(SpinEnc_CompressedPicture* pCompPic);

/**
 * Return the encoded header data. This included the parameter sets and static SEI messages.
 *
 * @param hEncoder The encoder context handle.
 * @param pHeaderBuf Pointer to the header data buffer. The pHeaderBuf->pData will be reallocated in case the SpinEnc_DataBuffer is reused.
 *
 * @return SD_OK on success.
 */

SPINLIB_API int SpinEncLib_GetHeaderData(SpinEncLib_Handle hEncoder, SpinEnc_DataBuffer* pHeaderBuf);

/**
 * Encode a SEI message.
 *
 * @param hEncoder The encoder context handle.
 * @param pSEIMsg Pointer of the to be encoded SpinSEI_Message struct.
 * @param pSEIBuf Pointer to the encoded SEI message buffer. The pSeiBuf->pData will be reallocated in case the SpinEnc_DataBuffer is reused.
 *
 * @return SD_OK on success.
 */
SPINLIB_API int SpinEncLib_CreateSEINal( SpinEncLib_Handle hEncoder, SpinSEI_Message* pSEIMsg, SpinEnc_DataBuffer* pSEIBuf);

/**
 * Free SpinEnc_DataBuffer previously allocated by SpinEncLib_GetHeaderData or SpinEncLib_CreateSEINal.
 *
 * @param hEncoder The encoder context handle.
 * @param pCompPic Compressed picture to be freed and cleared;
 *
 * @return SD_OK on success.
 */

SPINLIB_API int SpinEncLib_FreeDataBuffer(SpinEnc_DataBuffer* pDataBuf);

/**
 * Get encoder configuration in C-string form for printing
 *
 * @param hEncoder The encoder context handle.
 *
 * @return Configuration string. Can be used until the next SpinEncLib_GetConfigString call of the context is closed.
 */
SPINLIB_API const char* SpinEncLib_GetConfigString(SpinEncLib_Handle hEncoder);


/**
* Return const char* pointing to a summary of the statistics string. Pointer is
* valid until the next call to SpinEncLib_GetFrameStatString or SpinEncLib_Close call.
*
* @param iVerbosity Verbosity of statistics, 1:basic frame info, 2:frame info with metrics
*/
SPINLIB_API const char* SpinEncLib_GetFrameStatString( SpinEncLib_Handle hEncoder, SpinEnc_CompressedPicture* pCompPic, int iVerbosity);

/**
 * Return const char* pointing to a summary of the statistics string. Pointer is
 * valid until the next call to SpinEncLib_GetSummaryString or SpinEncLib_Close call.
 *
 * @param iVerbosity Verbosity of statistics, 0:overall summary only, 1:per frame type statistics
 */
SPINLIB_API const char* SpinEncLib_GetSummaryString( SpinEncLib_Handle hEncoder, int iVerbosity);

#ifdef __cplusplus
}
#endif

/** @}*/
