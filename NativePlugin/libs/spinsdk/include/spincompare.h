/**
 * \file    spincompare.h
 * \brief   Spin Digital video quality comparison
 * \author  Chi Ching Chi <chi@spin-digital.com>
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2017-2020, Spin Digital Video Technologies GmbH
 * Berlin, Germany
 */

#pragma once

#include "spincommon.h"

 /** \defgroup compare SpinCompare
 *  \brief Spin Digital full reference quality metric library API.
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif


typedef enum SE_CompareMode
{
  SE_COMPAREMODE_Even = 1,
  SE_COMPAREMODE_Spherical = 2,
} SE_CompareMode;

typedef struct SpinCompare_Param
{
  Spin_Picture sCmpInFmt;   ///< input format
  //metrics
  int iPSNR; //set one to one or more SE_CompareMode to enable
  int iSSIM;
  int iMSSSIM;

  int iNumPicInFlight;
  int iSIMDExtension;
} SpinCompare_Param;


typedef void* SpinCompareLib_Handle;

typedef struct SpinCompare_Statistics
{
  double dPSNR;
  double dMSE;

  double dWSPSNR;
  double dWSMSE;

  double dSSIM;
  double dWSSSIM;

  double dMSSSIM;
  double dWSMSSSIM;
} SpinCompare_Statistics;

typedef struct SpinCompare_FrameStat
{
  SpinCompare_Statistics asPlaneStat[4];
  SpinCompare_Statistics asPictureStat;
} SpinCompare_FrameStat;

/**
 *  Compare API
 */

SPINLIB_API int SpinCompareLib_Open( SpinCompareLib_Handle* hCompare, const SpinCompare_Param* pParam );

SPINLIB_API int SpinCompareLib_CompareFrame( SpinCompareLib_Handle hFilter, Spin_Picture** ppRefPic, Spin_Picture** ppTestPic, SpinCompare_FrameStat* pStat, SpinCompare_FrameStat* pAvgStat);

SPINLIB_API int SpinCompareLib_Close( SpinCompareLib_Handle* hCompare);

SPINLIB_API const char* SpinCompareLib_GetFrameStatString(SpinCompareLib_Handle hCompare, SpinCompare_FrameStat* pStat, int mode);

#ifdef __cplusplus
}
#endif

/** @}*/

