/**
 * \file    spincommonstr.h
 * \brief   String names of spincommon enums
 * \author  Chi Ching Chi <chi@spin-digital.com>
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2015-2018, Spin Digital Video Technologies GmbH
 * Berlin, Germany
 */

#ifndef __SPINCOMMONSTR_H__
#define __SPINCOMMONSTR_H__

/** \addtogroup common SpinCommon
 *  @{
 */
#include "spincommon.h"

#ifdef __cplusplus
extern "C" {
#endif

__SD_UNUSED__
static const char* SD_gacChromaFmtStr[SE_NUM_ChromaFormats]
{
  "400",
  "420",
  "422",
  "444"
};

__SD_UNUSED__
static const char* SD_gacColorModelStr[SE_NUM_ColorModels]
{
  "yuv",
  "rgb",
  "xyz",
};

__SD_UNUSED__
static const char* SD_gacColorSpaceStr[SE_NUM_ColorSpaces][SE_NUM_ColorRanges]
{
  {"bt601f","bt601l"},
  { "bt709f","bt709l" },
  { "bt2020f","bt2020l" },
  { "dcip3","dcip3yuv" },
};

__SD_UNUSED__
static const char* SD_gacTransferStr[SE_NUM_TransferFuncs]
{
  "sdr",
  "linear",
  "st2084",
  "stdb67",
  "gamma26",
  "srgb"
};

__SD_UNUSED__
static const char* SD_gacGeometryStr[SE_NUM_GeometryTypes]
{
  "plane",
  "cylinder",
  "erp",
  "cmp"
};

__SD_UNUSED__
static const char* SD_gacPixFmtStr[SE_NUM_PixFormats]
{
  "400p",
  "420p",
  "422p",
  "444p",
  "4444p",
  "420sp",
  "422sp",
  "400bp",
  "420bp",
  "422bp",
  "444bp",  
  "4444bp",  
  "420bsp",
  "422bsp",
  "p422",
  "p422_v210",
  "p422_v210a",
  "p444",
  "p4444",
  "bp444",
  "p444_16",
  "p444_32",
  "p444_ra32",
  "p444_rb32",
  "400bc4",
  "420bc4",
  "422bc4",
  "444bc4",
  "4444bc4",

};

#ifdef __cplusplus
}
#endif
#endif

/**@}*/
