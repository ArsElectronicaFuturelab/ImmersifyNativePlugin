/**
 * \file    spinfilter.h
 * \brief   Spin Digital raw image filter/conversion library header file
 * \author  Chi Ching Chi <chi@spin-digital.com>
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2015-2020, Spin Digital Video Technologies GmbH
 * Berlin, Germany
 */

#pragma once

#include "spincommon.h"

 /** \defgroup filter SpinFilter
 *  \brief Spin Digital image filter library API.
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
* Filter types. See SpinFilter_FilterDesc for detailed options per filter.
*/
typedef enum SE_FilterType
{
  SE_FILT_None = -1,
  SE_FILT_Auto,         ///< Auto filter, the library determines automatically the best filter chain for specified changes.
  /**
  * Resolution scaling filter. Support all pixel formats as input, but always produces planar output.
  * Can resize to any target resolution. For 420 the luma width and height of the destination has to
  * be a multiple of 2, for 422 only the luma width has to be a multiple of 2. The interpolation
  * methods are listed in SE_ScalingMeth.
  *
  * This filter supports frame pass execution and block-based parallel execution.
  */
  SE_FILT_Scale,
  /**
  * Cropping and padding filter. Only supports planar pixel formats. Can crop or pad to any target
  * resolution. For 420 the offsets and dimensions of the destination has to be a multiple of 2, for
  * 422 only the x-offset and width has to be a multiple of 2.
  *
  * This filter supports frame pass execution and block-based parallel execution.
  */
  SE_FILT_Crop,
  /**
  * Color space, color model/range, and transfer function conversion. Only supports 4:4:4 planar pixel
  * formats. For color model and range conversion this always uses a 3x3 matrix based conversion. If
  * (also) a color space and/or transfer function conversion is to be performed, the specified
  * SE_ColorConvMeth is used.
  *
  * This filter supports frame pass execution and fused block-based parallel execution.
  */
  SE_FILT_ColorConv,
  /**
  * Pixel layout and chroma format conversion filter. Can convert from any SE_PixFormat to another.
  * In case the chroma format changes simple averaging and duplication operation are used.
  *
  * This filter supports frame pass execution and fused block-based parallel execution.
  */
  SE_FILT_FormatConv,
  /**
  * Blends an overlay image on to the input image. Only supports planar pixel formats for the input
  * image. The overlay image is automatically scaled to the required size resulting from the blending
  * description. The overlay image is also converted to the input image format before blending.
  *
  * This filter supports frame pass execution and fused block-based parallel execution.
  */
  SE_FILT_Blend,
  /**
  * Changes the orientation of the image according to the desired SE_Orientation
  *
  * This filter supports frame pass execution and block-based parallel execution.
  */
  SE_FILT_Orientation,
  /**
  * Image geometry conversion filter. Can convert between 360 degrees spherical formats, and
  * project 360 degrees spherical formats to non-spherical formats such as plane and cylinder
  * (SE_GeometryType).
  *
  * This filter supports frame pass execution and block-based parallel execution. A fence is placed
  * before this filter in the filter chain execution.
  */
  SE_FILT_GeomProject,
  /**
  * Texture compression filter. Can convert from planar to a BC4 (compression), and a BC4
  * to planar (decompression). The chroma format of the input and output must be the same.
  *
  */
  SE_FILT_TexCompress,
  /**
  * Split filter. Splits or duplicates the output in to multiple output images.
  * Supported operations are duplicate, tiling, and 2-sample deinterleave.
  * The split filter is the final filter in the chain. Adding additional filters
  * will fail.
  */
  SE_FILT_Split,
  SE_NUM_FilterTypes,
} SE_FilterType ;


/**
* Interpolation methods for resolution scaling and geometry mapping.
*/
typedef enum SE_ScalingMeth
{
  SE_SCALE_None = -1,
  SE_SCALE_Nearest,         ///< Nearest neighbor.
  SE_SCALE_Linear,          ///< Linear interpolation.
  SE_SCALE_Cubic,           ///< Cubic interpolation.
  SE_SCALE_Lanczos2,        ///< Lanczos interpolation with 2 lobes.
  SE_SCALE_Lanczos3,        ///< Lanczos interpolation with 3 lobes.
  SE_SCALE_CustomChroma,    ///< Chroma sampling with up to 8-taps custom coefficients.
  SE_NUM_ScalingMeths,
} SE_ScalingMeth ;

/**
* Conversion methods for the SE_FILT_ColorConv filter.
*/
typedef enum SE_ColorConvMeth
{
  SE_CCONV_None = -1,
  SE_CCONV_Exact,           ///< Use exact computation for each conversion operation.
  SE_CCONV_3DLUT,           ///< Use 3D LUT for entire conversion process.
  SE_NUM_ColorConvMeths,
} SE_ColorConvMeth ;

/**
* Orientations changes for the SE_FILT_Orientation filter.
*/
typedef enum SE_Orientation
{
  SE_ORIENT_None = -1,
  SE_ORIENT_Default,
  SE_ORIENT_90,               ///< 90 degrees counter clock rotation
  SE_ORIENT_180,              ///< 180 degrees rotation
  SE_ORIENT_270,              ///< 270 degrees counter clock rotation
  SE_ORIENT_FlipHor,          ///< Mirror horizontally (over vertical axis)
  SE_ORIENT_FlipVer,          ///< Mirror vertically (over horizontal axis)
  SE_NUM_Orientations,
} SE_Orientation;

/**
* Split modes supported by the split filter
*/
typedef enum SE_SplitMode
{
  SE_SPLIT_None = -1,
  SE_SPLIT_Clone,             ///< Duplicate image iNumOutput times
  SE_SPLIT_2SD,               ///< 2 sample deinterleave in 4 sub images
  SE_NUM_SplitModes
} SE_SplitMode;

/**
* Execution modes of the filter chain.
*/
typedef enum SE_FilterExecMode
{
  SE_FILTEXEC_None = -1,
  /**
  * Allow for frame and block based parallel execution for supported filters.
  */
  SE_FILTEXEC_Tiled,
  /**
  * Force frame pass execution with one thread per picture in flight.
  */
  SE_FILTEXEC_FramePass,
} SE_FilterExecMode;


typedef struct SpinFilter_Frame
{
  Spin_Picture asPics[16];      ///< Picture array
  int iNumPics;                 ///< Number of pictures
  int64_t llPts;                ///< PTS of all pictures
} SpinFilter_Frame;

/**
* SpinFilter configuration struct for initializing the filter context.
*
* @see SpinRenderLib_Open
*/
typedef struct SpinFilter_Param
{
  Spin_Picture sFiltInFmt;            ///< Describes input picture format of the filter context.
  SE_FilterExecMode eFiltExecMode;    ///< Parallel execution mode.
  /**
  * Number of pictures in flight. When set higher than one the SpinFilterLib_ConvFrame
  * API call becomes asynchronous with iNumPicInFlight-1 calls delay.
  */
  int iNumPicInFlight;
  struct
  {
    int iNumThreads;                    ///< Number of filtering threads.
    uint64_t aiCPUAffinityMask[16];     ///< CPU affinity mask.
  }asThreadPoolConfig[4];               ///< Filtering thread pools.Currently, the number of thread pools should be the same as Number of pictures in flight and maximum 4.

  int bAutoFilterInjection;             ///< Not implemented yet. Enable auto format conversion filter injection when required.

  SD_ExtLogFunc fLogFunc;         ///< Callback function for log messages.
  void* hLogHandle;               ///< Callback handle for fLogFunc callback function.

} SpinFilter_Param;

/**
* Overlay picture and configuration for the SE_FILT_Blend filter.
*/
typedef struct SpinFilter_Overlay
{
  Spin_Picture sOverlay;          ///< Overlay picture.
  /** Normalized offset and size of the overlay. Top-left corner is origin. */
  ///@{
  float fX, fY, fW, fH;
  ///@}

  /**
  * Opacity of the overlay. If the overlay picture already has an alpha channel,
  * the opacity is multiplied.
  */
  float fOpacity;
} SpinFilter_Overlay;

/**
* Description of a single filter link. Use SpinFilterLib_AddFilter to add a filter to the filter chain
* described by SpinFilter_FilterDesc.
*
* @see SpinFilterLib_AddFilter
*/
typedef struct SpinFilter_FilterDesc
{
  SE_FilterType filterOp;           ///< Filter operation. Defines which union member contains valid data.
  bool bExport;                     ///< Export intermediate output of this filter even if it is not the last filter added.
  union{
    struct {
      SE_ScalingMeth eMeth;         ///< Interpolation method for resolution scaling and geometry conversion.
      int iWidth;                   ///< Desired width, unchanged if set to 0. When iHeight is 0 the height is automatically calculated to maintain aspect ratio.
      int iHeight;                  ///< Desired height, unchanged if set to 0. When iWidth is 0 the width is automatically calculated to maintain aspect ratio.
      /**
      * Setting iPaddingMode will ensure that the aspect ratio is maintained, even if width and height
      * specify a different aspect ratio. The specified width or height is achieved with padding.
      * 0: No padding will be applied
      * 1: Padding evenly on oppposite sides
      * 2: Padding on left and top sides
      * 3: Padding on right and top sides
      * 4: Padding on right and bottom sides
      * 5: Padding on left and bottom sides
      */
      int iPaddingMode;
      SE_ChromaFormat eCf;          ///< Desired chroma format, unchanged when set to an unknown value.
      SE_ColorModel eCm;            ///< Desired color model, unchanged when set to an unknown value.
      SE_ColorRange eCr;            ///< Desired color range, unchanged when set to an unknown value.
      /**
      * Desired color space, unchanged when set to an unknown value. Color space conversion is performed
      * using the 3D-LUT approach.
      */
      SE_ColorSpace eCs;
      /**
      * Desired transfer function, unchanged when set to an unknown value. Transfer function conversion
      * is performed using the 3D-LUT approach.
      */
      SE_TransferFunc eTf;
      /**
      * Absolute light level corresponding to the maximum pixel value of the output frame.
      * Is ignored in absolute transfer function such as ST2084 for which the maximum light level is standardized.
      */
      float fMaxCLL;
      /**
      * 0: Clip conversion when transforming colorspaces and transfer functions
      * 1: PQ domain tonemapping according to BT.2390
      */
      int iToneMap;
      int iBitdepth;                ///< Desired bit depth.
      SE_GeometryType eGeom;        ///< Target geometry type.
      /**
      * FOV for rectilinear and cylindrical projection. Derived from resolution when set to 0.
      */
      ///@{
      float fFovH;
      float fFovV;
      ///@}
    } sAutoFmt;
    struct {
      SE_ScalingMeth eMeth;         ///< Interpolation method for resolution scaling.
      int iWidth;                   ///< Desired width, unchanged if set to 0. When iHeight is 0 the height is automatically calculated to maintain aspect ratio.
      int iHeight;                  ///< Desired height, unchanged if set to 0. When iWidth is 0 the width is automatically calculated to maintain aspect ratio.
      SE_ChromaFormat eCf;          ///< Desired chroma format, unchanged when set to an unknown value.
      /**
      * Custom filter coeffs for chroma up- or down-sampling. Up to 8 taps filters are supported.
      * For upsampling two filters have to be specified, for downsampling only one.
      */
      float afChromaSamplingCoeffs[2][2][8];
      /**
      * Sampling taps for the horizontal and vertical filters. When set to zero, it indicates no filter is present.
      */
      int aiChromaSamplingTaps[2][2];
    } sScale;
    struct {
      int iOffsetX;                 ///< Horizontal offset to start cropping. Negative values will pad the picture instead. Top-left is origin.
      int iOffsetY;                 ///< Vertical offset to start cropping. Negative values will pad the picture instead. Top-left is origin.
      int iWidth;                   ///< Target width of the result image. If the width would pass the border of the original image, the resulting image will include padding.
      int iHeight;                  ///< Target height of the result image. If the width would pass the border of the original image, the resulting image will include padding.
      int bColor;                   ///< Enables custom padding color. Default padding color is black.
      int aiPadColor[4];            ///< Padding color values.
    } sCrop;
    struct {
      SE_ColorModel eCm;            ///< Desired color model, unchanged when set to an invalid value.
      SE_ColorRange eCr;            ///< Desired color range, unchanged when set to an invalid value.
      SE_ColorSpace eCs;            ///< Desired color space, unchanged when set to an invalid value.
      SE_CompOrder eCo;             ///< Desired component order, unchanged when set to an unknown value.
      SE_TransferFunc eTf;          ///< Desired transfer function, unchanged when set to an unknown value.
      /**
      * Target absolute light level corresponding to the maximum pixel value.
      */
      float fMaxCLL;
      /**
      * Target absolute light level corresponding to the minimum pixel value.
      */
      float fMinCLL;
      /**
      * 0: Clip conversion when transforming colorspaces and transfer functions
      * 1: PQ domain tonemapping according to BT.2390
      */
      int iToneMap;
      SE_ColorConvMeth eMeth;       ///< Color conversion method.
      int iLutDim;                  ///< 3D-LUT dimension (iLutDim x iLutDim x iLutDim).
      /**
       * Custom LUT. Each entry is 4 floats (for 128-bit alignment) in the range 0.0 to 1.0.
       * Lookup order is (x,y,z), with (x,y,z) located in float position 4*(x + y*iLutDim + z*iLutDim*iLutDim).
       * An LUT entry has the x value in the first position.
       */
      float* pCustomLUT;
    } sColorConv;
    struct {
      SE_PixFormat eTgtPixFmt;      ///< Target pixel format.
      int aiBitdepth[4];            ///< Target bit depth for each component.
    } sFmtConv;
    struct {
      SpinFilter_Overlay* pOverlay; ///< Pointer to overlay description.
    } sBlend;
    struct {
      SE_Orientation eOrient;       ///< Target orientation change.
    } sOrient;
    struct {
      SE_ScalingMeth eMeth;         ///< Interpolation method for geometry conversion.
      SE_GeometryType eGeom;        ///< Target geometry
      int iWidth;                   ///< Desired width of the output picture. When set to zero it is automatically derived.
      int iHeight;                  ///< Desired height of the output picture. When set to zero it is automatically derived.
      ///@{
      float fFovH;   ///<FOV for rectilinear and cylindrical projection
      float fFovV;
      ///@}
      /**
      * Relative cylinder radius for projection. Cylinder and sphere overlap in the front facing point (0,0,R).
      */
      float fCylRad;
      /**
      * When set to 1 actively resolves rotation of input. The resulting picture will have its adFrontCoor resolve to (0.0, 0.0).
      * Otherwise the adFrontCoor is preserved in the resulting picture.
      * With eGeom SE_GEOM_Plane this is always performed, even when set to 0.
      */
      int bResolveRotate;
      float fYaw;                   ///< Initial yaw offset. FIXME check scale and direction
      float fPitch;                 ///< Initial pitch offset. FIXME
    } sGeomConv;
    struct {
      SE_PixFmtCat eTgtFmtCat;      ///< Either SE_PFCAT_BC4 or SE_PFCAT_Planar.
      int iBitdepth;                ///< Only used when converting to planar(decompressing).
    } sTexCompress;
    struct {
      SE_SplitMode eSplitMode;
      int iNumOutput;               ///< Number of output picture. Must be set for SE_SPLIT_Clone
      SE_PixFormat ePixFmt;         ///< Output pixel format for SE_SPLIT_2SD. Supported formats: SE_Packed422_V210
    } sSplit;
  } ;
} SpinFilter_FilterDesc;


typedef void* SpinFilterLib_Handle;

/**
* Opens a filter context and returns a handle on success.
*
* @param phFilter The filter context handle is returned in *phHilter.
* @param pParam Pointer to the SpinFilter_Param struct used to initialize the filter context.
*
* @see SpinFilter_Param
* @see Spin_Picture
*
* @return SD_OK on success. See spinretcodes.h for other return codes.
*/
SPINLIB_API int SpinFilterLib_Open( SpinFilterLib_Handle* phFilter, const SpinFilter_Param* pParam );

/**
* Adds a filter to the filter context. If compatible the filter is appended to the back of the filter chain.
*
* @param hFilter Filter context handle.
* @param pFiltDesc Pointer to filter description.
*
* @see SpinFilter_FilterDesc
*/
SPINLIB_API int SpinFilterLib_AddFilter( SpinFilterLib_Handle hFilter, const SpinFilter_FilterDesc* pFiltDesc );

/**
* Finalizes the filter chain constructed in the filter context. After this call SpinFilterLib_ConvFrame may be called.
*
* @param hFilter Filter context handle.
* @return SD_OK on success
*/
SPINLIB_API int SpinFilterLib_FinalizeChain( SpinFilterLib_Handle hFilter );

/**
* Returns the number of output pictures the filter chain produces and expects.
* SpinFilterLib_AllocOutFrame and SpinFilterLib_ConvFrame will assume these number
* Spin_Picture structs are behind the pointer.
*
* This function returns 1, unless a split filter is used.
*
* @param hFilter Filter context handle.
*
* @return Number of output pictures.
*/
SPINLIB_API int SpinFilterLib_GetNumOutputPics(SpinFilterLib_Handle hFilter);


/**
* Allocates an output picture in the result format of the filter chain. Pictures allocated in this way are guaranteed
* to be compatible to be used as the output picture of SpinFilterLib_ConvFrame.
* SpinLib_FreeFrame can be called to free allocated pictures.
*
* @param hFilter Filter context handle.
* @param pOutPic Pointer to (empty) picture struct. The allocated picture is filled in pOutPic.
*
* @return SD_OK on success.
*/
SPINLIB_API int SpinFilterLib_AllocOutFrame( SpinFilterLib_Handle hFilter, Spin_Picture* pOutPic );


/**
* Fills the SpinFilter_Frame with the picture(s) the filter context consumes or produces.
* If bInput is set true, a SpinFilter_Frame compatible with the input frame of SpinFilterLib_ConvFrame2 is allocated.
* If bInput is set false, a SpinFilter_Frame compatible with the output frame of SpinFilterLib_ConvFrame2 is allocated.
* If bAllocMem is false, the SpinFilter_Frame picture struct are allocated and filled with the picture formats,
* but does not allocate the memory for the picture planes.
* If bAllocMem is true, the memory for the pictures is allocated as well.
* SpinFilterLib_FreeFrame must be called to free the allocated SpinFilter_Frame.
*
* The SpinFilterLib_AllocFrame replaced both SpinFilterLib_AllocOutFrame and SpinFilterLib_GetNumOutputPics as the SpinFilter_Frame also holds the number of pictures.
*
* @param hFilter Filter context handle.
* @param pFrame Pointer to SpinFilter_Frame in which the Spin_Picture(s) are returned.
*
* @return SD_OK on success.
*/
SPINLIB_API int SpinFilterLib_AllocFrame(SpinFilterLib_Handle hFilter, SpinFilter_Frame* pFrame, bool bInput, bool bAllocMem);

/**
* Frees a SpinFilter_Frame allocated with SpinFilterLib_AllocFrame
* When successful the pFrame struct is cleared on return.
*
* @param hFilter Filter context handle.
* @param pFrame Pointer to the SpinFilter_Frame that is to be freed.
*
* @return SD_OK on success.
*/
SPINLIB_API int SpinFilterLib_FreeFrame(SpinFilterLib_Handle hFilter, SpinFilter_Frame* pFrame);


/**
* Convert an input picture to the output picture according to the operation in the filter chain.
* This function may only be called after SpinFilterLib_FinalizeChain. The *ppSrcPic and *ppDstPic
* are consumed and returned in pairs by this call. If iNumPicInFlight == 1, the pictures are
* returned immediately.
* If iNumPicInFlight > 1, this API call becomes asynchronous, and pictures are returned with
* iNumPicInFlight-1 calls delay. To flush the pictures out *ppInPic and *ppOutPic should be set to NULL,
* to prevent the call from issuing more picture filtering.
*
* @param hFilter Filter context handle.
* @param ppInPic Pointer to pointer of the input picture. If *ppInPic is NULL, no filter operation is
* performed due to the lack of an input picture. The processed pictures are returned in *ppInpic.
* @param ppOutPic Pointer to pointer of the output picture. If *ppOutPic is NULL, no filter operation is
* performed due to the lack of an output picture. The processed pictures are returned in *ppOutpic.
*
* @return Returns 1 when a picture is returned and 0 otherwise. See spinretcodes.h for other return codes.
*/
SPINLIB_API int SpinFilterLib_ConvFrame( SpinFilterLib_Handle hFilter, Spin_Picture** ppInPic, Spin_Picture** ppOutPic );

/**
* Convert an input frame to the output frame according to the operation in the filter chain.
* This function may only be called after SpinFilterLib_FinalizeChain. The pInFrame and pOutFrame
* function both as source and destination. If valid frames are provided a filter operation will be queued.
* Then if a frame filtering is ready for output the corresponding in and out frame pair are returned in pInFrame and pOutFrame.
* If iNumPicInFlight == 1, the same frame pair is returned within the same call.
* If iNumPicInFlight > 1, this API call becomes asynchronous, and frames are returned with
* iNumPicInFlight-1 calls delay. To flush the frames use SpinFilterLib_GetFrame.
*
* @param hFilter Filter context handle.
* @param pInFrame Pointer to the input frame. If pInFrame->iNumPics is 0, no filter operation is
* performed as no input picture is made available. An earlier input frame is returned in pInFrame.
* @param pOutFrame Pointer to the output frame. If pOutFrame->iNumPics is 0, no filter operation is
* performed as no output picture is made available. An earlier processed frame is returned in pOutFrame.
*
* @return Returns 1 when a frame is returned and 0 otherwise. See spinretcodes.h for other return codes.
*/
SPINLIB_API int SpinFilterLib_ConvFrame2(SpinFilterLib_Handle hFilter, SpinFilter_Frame* pInFrame, SpinFilter_Frame* pOutFrame);

/**
 * Returns a pair of input and output SpinFilter_Frame structs when available. 
 * The typical use case of this function is to flush the final pictures out of the filter context
 * when more than a single frame-in-flight is used.
 *
 * @param hFilter Filter context handle.
 * @param pInFrame Pointer to the input frame.
 * @param pOutFrame Pointer to the output frame. 
 *
 * @return Returns 1 when a picture is returned and 0 otherwise.
 *
 * @see SpinFilter_Frame
 * @see SpinFilterLib_ConvFrame2
 */
SPINLIB_API int SpinFilterLib_GetFrame(SpinFilterLib_Handle hFilter, SpinFilter_Frame* pInFrame, SpinFilter_Frame* pOutFrame);


/**
 * Checks if frames are still remaining in the filte context.
 *
 * @return Returns true when a frame is remaining.
 */
SPINLIB_API bool SpinFilterLib_HasFrame(SpinFilterLib_Handle hFilter);

/**
* Closes the filter context.
*
* @param phFilter The filter context handle to close. *phRender is set to NULL after successfully
* closing the renderer. Pictures allocated by SpinFilterLib_AllocOutFrame are not freed and should
* be freed individually using SpinLib_FreeFrame.
*
* @return SD_OK on success.
*/
SPINLIB_API int SpinFilterLib_Close( SpinFilterLib_Handle* phFilter);

/**
* Returns the filter chain descriptors. The descriptors describe the internal filter chain. More filter descriptors might be
* returned than filter added, due to the automatic injection of filters.
*
* @param hFilter Filter context handle.
* @param ppFilters Pointer to a pointer of a filter descriptor. The library will make an array of filter descriptors
* available through this pointer. The filter descriptors are placed in chain order in the array. ppFilter is valid until next API call.
* @param piNumFilterDescs Number of filter descriptors available in the array.
*
* @return SD_OK on success.
*/
SPINLIB_API void SpinFilterLib_GetFilterChain(SpinFilterLib_Handle hFilter, const SpinFilter_FilterDesc **ppFilters, int* piNumFilterDescs);

/**
* Returns the filter chain in a printable string form. The returned string is valid until next API call.
*
* @return non-null char pointer on success.
*/
SPINLIB_API const char* SpinFilterLib_GetFilterChainString(SpinFilterLib_Handle hFilter);

#ifdef __cplusplus
}
#endif

/** @}*/


