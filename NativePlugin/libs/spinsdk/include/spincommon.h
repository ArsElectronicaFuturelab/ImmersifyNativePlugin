/**
 * \file    spincommon.h
 * \brief   Spin Digital common structs and enums header file
 * \author  Chi Ching Chi <chi@spin-digital.com>
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2015-2020, Spin Digital Video Technologies GmbH
 * Berlin, Germany
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>


#ifdef __GNUC__
#define __SD_UNUSED__ __attribute__((__unused__))
#define DEPRECATED_API __attribute__((__unused__)) __attribute__((__deprecated__))
#define SPINLIB_API __attribute__((__unused__))
#elif _MSC_VER
#define __SD_UNUSED__
#define DEPRECATED_API __declspec(deprecated)
#define SPINLIB_API __declspec(dllexport)
#endif


#include "spinretcodes.h"
#include "spinsei.h"
#include "spinlicense.h"

/** \defgroup common SpinCommon
 *  \brief Common API included in all Spin Digital libraries.
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * External log callback prototype.
 * The messages verbosity (iVerbosity) is defined as:
 * 0 - ERROR
 *   - Something went really wrong and we will crash now.
 *   - Something went wrong and recovery is not possible.
 *   - Something went wrong and cannot losslessly be recovered.  Still, not all
       future data is affected.
 * 1 - WARNING
 *   - Something somehow does not look correct. This may or may not lead to new
       problems.
 * 2 - INFORMATIONAL
 *   - Standard information.
 * 3 - VERBOSE
 *   - Detailed information.
 * 4 - DEBUG
 *   - Detailed information for debugging purporses.
 * 5 - DEVELOPER
 *   - Stuff which is only useful for developers.
 *   - Extremely verbose debugging, useful for development.
 */
typedef void (*SD_ExtLogFunc)(void* pHandle, int iVerbosity, const char* pcMessage);


/**
 * Coded video frame types.
 */
typedef enum SE_FrameType
{
  SE_FT_None = -1,
  SE_FT_BFrame,       ///< Bi-prediction frame.
  SE_FT_PFrame,       ///< Uni-prediction frame.
  SE_FT_IFrame,       ///< Intra-only frame. No dependencies to previous frames.
  SE_NUM_FrameTypes
} SE_FrameType ;

/**
 * Reference frame type
 */
typedef enum SE_RefType
{
  SE_Ref_None = -1,
  SE_REF_Key,      ///< IRAP
  SE_REF_Ref,      ///< Reference picture
  SE_REF_NonRef,   ///< Non-ref picture
  SE_NUM_RefFrameTypes
} SE_RefType;

/**
 * Chroma formats. This enum is castable to SE_PixFormat which results in the planar type
 * of chroma format.
 */
typedef enum SE_ChromaFormat
{
  SE_CF_None = -1,
  SE_CF_400,          ///< 4:0:0 monochrome
  SE_CF_420,          ///< 4:2:0 chroma is 2x subsampled in both width and height
  SE_CF_422,          ///< 4:2:2 chroma is 2x subsampled in width only
  SE_CF_444,          ///< 4:4:4 no chroma subsampling
  SE_NUM_ChromaFormats
} SE_ChromaFormat ;

/**
 * HEVC NAL unit types (NUT) as they appear in the ITU-T H.265 recommendation.
 */
typedef enum SE_NalUnitType
{
  SE_NUT_None = -1,
  SE_NUT_CODED_SLICE_TRAIL_N,     // 0
  SE_NUT_CODED_SLICE_TRAIL_R,     // 1

  SE_NUT_CODED_SLICE_TSA_N,       // 2
  SE_NUT_CODED_SLICE_TSA_R,       // 3

  SE_NUT_CODED_SLICE_STSA_N,      // 4
  SE_NUT_CODED_SLICE_STSA_R,      // 5

  SE_NUT_CODED_SLICE_RADL_N,      // 6
  SE_NUT_CODED_SLICE_RADL_R,      // 7

  SE_NUT_CODED_SLICE_RASL_N,      // 8
  SE_NUT_CODED_SLICE_RASL_R,      // 9

  SE_NUT_RESERVED_VCL_N10,
  SE_NUT_RESERVED_VCL_R11,
  SE_NUT_RESERVED_VCL_N12,
  SE_NUT_RESERVED_VCL_R13,
  SE_NUT_RESERVED_VCL_N14,
  SE_NUT_RESERVED_VCL_R15,

  SE_NUT_CODED_SLICE_BLA_W_LP,    // 16
  SE_NUT_CODED_SLICE_BLA_W_RADL,  // 17
  SE_NUT_CODED_SLICE_BLA_N_LP,    // 18
  SE_NUT_CODED_SLICE_IDR_W_RADL,  // 19
  SE_NUT_CODED_SLICE_IDR_N_LP,    // 20
  SE_NUT_CODED_SLICE_CRA,         // 21
  SE_NUT_RESERVED_IRAP_VCL22,
  SE_NUT_RESERVED_IRAP_VCL23,

  SE_NUT_RESERVED_VCL24,
  SE_NUT_RESERVED_VCL25,
  SE_NUT_RESERVED_VCL26,
  SE_NUT_RESERVED_VCL27,
  SE_NUT_RESERVED_VCL28,
  SE_NUT_RESERVED_VCL29,
  SE_NUT_RESERVED_VCL30,
  SE_NUT_RESERVED_VCL31,

  SE_NUT_VPS,                     // 32
  SE_NUT_SPS,                     // 33
  SE_NUT_PPS,                     // 34
  SE_NUT_ACCESS_UNIT_DELIMITER,   // 35
  SE_NUT_EOS,                     // 36
  SE_NUT_EOB,                     // 37
  SE_NUT_FILLER_DATA,             // 38
  SE_NUT_PREFIX_SEI,              // 39
  SE_NUT_SUFFIX_SEI,              // 40

  SE_NUT_RESERVED_NVCL41,
  SE_NUT_RESERVED_NVCL42,
  SE_NUT_RESERVED_NVCL43,
  SE_NUT_RESERVED_NVCL44,
  SE_NUT_RESERVED_NVCL45,
  SE_NUT_RESERVED_NVCL46,
  SE_NUT_RESERVED_NVCL47,
  SE_NUT_UNSPECIFIED_48,
  SE_NUT_UNSPECIFIED_49,
  SE_NUT_UNSPECIFIED_50,
  SE_NUT_UNSPECIFIED_51,
  SE_NUT_UNSPECIFIED_52,
  SE_NUT_UNSPECIFIED_53,
  SE_NUT_UNSPECIFIED_54,
  SE_NUT_UNSPECIFIED_55,
  SE_NUT_UNSPECIFIED_56,
  SE_NUT_UNSPECIFIED_57,
  SE_NUT_UNSPECIFIED_58,
  SE_NUT_UNSPECIFIED_59,
  SE_NUT_UNSPECIFIED_60,
  SE_NUT_UNSPECIFIED_61,
  SE_NUT_UNSPECIFIED_62,
  SE_NUT_UNSPECIFIED_63,
  SE_NUT_INVALID,
} SE_NalUnitType;

/**
 * Reference time update method. When non-linear operations occur the reference time should be updated in order to associate the new
 * PTS with the current time.
 */
typedef enum SE_RefTimeMeth
{
  SE_REFTIME_SyncToPTS,  ///< Synchronize reference time to pts after non-linear operation (e.g., SpinRenderLib_InvalidateInflightFrames).
  SE_REFTIME_Explicit    ///< Changed only explicitly using SE_RCOP_SetRefTime control operations.
} SE_RefTimeMeth;


/**
 * Pixel formats. Three main categories of pixel formats are supported: planar, semi-planar,
 * and packed. In planar formats, each component has its own plane. In semi-planar the
 * dominant component has its own plane and the two other components are pixel interleaved
 * in one plane. For packed formats only a single plane is available in which all components
 * reside.
 *
 * The SE_PixFormat is an important attribute of the Spin_Picture struct, and is used to derive
 * the number of planes, number of components, individual plane sample sizes, and  chroma
 * subsampling factors.
 *
 * @see Spin_Picture
 * @see SD_IsPacked
 * @see SD_IsPlanar
 * @see SD_NumPlanes
 * @see SD_NumComps
 * @see SD_Cx
 * @see SD_Cy
 *
 */
typedef enum SE_PixFormat
{
  SE_PF_None = -1,
  SE_PF_Planar400,        ///< 4:0:0 monochrome single plane.
  SE_PF_Planar420,        ///< 4:2:0 planar (each component in separate plane).
  SE_PF_Planar422,        ///< 4:2:2 planar (each component in separate plane).
  SE_PF_Planar444,        ///< 4:4:4 planar (each component in separate plane).
  SE_PF_Planar4444,       ///< Same as 4:4:4 planar but with an extra alpha plane.

  SE_PF_SemiPl420,        ///< 4:2:0 semi-planar. One plane for luma, and one for chroma. The U and V
                          ///< components are interleaved in a single plane on pixel basis, with U in
                          ///< in the least significant position.
  SE_PF_SemiPl422,        ///< 4:2:2 semi-planar. One plane for luma, and one for chroma.The U and V
                          ///< components are interleaved in a single plane on pixel basis, with U in
                          ///< in the least significant position.

  /**
  * In bit planar formats individual samples are not aligned to bytes, to reduce or eliminate the unused zero bits.
  * Instead the samples are aligned to either 10- or 12-bit boundaries depending on the bitdepth of the plane.
  * Plane bit depths of 9- and 10-bit use a 10-bit alignment, while 11-and 12-bit use a 12-bit alignment.
  * Strides of planes using bit packed 10-bit must be a multiple of 4, and for bit packed 12-bit a multiple of 2.
  */
  ///@{
  SE_PF_BitPlanar400,
  SE_PF_BitPlanar420,
  SE_PF_BitPlanar422,
  SE_PF_BitPlanar444,
  SE_PF_BitPlanar4444,
  ///@}
  ///@{
  /**
  * Same as bit planar, except that the chroma planes are interleaved.
  */
  SE_PF_BitSemiPl420,
  SE_PF_BitSemiPl422,
  ///@}

  /**
   * Packed 4:2:2. The components are interleaved on a pixel basis, e.g., UYVY (UY0VY1) with the U component in the
   * least significant position. All sample storage sizes are the same for each of the component, and the
   * maximum component bit depth is used to determine this. For instance, if the bit depth would be 12b
   * for Y and 8b for both U and V all samples would be placed in 2 bytes, resulting in 8 bytes for every
   * repetition of UYVY (two pixels).
  */
  SE_PF_Packed422,

  /**
   * Packed 4:2:2 according to the V210 format. In this format the 12 components of each set of 6 pixels are packed in
   * 4 blocks of 32-bit values. Each 32-bit value holds 3 10-bit components, 2-bits remain unused.
   *
   * block 1, bits  9 -  0: U0+1
   * block 1, bits 19 - 10: Y0
   * block 1, bits 29 - 20: V0+1
   * block 2, bits  9 -  0: Y1
   * block 2, bits 19 - 10: U2+3
   * block 2, bits 29 - 20: Y2
   * block 3, bits  9 -  0: V2+3
   * block 3, bits 19 - 10: Y3
   * block 3, bits 29 - 20: U4+5
   * block 4, bits  9 -  0: Y4
   * block 4, bits 19 - 10: V4+5
   * block 4, bits 29 - 20: Y5
   */
  SE_PF_Packed422_V210,
  /**
  * Packed 4:2:2 according to the DPX Filler Method A format. Similar to the V210 format the 12 components of each set of 6 pixels
  * are packed in 4 blocks of 32-bit values. Each 32-bit value holds 3 10-bit components, 2-bits remain unused. The difference
  * is in the position of the unused bits and the order of the samples inside a 32-bit block.
  *
  * block 1, bits 31 - 22: U0+1
  * block 1, bits 21 - 12: Y0
  * block 1, bits 11 -  2: V0+1
  * block 2, bits 31 - 22: Y1
  * block 2, bits 21 - 12: U2+3
  * block 2, bits 11 -  2: Y2
  * block 3, bits 31 - 22: V2+3
  * block 3, bits 21 - 12: Y3
  * block 3, bits 11 -  2: U4+5
  * block 4, bits 31 - 22: Y4
  * block 4, bits 21 - 12: V4+5
  * block 4, bits 11 -  2: Y5
  */
  SE_PF_Packed422_V210A,
  /**
   * Packed 4:4:4. The three components are interleaved on a pixel basis e.g., YUVYUVYUV or RGBRGBRGB. The Y
   * component resides in the lowest significant position followed by U and V.
   * The samples are byte aligned, meaning that a 10 bit component would take two bytes. All sample sizes
   * are the same for all of the component, and the maximum component bit depth is used to determine this. For
   * instance, if the bit depth would be 12b for Y and 8b for both U and V, the pixel size would be 3x2 = 6 bytes.
   */
  SE_PF_Packed444,
  SE_PF_Packed4444,       ///< Same as SE_PF_Packed444 but with an extra alpha component and the end.
  /**
   * Bit packed 4:4:4. The three components are inteleaved on a pixel basis. Each component occupies 10 or 12 bit,
   * depending on the bitdepth. The bitdepth is assumed to be equal for all components.
   */
  SE_PF_BitPacked444,
  /**
   * Packed 4:4:4 using 16 bit per pixel. Typical divisions over the components are Y6 U5 V5
   * or R5 G6 B5. The first component is in the MSB. Warning, not supported yet!
   */
  SE_PF_Packed444_16b,
  /**
  * Packed 4:4:4 using 32 bit per pixel. The layout of the samples is bit 9-0 for component 0, bit 19-10 for
  * component 1, and bit 29-20 for component 2. It is compatible to DXGI_FORMAT_R10G10B10A2_*. with the alpha bits zero.
  */
  SE_PF_Packed444_32b,
  /**
  * Reverse packed 4:4:4 using 32 bit per pixel. It is similar to SE_PF_Packed444_32b but the components in
  * reverse order. The layout is bit 31-22 for component 0, bit 21-12 for component 1, bit 11-2 for component 2.
  * With component order SE_CO_RGBA this format is compatible with the popular DPX 10-bit RGB FillerMethodA format.
  */
  SE_PF_Packed444RevA_32b,
  /**
   * Reverse packed 4:4:4 using 32 bit per pixel. It is similar to SE_PF_Packed444_32b but the components in
   * reverse order. Compared to SE_PF_Packed444RevA_32b there are no unused bits and the first component
   * allows it up to 12 bit. Component 1 and 2 have a maximum of 10 bit.
   * The layout of the samples is bit 31-20 for component 0, 19-10 for component 1, and 9-0 for component 2.
   * Examples layouts with equal component bit depths are  Y10 U10 V10 and R10 G10 B10, with the first two MSBs
   * unused, and with mixed bit depths Y12 U10 V10, and Y12 U8 V8.
   */
  SE_PF_Packed444RevB_32b,
  /** @name BC4 texture compressed formats
  * In the BC4 format, 8 bytes represent 4x4 samples of a plane. The first two bytes are the
  * reference colors. This is followed by 16x 3bit indices on for each sample. The UNORM
  * variant of the BC4 format is used.
  * The width and height of a plane in this format is 4x lower than the original, as one sample
  * of 8 bytes represents a 4x4 sample block.
  */
  ///@{
  SE_PF_BC4_400,
  SE_PF_BC4_420,
  SE_PF_BC4_422,
  SE_PF_BC4_444,
  SE_PF_BC4_4444,
  ///@}

  SE_NUM_PixFormats
} SE_PixFormat ;


/**
 * Pixel format categories. Used in combination with SE_ChromaFormat to derive a specific SE_PixFormat.
 * The main use is to communicate the pixel format style to use in situation where the chroma format is
 * not known yet.
 */
typedef enum SE_PixFmtCat
{
  SE_PFCAT_None = -1,
  SE_PFCAT_Planar,         ///< This category includes SE_PF_Planar400, SE_PF_Planar420, SE_PF_Planar422,
                           ///< SE_PF_Planar444 and SE_PF_Planar4444.
  SE_PFCAT_SemiPlanar,     ///< This category includes SE_PF_Semipl420 and SE_PF_Semipl422.
  SE_PFCAT_Packed32b,      ///< This category includes SE_PF_Packed444_32b and SE_PF_Packed4444_32b.
  SE_PFCAT_BitPlanar,      ///< Bit-packed planar formats.
  SE_PFCAT_BitSemiPl,      ///< Bit-packed semi-planar formats.
  SE_PFCAT_BC4,            ///< BC4 texture formats
  SE_PFCAT_UnCategorized,  ///< Remaining pixel formats, including SE_PF_Packed444, SE_PF_Packed4444 and SE_PF_Packed444_16b.
  SE_NUM_PixFmtCats
} SE_PixFmtCat;


/**
 * Geometry type
 */
typedef enum SE_GeometryType
{
  SE_GEOM_None = -1,
  SE_GEOM_Plane,        ///< Regular planar
  SE_GEOM_Cylinder,     ///< Cylinder projection
  SE_GEOM_ERP,          ///< Equirectangular projection
  SE_GEOM_CMP,          ///< Packed 3x2 cube map (according to JVET)
  SE_NUM_GeometryTypes,
} SE_GeometryType;

/**
 * Stereoscopy: configuration of left-eye and right-eye pictures
 */
typedef enum SE_Stereo3DFormat
{
  SE_STEREO3D_None = -1,  ///< 2D
  SE_STEREO3D_TopBottom,  ///< Top-bottom
  SE_STEREO3D_SideBySide, ///< Side-by-side
  SE_NUM_StereoFormats,
} SE_Stereo3DFormat;

/**
 * Returns if the SE_PixFormat is a packed format.
 */
__SD_UNUSED__
static int SD_IsPacked( SE_PixFormat pixfmt )
{
  return ( pixfmt >= SE_PF_Packed422 && pixfmt <= SE_PF_Packed444RevB_32b);
}

/**
 * Returns if the SE_PixFormat is a planar format.
 */
__SD_UNUSED__
static int SD_IsPlanar( SE_PixFormat pixfmt )
{
  return ( pixfmt <= SE_PF_Planar4444 );
}

/**
* Returns if the SE_PixFormat is a bit-packed semi-planar format.
*/
__SD_UNUSED__
static int SD_IsBitPlanar(SE_PixFormat pixfmt)
{
  return (pixfmt >= SE_PF_BitPlanar400 && pixfmt <= SE_PF_BitPlanar4444);
}

/**
* Returns if the SE_PixFormat is a semi-planar format.
*/
__SD_UNUSED__
static int SD_IsSemiPlanar(SE_PixFormat pixfmt)
{
  return (pixfmt == SE_PF_SemiPl420 || pixfmt == SE_PF_SemiPl422);
}

/**
* Returns if the SE_PixFormat is a bit-packed semi-planar format.
*/
__SD_UNUSED__
static int SD_IsBitSemiPlanar(SE_PixFormat pixfmt)
{
  return (pixfmt == SE_PF_BitSemiPl420 || pixfmt == SE_PF_BitSemiPl422);
}

/**
* Returns if the SE_PixFormat is a BC4 compressed format.
*/
__SD_UNUSED__
static int SD_IsBC4(SE_PixFormat pixfmt)
{
  return (pixfmt >= SE_PF_BC4_400 && pixfmt <= SE_PF_BC4_4444);
}

/**
 * Returns the number of planes of the SE_PixFormat.
 *
 * @param pixfmt Pixel format for which the number of planes is to be returned.
 *
 * @return Number of planes or SD_E_GENERIC when pixfmt is unknown.
 */
__SD_UNUSED__
static int SD_NumPlanes( SE_PixFormat pixfmt )
{
  switch (pixfmt){
  case SE_PF_Planar4444:
  case SE_PF_BC4_4444:
  case SE_PF_BitPlanar4444:
    return 4;
  case SE_PF_Planar420:
  case SE_PF_Planar422:
  case SE_PF_Planar444:
  case SE_PF_BC4_420:
  case SE_PF_BC4_422:
  case SE_PF_BC4_444:
  case SE_PF_BitPlanar420:
  case SE_PF_BitPlanar422:
  case SE_PF_BitPlanar444:
    return 3;
  case SE_PF_SemiPl420:
  case SE_PF_SemiPl422:
  case SE_PF_BitSemiPl420:
  case SE_PF_BitSemiPl422:
    return 2;
  case SE_PF_Planar400:
  case SE_PF_BC4_400:
  case SE_PF_BitPlanar400:
  case SE_PF_Packed422:
  case SE_PF_Packed422_V210:
  case SE_PF_Packed422_V210A:
  case SE_PF_Packed444:
  case SE_PF_Packed4444:
  case SE_PF_BitPacked444:
  case SE_PF_Packed444_16b:
  case SE_PF_Packed444_32b:
  case SE_PF_Packed444RevA_32b:
  case SE_PF_Packed444RevB_32b:
    return 1;
  default:
    return SD_E_GENERIC;
  }
}

/**
 * Returns the number of components of the SE_PixFormat. A component in this context is to be interpreted as
 * color channels (YUV,RGB = 3 components).
 *
 * @param pixfmt Pixel format for which the number of components is to be returned.
 *
 * @return Number of components or SD_E_GENERIC when pixfmt is unknown.
 */
__SD_UNUSED__
static int SD_NumComps(SE_PixFormat pixfmt)
{
  switch (pixfmt) {
  case SE_PF_Planar4444:
  case SE_PF_BC4_4444:
  case SE_PF_BitPlanar4444:
  case SE_PF_Packed4444:
    return 4;
  case SE_PF_Packed444_16b:
  case SE_PF_Packed444_32b:
  case SE_PF_Packed444RevA_32b:
  case SE_PF_Packed444RevB_32b:
  case SE_PF_Packed422:
  case SE_PF_Packed422_V210:
  case SE_PF_Packed422_V210A:
  case SE_PF_BC4_420:
  case SE_PF_BC4_422:
  case SE_PF_BC4_444:
  case SE_PF_Planar420:
  case SE_PF_Planar422:
  case SE_PF_Planar444:
  case SE_PF_BitPlanar420:
  case SE_PF_BitPlanar422:
  case SE_PF_BitPlanar444:
  case SE_PF_Packed444:
  case SE_PF_BitPacked444:
  case SE_PF_SemiPl420:
  case SE_PF_SemiPl422:
  case SE_PF_BitSemiPl420:
  case SE_PF_BitSemiPl422:
    return 3;
  case SE_PF_Planar400:
  case SE_PF_BC4_400:
  case SE_PF_BitPlanar400:
    return 1;
  default:
    return SD_E_GENERIC;
  }
}

/**
* Casts the pixel format to planar version of the pixel format.
*
* @param pixfmt Input pixel format.
*
* @return Casted pixel format.
*/
__SD_UNUSED__
static SE_PixFormat SD_CastToPlanar(SE_PixFormat pixfmt)
{
  switch (pixfmt) {
  case SE_PF_Planar400:
  case SE_PF_BitPlanar400:
  case SE_PF_BC4_400:
    return SE_PF_Planar400;
  case SE_PF_Planar420:
  case SE_PF_SemiPl420:
  case SE_PF_BitPlanar420:
  case SE_PF_BitSemiPl420:
  case SE_PF_BC4_420:
    return SE_PF_Planar420;
  case SE_PF_Planar422:
  case SE_PF_SemiPl422:
  case SE_PF_BitPlanar422:
  case SE_PF_BitSemiPl422:
  case SE_PF_BC4_422:
  case SE_PF_Packed422:
  case SE_PF_Packed422_V210:
  case SE_PF_Packed422_V210A:
    return SE_PF_Planar422;
  case SE_PF_Planar444:
  case SE_PF_BC4_444:
  case SE_PF_BitPlanar444:
  case SE_PF_Packed444:
  case SE_PF_BitPacked444:
  case SE_PF_Packed444_16b:
  case SE_PF_Packed444_32b:
  case SE_PF_Packed444RevA_32b:
  case SE_PF_Packed444RevB_32b:
    return SE_PF_Planar444;
  case SE_PF_BitPlanar4444:
  case SE_PF_Planar4444:
  case SE_PF_BC4_4444:
  case SE_PF_Packed4444:
    return SE_PF_Planar4444;
  default:
    break;
  }
  return (SE_PixFormat) SD_E_GENERIC;
}

/**
* Returns the associated byte aligned pixel format
*
* @param pixfmt Input pixel format.
*
* @return Byte aligned pixel format.
*/
__SD_UNUSED__
static SE_PixFormat SD_BitToByteFmt(SE_PixFormat pixfmt)
{
  switch (pixfmt) {
  case SE_PF_BitPlanar400:
    return SE_PF_Planar400;
    break;
  case SE_PF_BitPlanar420:
    return SE_PF_Planar420;
    break;
  case SE_PF_BitPlanar422:
    return SE_PF_Planar422;
    break;
  case SE_PF_BitPlanar444:
    return SE_PF_Planar444;
    break;
  case SE_PF_BitSemiPl420:
    return SE_PF_SemiPl420;
    break;
  case SE_PF_BitSemiPl422:
    return SE_PF_SemiPl422;
  case SE_PF_BitPacked444:
    return SE_PF_Packed444;
  }

  return pixfmt;
}


/**
* Determine the pix format category of the pixel format.
*
* @param pixfmt Input pixel format.
*
* @return pixel format category.
*/
__SD_UNUSED__
static SE_PixFmtCat SD_GetPixFmtCat(SE_PixFormat pixfmt)
{
  switch (pixfmt) {
  case SE_PF_Planar400:
  case SE_PF_Planar420:
  case SE_PF_Planar422:
  case SE_PF_Planar444:
  case SE_PF_Planar4444:
    return SE_PFCAT_Planar;
  case SE_PF_BC4_400:
  case SE_PF_BC4_420:
  case SE_PF_BC4_422:
  case SE_PF_BC4_444:
  case SE_PF_BC4_4444:
    return SE_PFCAT_BC4;
  case SE_PF_SemiPl420:
  case SE_PF_SemiPl422:
    return SE_PFCAT_SemiPlanar;
  case SE_PF_BitPlanar400:
  case SE_PF_BitPlanar420:
  case SE_PF_BitPlanar422:
  case SE_PF_BitPlanar444:
  case SE_PF_BitPlanar4444:
    return SE_PFCAT_BitPlanar;
  case SE_PF_BitSemiPl420:
  case SE_PF_BitSemiPl422:
    return SE_PFCAT_BitSemiPl;
  case SE_PF_Packed444:
  case SE_PF_Packed4444:
  case SE_PF_BitPacked444:
  case SE_PF_Packed444_16b:
  case SE_PF_Packed422:
  case SE_PF_Packed422_V210:
  case SE_PF_Packed422_V210A:
    return SE_PFCAT_UnCategorized;
  case SE_PF_Packed444_32b:
  case SE_PF_Packed444RevA_32b:
  case SE_PF_Packed444RevB_32b:
    return SE_PFCAT_Packed32b;
  default:
    break;
  }
  return (SE_PixFmtCat)SD_E_GENERIC;
}

/**
* Determine the chroma format of the pixel format.
*
* @param pixfmt Input pixel format.
*
* @return Chroma format.
*/
__SD_UNUSED__
static SE_ChromaFormat SD_GetChromaFormat(SE_PixFormat pixfmt)
{
  switch (pixfmt) {
  case SE_PF_Planar400:
  case SE_PF_BitPlanar400:
  case SE_PF_BC4_400:
    return SE_CF_400;
  case SE_PF_Planar420:
  case SE_PF_SemiPl420:
  case SE_PF_BitPlanar420:
  case SE_PF_BitSemiPl420:
  case SE_PF_BC4_420:
    return SE_CF_420;
  case SE_PF_Planar422:
  case SE_PF_SemiPl422:
  case SE_PF_BitPlanar422:
  case SE_PF_BitSemiPl422:
  case SE_PF_Packed422:
  case SE_PF_Packed422_V210:
  case SE_PF_Packed422_V210A:
  case SE_PF_BC4_422:
    return SE_CF_422;
  case SE_PF_Planar444:
  case SE_PF_Planar4444:
  case SE_PF_BitPlanar444:
  case SE_PF_BitPlanar4444:
  case SE_PF_Packed444:
  case SE_PF_Packed4444:
  case SE_PF_BitPacked444:
  case SE_PF_Packed444_32b:
  case SE_PF_Packed444RevA_32b:
  case SE_PF_Packed444RevB_32b:
  case SE_PF_Packed444_16b:
  case SE_PF_BC4_444:
  case SE_PF_BC4_4444:
    return SE_CF_444;
  default:
    break;
  }
  return (SE_ChromaFormat)SD_E_GENERIC;
}

/**
 * Returns chroma shift amount of the SE_PixFormat in horizontal direction.
 * For instance for planar 4:2:0 a subsampling factor of 2X exists for chroma. In this case
 * a 1 is returned. Typical usage is "width >> SD_Cx(pixfmt)".
 *
 * @param pixfmt Pixel format for which the horizontal chroma subsampling shift is to be returned.
 *
 * @return Horizontal shift amount for chroma subsampling.
 */
__SD_UNUSED__
static int SD_Cx( SE_PixFormat pixfmt )
{
  switch (pixfmt){
  case SE_PF_Planar400:
  case SE_PF_Planar4444:
  case SE_PF_Planar444:
  case SE_PF_BC4_400:
  case SE_PF_BC4_444:
  case SE_PF_BC4_4444:
  case SE_PF_SemiPl420:
  case SE_PF_SemiPl422:
  case SE_PF_BitPlanar400:
  case SE_PF_BitPlanar444:
  case SE_PF_BitPlanar4444:
  case SE_PF_BitSemiPl420:
  case SE_PF_BitSemiPl422:
  case SE_PF_Packed422:
  case SE_PF_Packed422_V210:
  case SE_PF_Packed422_V210A:
  case SE_PF_Packed444:
  case SE_PF_Packed4444:
  case SE_PF_BitPacked444:
  case SE_PF_Packed444_16b:
  case SE_PF_Packed444_32b:
  case SE_PF_Packed444RevA_32b:
  case SE_PF_Packed444RevB_32b:
    return 0;
  case SE_PF_Planar420:
  case SE_PF_Planar422:
  case SE_PF_BC4_420:
  case SE_PF_BC4_422:
  case SE_PF_BitPlanar420:
  case SE_PF_BitPlanar422:
    return 1;
  default:
    break;
  }
  return SD_E_GENERIC;
}

/**
 * Returns chroma shift amount of the SE_PixFormat in vertical direction.
 * For instance for planar 4:2:0 a subsampling factor of 2X exists for chroma. In this case
 * a 1 is returned. Typical usage is "height >> SD_Cy(pixfmt)".
 *
 * @param pixfmt Pixel format for which the vertical chroma subsampling shift is to be returned.
 *
 * @return Vertical shift amount for chroma subsampling.
 */
__SD_UNUSED__
static int SD_Cy( SE_PixFormat pixfmt )
{
  switch (pixfmt){
  case SE_PF_Planar400:
  case SE_PF_Planar422:
  case SE_PF_Planar444:
  case SE_PF_Planar4444:
  case SE_PF_BC4_400:
  case SE_PF_BC4_422:
  case SE_PF_BC4_444:
  case SE_PF_BC4_4444:
  case SE_PF_BitPlanar400:
  case SE_PF_BitPlanar444:
  case SE_PF_BitPlanar4444:
  case SE_PF_SemiPl422:
  case SE_PF_BitPlanar422:
  case SE_PF_BitSemiPl422:
  case SE_PF_Packed422:
  case SE_PF_Packed422_V210:
  case SE_PF_Packed422_V210A:
  case SE_PF_Packed444:
  case SE_PF_Packed4444:
  case SE_PF_BitPacked444:
  case SE_PF_Packed444_16b:
  case SE_PF_Packed444_32b:
  case SE_PF_Packed444RevA_32b:
  case SE_PF_Packed444RevB_32b:
    return 0;
  case SE_PF_SemiPl420:
  case SE_PF_Planar420:
  case SE_PF_BC4_420:
  case SE_PF_BitPlanar420:
  case SE_PF_BitSemiPl420:
    return 1;
  default:
    break;
  }
  return SD_E_GENERIC;
}

/**
 * Color models such as YUV, RGB, and XYZ. Used in combination with SE_ColorSpace and SE_ColorRange
 * to determine the right conversions when required.
 */
typedef enum SE_ColorModel
{
  SE_CM_None = -1,
  SE_CM_YUV,            ///< YUV color model with separated luminance and chrominance components.
  SE_CM_RGB,            ///< RGB color model with separated red, green, and blue components.
  SE_CM_XYZ,            ///< Not supported yet.
  SE_NUM_ColorModels
} SE_ColorModel ;

/**
* Component order. For packed formats the positioning depends on the endianness. For little endian formats
* the first component is assumed to be in the lowest byte addresses of the packed word. For big endian formats
* the first component is assumed to be in the highest byte address.
*/
typedef enum SE_CompOrder
{
  SE_CO_None = -1,
  SE_CO_YUVA_GBRA,
  SE_CO_YUVA = SE_CO_YUVA_GBRA,
  SE_CO_GBRA = SE_CO_YUVA_GBRA,
  SE_CO_RGBA,
  SE_CO_BGRA,
  SE_NUM_CompOrders
} SE_CompOrder;

/**
 * Color spaces such as bt.709 and bt.2020. Used in combination with SE_ColorModel and SE_ColorRange
 * to determine the right conversions when required.
 */
typedef enum SE_ColorSpace
{
  SE_CS_None = -1,
  SE_CS_BT601,          ///< ITU-R BT.601
  SE_CS_BT709,          ///< ITU-R BT.709
  SE_CS_BT2020,         ///< ITU-R BT.2020
  SE_CS_DCIP3,          ///< SMPTE 432-1 DCI-P3 D65
  SE_NUM_ColorSpaces
} SE_ColorSpace;

/**
 * Color ranges. Specifies if the full or limited range is utilized.
 */
typedef enum SE_ColorRange
{
  SE_CR_None = -1,
  SE_CR_Full,           ///< 0-255 for 8-bit and 0-1020 for 10-bit
  SE_CR_Limited,        ///< 16-235 for 8-bit and 64-960 for 10-bit
  SE_NUM_ColorRanges
} SE_ColorRange ;


/**
 * Non-linear to linear transfer functions types. The transfer function indicated which linear to
 * non-linear light transformation has been used on the picture.
 */
typedef enum SE_TransferFunc
{
  SE_TF_None = -1,
  SE_TF_SDR,            ///< Most common SDR transfer function BT.1886 (BT.601, BT.709, BT.2020).
  SE_TF_Linear,         ///< Indicates content is linear. Currently only used internally.
  SE_TF_ST2084,         ///< HDR transfer function used in HDR10, Dolby Vision, etc.
  SE_TF_STDB67,         ///< Hybrid log gamma transfer function as specified in ARIB STD-B67.
  SE_TF_GAMMA26,        ///< Gamma 2.6 used in DCI-P3 color space.
  SE_TF_SRGB,           ///< sRGB transfer function
  SE_NUM_TransferFuncs
} SE_TransferFunc ;

typedef enum SE_StreamMode
{
  SE_SM_AUTO,
  SE_SM_FILE,
  SE_SM_LIVE,
  SE_NumStreamModes
} SE_StreamMode;

typedef enum SE_SeekMode
{
  SE_SEEK_Rel,
  SE_SEEK_Abs,
  SE_NUM_SeekMethods
} SE_SeekMode;

/**
 * Number of bytes per set of samples for a certain format.
 *
 * @see SpinLib_GetBytesSamplesSet
 */
typedef struct Spin_BPS
{
  short iBytes;         ///< Number of bytes for each sample set.
  short iSamples;       ///< Sample set size. Picture width of formats using this size must be a multiple of iSamples.
} Spin_BPS;

/**
 * Converts the number of samples into the corresponding size in bytes.
 */
static int SD_Samples2Bytes(int samples, Spin_BPS bps)
{
  return samples* bps.iBytes / bps.iSamples;
}

/**
 * Converts a byte number into the corresponding number of samples.
 */
static int SD_Bytes2Samples(int bytes, Spin_BPS bps)
{
  return bps.iSamples * (bytes/ bps.iBytes);
}

/**
 * Spin_Plane holds the plane data. The attributes are in samples units not bytes. The actual
 * byte sizes can be determined using the SE_PixFormat and bit depths residing in the
 * Spin_Picture parent struct. For planar formats one sample unit is a single plane element.
 * For semi-planar formats one sample unit is a single plane element in the first plane. In the
 * interleaved plane, the samples units are counted for each element, i.e., a uv-pair would count
 * as two samples. For packed format, one sample units corresponds to one entire packed pixel.
 *
 * @see Spin_Picture
 */
typedef struct Spin_Plane
{
  int iWidth;           ///< Width of the plane
  int iHeight;          ///< Height of the plane
  int iStride;          ///< Stride of the plane. The next sample in the same column is stride
                        ///< samples further. The stride must be a multiple of the number of
                        ///< samples in a set in case the pixel format has samples with non-integer byte sizes.
  int iMarginX;         ///< Minimum available margin around the plane in horizontal direction.
                        ///< Margin applies to both sides of the plane.
  int iMarginY;         ///< Minimum available margin around the plane in vertical direction.
                        ///< Margin applies to both sides of the plane.

  int iCropLeft;        ///< Samples to crop for the visible window from the left plane boundary.
  int iCropRight;       ///< Samples to crop for the visible window from the right plane boundary.
  int iCropTop;         ///< Samples to crop for the visible window from the top plane boundary.
  int iCropBottom;      ///< Samples to crop for the visible window from the bottom plane boundary

  int iAlign;           ///< Address alignment for all samples in column 0 of the plane.
                        ///< (iStride*samplesSize) % iAlign must be 0.
  int iEndian;          ///< 0: little endian 1: big endian (limited support, only SE_Packed444_32b)
  void* pPlane;         ///< Starting position of the plane pointing at sample (0,0).
} Spin_Plane ;


/**
 * Spin_Picture holds the picture information and the plane data. It is one of the main communication
 * structs with and between the Spin Digital libraries. Some helper functions exists such as SpinLib_AllocFrame
 * and SpinLib_FreeFrame for improved usability.
 *
 * @see Spin_Plane
 * @see SpinLib_AllocFrame
 * @see SpinLib_FreeFrame
 */
typedef struct Spin_Picture
{
  void* pPlanesData;           ///< Allocated memory for planes data.
  int iAllocSize;              ///< Allocated size of pPlanesData in bytes. This value is used in
                               ///< SpinLib_ReformFrame to determine if the Spin_Picture can be
                               ///< Reformed to the new format.

  Spin_Plane asPlanes[4];      ///< Picture planes to access the sample data.
  int aiBitdepth[4];           ///< Up to 4 bit depths, one for each component
  /**
  * Component order of the planes data and bitdepth array.
  */
  SE_CompOrder eCompOrder;
  SE_PixFormat ePixFormat;      ///< Pixel format of the picture.
  SE_ColorModel eColorModel;    ///< Color model of the picture samples.
  SE_ColorSpace eColorSpace;    ///< Color space of the picture samples.
  SE_ColorRange eColorRange;    ///< Color range of the picture samples.
  SE_TransferFunc eTransferFunc;///< Transfer function of the picture samples.
  /**
   * Light level in cd/m2 corresponding to the maximum sample value.
   * When the transfer function is ST2084, this value represents the maximum light value present in the frame.
   * When set to zero a default value depending on the transfer function is assumed.
   */
  float fMaxLightLevel;
  /**
   * Analogous to fMaxLightLevel
   */
  float fMinLightLevel;

  SE_GeometryType eGeomType;       ///< Projection geometry of picture
  SE_Stereo3DFormat eStereo3DFormat; ///< Stereo format of picture
  float afFOV[2];               ///< Horizontal and vertical field of view in radians
  float afFrontCoor[3];         ///< Longitude, latitude, and roll in radians

  long long int llPts;          ///< Presentation time stamp.
  double dLocalTime;            ///< Optional field to store the local system timestamp using SpinLib_GetRealTime()
  void* pOpaquePic;             ///< Opaque ptr to higher level struct in which this Spin_Picture is contained.
  int data[16];                 ///< Opaque extra data. Usage depends on the specific library call.
} Spin_Picture ;

typedef struct Spin_Rational
{
  int iNum;
  int iDen;
} Spin_Rational;

typedef struct Spin_VideoFormat
{
  Spin_Picture sPicFmt;
  Spin_Rational sRate;
} Spin_VideoFormat;

typedef struct Spin_360ViewState {
  //static const int defFovPlane = 70;
  float fVFOV;
  float fYaw;
  float fPitch;
  float fZoom;

} Spin_360ViewState;

typedef enum SE_SampleFormat
{
  SE_SAMFMT_None = -1,
  SE_SAMFMT_S16,          ///< Signed 16-bit
  SE_SAMFMT_S32,          ///< Signed 32-bit
  SE_SAMFMT_FLT,          ///< Floating point [-1, +1]
  SE_NUM_SampleFormats,
} SE_SampleFormat;

__SD_UNUSED__
static int SD_GetSampleFmtSize(SE_SampleFormat eFmt)
{
  switch (eFmt) {
  case SE_SAMFMT_S16:
    return 2;
  case SE_SAMFMT_S32:
  case SE_SAMFMT_FLT:
    return 4;
  default:
    break;
  }
  return 0;
}


typedef struct Spin_AudioFormat
{
  int iSampleRate;
  SE_SampleFormat eSamFmt;
  int iNumChannels;
} Spin_AudioFormat;

typedef struct Spin_AudioBlock
{
  Spin_AudioFormat sFmt;
  void* pSamples;
  int iNumFrames;
  int64_t llPts;        ///< Presentation time stamp of first sample
  int data[4];          ///< For application specific use
} Spin_AudioBlock;

typedef double(*Spin_ExtTimeFunc)(void* pHandle);  ///< External ref time callback prototype.

/**
 * SpinLib_AllocFrame allocates the planes of Spin_Picture based on its attributes. All the fields that
 * describe the picture must be filled in. More specifically, the bit depths (aiBitdepth), pixel format (ePixFormat),
 * and the planes (asPlanes).
 *
 * For each relevant plane the width, height, margins and alignments must also be specified.
 * The width, height, and margin, represent the pixel units, not actual byte size. The alignment, however, is interpreted
 * in terms of bytes. The stride can be set to 0 to let the library decide the best stride, that suffices all the
 * requirements. It can also be set manually, for instance if a bigger stride is desired. When modification are
 * required the stride attribute is changed within the API call.
 *
 * @param pPic The Spin_Picture for which the planes are to be allocated.
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 *
 * @see Spin_Plane
 * @see Spin_Picture
 * @see SpinLib_FreeFrame
 */
SPINLIB_API int SpinLib_AllocFrame( Spin_Picture* pPic );

/**
 * Picture compatibility modes.
 */
typedef enum SE_PicCompatMode
{
  SE_PICCOMPAT_Exact,       //Pictures have exact same storage layout (dimension, pixel format, bitdepth, alignment)
  SE_NUM_PicCompatModes
} SE_PicCompatMode;


/**
 * SpinLib_IsPicCompatible returns if two picture are compatible according to the SE_PicCompatMode.
 *
 * @param pPic    Original picture
 * @param pPic2   Target picture to compare against
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 *
 * @see SE_PicCompatMode
 * @see Spin_Picture
 */
SPINLIB_API int SpinLib_IsPicCompatible( const Spin_Picture* pPic, const Spin_Picture* pPic2, SE_PicCompatMode ePicCompat);

/**
 * SpinLib_ReformFrame reforms the pPic into the target format. Reform does not realloc the
 * picture, but will recalculate the fields of pPic in order for the allocated space to be
 * reused as a picture specified in pTargetFmt.This function only succeeds if
 * pPic->iAllocSize >= pTargetFmt->iAllocSize and the iAlign of each plane of pPic
 * is larger or equal than all planes of pTargetFmt.
 *
 * @param pPic The Spin_Picture for which the picture if to be reformed.
 * @param pTargetFmt The Spin_Picture describing the desired new picture format.
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 *
 * @see Spin_Plane
 * @see Spin_Picture
 */
SPINLIB_API int SpinLib_ReformFrame( Spin_Picture* pPic, const Spin_Picture* pTargetFmt );

/**
 * SpinLib_FreeFrame frees the planes allocated by SpinLib_AllocFrame.
 *
 * @param pPic The Spin_Picture for which the planes should be freed.
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 *
 * @see SpinLib_AllocFrame
 */
SPINLIB_API int SpinLib_FreeFrame( Spin_Picture* pPic );

/**
 * @deprecated SpinLib_BytesPerSample returns the byte size of a sample in a particular plane of a certain pixel format. The bit depths
 * of each component must be provided for the calculation to succeed.
 *
 * @param pixfmt Pixel format used to calculate the sample size.
 * @param plane Plane number for which the sample size is calculated.
 * @param bitdepth Bit depth array of all planes of the picture. For non-default component orders the bit depth arrays
 * should be reordered before calling this function. Otherwise the aiBitdepth field of Spin_Picture can be uses directly.
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
DEPRECATED_API
SPINLIB_API int SpinLib_BytesPerSample( SE_PixFormat pixfmt, int plane, const int bitdepth[4] );

/**
* SpinLib_BytesPerSampleSet returns the byte size of the minimume sample set in a particular plane of a
* certain pixel format. The bit depths of each component must be provided for the calculation to succeed.
* Compared to SpinLib_BytesPerSample this function can also handle formats that do not have an integer
* number of bytes per sample.
*
* @param pixfmt Pixel format used to calculate the sample size.
* @param plane Plane number for which the sample size is calculated.
* @param bitdepth Bit depth array of all planes of the picture. For non-default component orders the bit depth arrays
* should be reordered before calling this function. Otherwise the aiBitdepth field of Spin_Picture can be uses directly.
*
* @return SD_OK on success. See spinretcodes.h for other return codes.
*/
SPINLIB_API Spin_BPS SpinLib_BytesPerSampleSet(SE_PixFormat pixfmt, int plane, const int bitdepth[4]);


/**
 * Returns a double that specifies the current real time in seconds.
 *
 * @return A double value that contains the current real time in seconds.
 */
SPINLIB_API double SpinLib_GetRealTime(void);

/**
 * Return an error message associated to the error code in a const char*. Not implemented yet!
 *
 * @param errcode Error code for which the error message is to be returned.
 *
 * @return The const char* to the error message.
 */
SPINLIB_API const char* SpinLib_GetErrorString( int errcode );

SPINLIB_API void SpinLib_GetColorFmtStr(SE_ColorModel eCm, SE_CompOrder eCo, SE_PixFormat ePf, char cfStr[5]);

/**
 * Returns the version string.
 */
SPINLIB_API const char* SpinLib_GetVersionString(void);

/**
 * Returns the major library version number.
 */
SPINLIB_API int SpinLib_GetMajorVersion(void);

/**
 * Returns the minor library version number.
 */
SPINLIB_API int SpinLib_GetMinorVersion(void);

#ifdef __cplusplus
}
#endif

/**@}*/

