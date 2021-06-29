/**
 * \file    spinsei.h
 * \brief   Supplemental enhancement information definitions
 * \author  Chi Ching Chi <chi@spin-digital.com>
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2016-2018, Spin Digital Video Technologies GmbH
 * Berlin, Germany
 */

#ifndef __SPINSEI_H__
#define __SPINSEI_H__

/** \addtogroup common SpinCommon
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum SE_SEI
{
  SE_SEI_None                               =-1,
  SE_SEI_BufferingPeriod                    = 0,
  SE_SEI_PictureTiming                      = 1,
  SE_SEI_PanScanRect                        = 2,
  SE_SEI_FillerPayload                      = 3,
  SE_SEI_UserDataRegistered                 = 4,
  SE_SEI_UserDataUnregistered               = 5,
  SE_SEI_RecoveryPoint                      = 6,
  SE_SEI_SceneInfo                          = 9,
  SE_SEI_FullFrameSnapShot                  = 15,
  SE_SEI_ProgressiveRefinementSegmentStart  = 16,
  SE_SEI_ProgressiveRefinementSegmentEnd    = 17,
  SE_SEI_FilmGrainCharacteristics           = 19,
  SE_SEI_PostFilterHint                     = 22,
  SE_SEI_ToneMappingInfo                    = 23,
  SE_SEI_FramePacking                       = 45,
  SE_SEI_DisplayOrientation                 = 47,
  SE_SEI_GreenMetaData                      = 56,
  SE_SEI_SOPDescription                     = 128,
  SE_SEI_ActiveParameterSets                = 129,
  SE_SEI_DecodingUnitInfo                   = 130,
  SE_SEI_TemporalLevel0Index                = 131,
  SE_SEI_DecodedPictureHash                 = 132,
  SE_SEI_ScalableNesting                    = 133,
  SE_SEI_RegionRefreshInfo                  = 134,
  SE_SEI_NoDisplay                          = 135,
  SE_SEI_TimeCode                           = 136,
  SE_SEI_MasteringDisplayColourVolume       = 137,
  SE_SEI_SegmRectFramePacking               = 138,
  SE_SEI_TempMotionContrainedTileSets       = 139,
  SE_SEI_ChromaResamplingFilterHint         = 140,
  SE_SEI_KneeFunctionInfo                   = 141,
  SE_SEI_ColourRemappingInfo                = 142,
  SE_SEI_ContentLightLevel                  = 144,
  SE_SEI_AlternativeTransferCharacteristics = 182,
} SE_SEI;


typedef struct SpinSEI_UserDataUnregistered
{
  unsigned char uuid_iso_iec_11578[16];
  int userDataLength;
  unsigned char *userData;
} SpinSEI_UserDataUnregistered;


typedef enum SE_HashMethod
{
  SE_HASH_None = -1,
  SE_HASH_MD5,
  SE_HASH_CRC,
  SE_HASH_Checksum,
  SE_HASH_Reserved,
  SE_NUM_HashTypes
} SE_HashMethod;

static const int SD_DigestLength[SE_NUM_HashTypes] = {16, 2, 4};

typedef struct SpinSEI_DecodedPictureHash
{
  SE_HashMethod eHashtype;
  unsigned char acRecDigest[3][16];
  unsigned char acRefDigest[3][16];
} SpinSEI_DecodedPictureHash;

typedef struct SpinSEI_MasteringDisplayColourVolume
{
  int bEnabled;
  unsigned short ausPrimaries[3][2];   ///< Primary coordinates in order RGB.
  unsigned short ausWhitePoint[2];     ///< White point coordinate.
  unsigned int uiMaxLuminance;         ///< Max luminance value of the display in units of 0.0001 Cd/M^2
  unsigned int uiMinLuminance;         ///< Min luminance value of the display in units of 0.0001 Cd/M^2
} SpinSEI_MasteringDisplayColourVolume;

typedef struct SpinSEI_ContentLightLevel
{
  int bEnabled;
  unsigned short usMaxContentLight;       ///< Max content light level
  unsigned short usMaxPicAvgLight;        ///< Max picture average light level
} SpinSEI_ContentLightLevel;


typedef struct SpinSEI_Message
{
  SE_SEI eSEIType;
  union {
    SpinSEI_UserDataUnregistered unreg;
    SpinSEI_MasteringDisplayColourVolume disp;
    SpinSEI_ContentLightLevel cll;
  };
} SpinSEI_Message ;

#ifdef __cplusplus
}
#endif
#endif

/**@}*/
