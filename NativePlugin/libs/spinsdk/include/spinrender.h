/**
 * \file    spinrender.h
 * \brief   Spin Digital video renderer header file
 * \author  Chi Ching Chi <chi@spin-digital.com>
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2016-2019, Spin Digital Video Technologies GmbH
 * Berlin, Germany
*/

#pragma once

/** \defgroup renderer SpinRender
 *  \brief Spin Digital renderer library API.
 *  @{
 */

#include "spincommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* Container for platform specific window handles.
*/
typedef struct SpinRender_WindowDesc
{
  /**
  * WINAPI -> HWND,
  * XCB -> xcb_connection_t*,
  * Wayland -> wl_display*
  */
  void* hWindowHandle;
  /**
  * For WINAPI -> unused,
  * XCB -> xcb_generate_id (cast from uint32_t to pointer type),
  * Wayland -> wl_surface*
  */
  void* hExtraHandle;
} SpinRender_WindowDesc;

/**
 * Vertical sync mode.
 *
 * @see SpinRenderLib_Control
 * @see SpinRender_CtrlOpDesc
 */
typedef enum SE_VSyncMode
{
  SE_VS_Off,            ///< Vertical sync off. Frames are presented as soon as they are ready.
                        ///< Tearing might occur especially in full screen mode.
  SE_VS_On,             ///< Vertical sync on. Tearing does not occur, but the maximum framerate is locked to the panel refresh rate.
  SE_NUM_VSyncModes
} SE_VSyncMode;

/**
* Render mode.
*/
typedef enum SE_RenderMode
{
  SE_RENDERMODE_None = -1,
  /**
   * Render rate coupled with to the video rate. (GPU)
   */
  SE_RENDERMODE_Coupled,
  /**
   * Separate threads to control video frame timing and presentation timing. (GPU, SDI)
   * Allows for low latency response to control operations (navigate, zoom/pan),
   * and improves general smoothness of the video.
   */
  SE_RENDERMODE_Decoupled,
  /**
   * Same as decoupled, but a seperate pair of threads for each device. (GPU)
   * Allows for different display rates in the same systems.
   */
  SE_RENDERMODE_DecoupledAsync,
  SE_NUM_RenderModes
} SE_RenderMode;

/**
* Multi device render mode.
*/
typedef enum SE_DeviceMode
{
  SE_DEVICEMODE_None = -1,
  SE_DEVICEMODE_Single,         ///< Regular single device single window/device rendering.
  SE_DEVICEMODE_Clone,          ///< Rendering same input on multiple windows/devices.
  /**
   * Combine displays as a single large surface.
   * The input is split statically over multiple windows/devices with a fixed division.
   * Render devices only receive their visible region. It is not possible to zoom, move,
   * or rotate the content in this mode.
   */
  SE_DEVICEMODE_Tiled,
  /**
   * Combine displays as a single large surface, while maintaining full interactive capabilities.
   * The full input is sent to each each device and only the required portion is rendered.
   */
  SE_DEVICEMODE_Tiled2,
  /**
   * Map and render for every device a different picture buffer. The user is responsible for
   * splitting the input. Useful for multi input rendering.
   * Currently supports only up to 8 devices.
   */
  SE_DEVICEMODE_MultiBuffer,
  /**
  * Alternate frame rendering. Splits frames onto multiple output devices.
  * Each device the render context is opened with, has a corresponding stream id.
  * Frames are rendered using the time stamps of stream 0, after a frame is received for each output device.
  * Useful for rendering single bitstream multiview video.
  */
  SE_DEVICEMODE_Alternate,
  SE_NUM_DeviceModes,
} SE_DeviceMode;

/**
* Render output type.
*
* @see SpinRender_Output
*/
typedef enum SE_DeviceOutputMode
{
  SE_DEVICEOUTPUT_Window,      ///< Renders via a windowing system, SpinRender_WindowDesc is required for rendering.
  SE_DEVICEOUTPUT_Direct       ///< Renders directly to device, no extra handle is required for rendering.
} SE_DeviceOutputMode;

/**
* Description of a render format.
*/
typedef struct SpinRender_Format
{
  SE_ColorModel eCm;            ///< Color model of the rendering target.
  SE_ColorSpace eCs;            ///< Color space of the rendering target.
  SE_ColorRange eCr;            ///< Color range of the rendering target.
  SE_TransferFunc eTFunc;       ///< Transfer function of the rendering target.
  float fMaxLightLevel;         ///< Maximum light level in nits.
  float fMinLightLevel;         ///< Minimum light level in nits.
} SpinRender_Format;

/**
* Description of a render output. A render adapter can have one or more of these outputs.
*
* @see SpinRender_Adapter
*/
typedef struct SpinRender_Output
{
  char acDescription[32];       ///< First 32 characters of the output name if available.
  int iTopLeftX;                ///< Topleft x coordinate of the output. Set if supported by platform and device type.
  int iTopLeftY;                ///< Topleft y coordinate of the output. Set if supported by platform and device type.
  int iWidth;                   ///< Maximum output width.
  int iHeight;                  ///< Maximum output height.
  SE_DeviceOutputMode eOutputMode;  ///< Output mode.
  /**
  * Output specific handle.
  * WINAPI + D3D12 -> HMONITOR,
  * XCB/Wayland + Vulkan -> VkDisplayKHR
  * Not available in other combinations.
  */
  void* hHandle;
} SpinRender_Output;

/**
* Render devices types
*/
typedef enum SE_RenderDeviceType
{
  SE_DT_None = -1,
  SE_DT_D3D12,                  ///< D3D12 GPU device. Available on Window 10 using AMD, Intel, and Nvidia graphics cards.
  SE_DT_VULKAN,                 ///< Not supported. Vulkan GPU device (experimental). Available on Window 10 and Linux using AMD, Intel, and Nvidia graphics cards.
  SE_DT_OUTCARD,                ///< Dedicated output card. Currently only supporting AJA Corvid88 SDI out cards.
  SE_NUM_RenderDeviceTypes,
} SE_RenderDeviceType;

/**
* SpinRender_Adapter description.
*/
typedef struct SpinRender_Adapter
{
  char acDescription[128];      ///< First 128 characters of the adapter name if available.
  int iDeviceId;                ///< Global render device id.
  SE_RenderDeviceType eDevType; ///< Device type.
  int iLocalId;                 ///< ID within device type family.
  int iCapabilties;             ///< Adapter capabilities (currently not set).

  /**
  * Number of render outputs associated to this adapter
  * For GPU-like devices this represent the number of connected monitors surfaces (NVida Mosaic and AMD Eyefinity count as single surface).
  * Currently only available when using WINAPI + D3D12, and Linux + Vulkan.
  */
  int iNumOutputs;
  SpinRender_Output sOutputs[16];
} SpinRender_Adapter;

typedef enum SE_MemoryObjectType
{
  SE_MOT_Host,
  SE_MOT_Device,
  //SE_MOT_DXVA,
} SE_MemoryObjectType;

typedef struct SpinRender_Frame
{
  SE_MemoryObjectType eType;
  int iNumPics;             ///< Number of mapped picture buffers. Set to 1 except for SE_DEVICEMODE_MultiBuffer.
  int bNoRender;            ///< Set if the mapped picture is to be unmapped only when calling SpinRender_RenderVideoFrame
  //Implement if partial updates desired
  //int bPicUpdate[8];        ///< Set if the picture in the corresponding slot is to be updated
  /**
  * Array of mapped picture buffers. Up to 8 buffers are currently available for SE_DEVICEMODE_MultiBuffer.
  * For all other modes only the first array element is used.
  */
  Spin_Picture sPic[8];
  long long int llPts;      ///< Presentation time stamp for all mapped buffers. Should be the same as the llPts of each Spin_Picture.
  int iStreamId;            ///< Stream identifier of the picture. Only used in the SE_DEVICEMODE_Alternate for identifying the stream.
} SpinRender_Frame;

typedef enum SE_RenderTargetMode
{
  SE_RTMODE_FrameBuffer,       ///< render to frame buffer and display
  SE_RTMODE_DeviceMemory       ///< render to device memory
} SE_RenderTargetMode;


/**
 * SpinRender configuration struct for initializing the renderer context.
 *
 * @see SpinRenderLib_Open
 */
typedef struct SpinRender_Param
{
  /**
  * Picture format of the to be rendered video frames. The rendering and buffer mapping will base its operation
  * on the specified parameters. The strides of the planes do not need the filled in. In the SE_DEVICEMODE_MultiBuffer
  * all devices have the same sInputFmt.
  */
  Spin_Picture sInputFmt;

  SE_MemoryObjectType eSrcMemType;

  SE_RenderMode eRenderMode;

  SE_RenderTargetMode eRTMode;

  /**
   * The maximum number of rendering buffers that are in the internal rendering queue. A buffer is
   * added to the rendering queue using the SpinRenderLib_RenderVideoFrame call. This variable does
   * not specify the number of buffer that can be mapped. Calls to SpinRenderLib_MapVideoFrame will
   * only block when the number of unrendered pictures has reached the iMaxRenderBuffers.
   */
  int iMaxRenderBuffers;

  /**
   * The number of back buffers used in the display swap chain. This value can be set to 2 for double
   * buffering, and to 3 for triple buffering.
   */
  int iNumBackBuffers;

  SE_DeviceMode eDeviceMode;

  int iNumDevices;                ///< Number of devices to use. Currently only one device is supported.
                                  ///< More than one device allows for frame split rendering.
  /**
   * Used when eDeviceMode is SE_DEVICEMODE_Tiled. Specifies the number of devices in width and height
   * the render target is to be divided over. The asDevs specifies which device to target for each segment,
   * in raster scan order.
   */
  int aiDevRasterConf[2];

  /**
   * Device and output configuration
   */
  struct {
    unsigned char ucDevID;
    unsigned char ucOutID;
    /**
    * The window to initialize the swap chain with.
    */
    SpinRender_WindowDesc sWndDesc;

    /**
    * Number of threads for rendering. Currently only used for SDI devices.
    */
    int iNumThreads;

    /**
    * The bit depth of the render target (back buffer). Possible values are 8- and 10-bit.
    */
    int iBackBufferBitdepth;
    SpinRender_Format sRtFormat;    ///< Output render format

    /*
    * Enable PQ domain tone mapping for HDR to SDR conversion based on ITU-R BT.2390.
    */
    int iToneMapping;

    uint64_t auCPUAffinityMask[16];
  } asDevs[16];

  int iRtWidth;                   ///< Width in pixels of the total render target. When 0 it is automatically selected based on the input resolution.
  int iRtHeight;                  ///< Height in pixels of the total render target. When 0 it is automatically selected based on the input resolution.

  int iRateDen;                   ///< Target framerate denominator
  int iRateNum;                   ///< Target framerate numerator

  float fSDRLightLevelFactor; ///< Light level multiplier for SDR content in HDR render targets

  //Display properties and positioning for immersive rendering
  //Applies to all devices.
  SE_GeometryType eDispGeom;      ///< Geometry typ of display
  int iPixPerTurn;                ///< Curvature of output in pixels per turn (SE_GEOM_Cylinder)
  float fViewRadRatio;            ///< Ratio of viewing position and screen curve radius (SE_GEOM_Cylinder)

  /**
   * Initial delay offset in milliseconds. This delays the rendering by the specified amount.
   */
  double dInitialDelayMs;
  /**
   * Time base to use to convert from PTS to real-time in seconds.
   * The value corresponds to a timestamp increment corresponding to 1 second.
   */
  int iTimeBase;
  SE_RefTimeMeth eRefTimeMode;    ///< Reference time update method.
  int bHighAccuracyFrameTiming;   ///< Use busy waiting to schedule frames at the right time.

  typedef double(*ExtTimeFunc)(void* pHandle);  ///< External ref time callback prototype.
  /**
   * Callback function for retrieving an external ref time. Must return the time in seconds.
   * If the callback function is not set an internal function is used for timing the presentation
   * of video frames. In this case times might diverge with for instance the audio if it uses a
   * different low-level time function.
   */
  ExtTimeFunc fRefTimeSec;
  void* hRefTimeHandle;           ///< Callback handle for fRefTimeSec callback function.

  typedef Spin_360ViewState(*ExtQuery360ViewStateFunc)(void* pHandle);
  ExtQuery360ViewStateFunc fRefQuery360ViewState;   ///< experimental callback for 360 pose
  void* hRefQuery360ViewStateHandle;

  SD_ExtLogFunc fLogFunc;     ///< Callback function for log messages.
  void* hLogHandle;           ///< Callback handle for fLogFunc callback function.

  /**
   * Initial state of viewing overlays. Overlays can be toggled on and of during runtime.
   *
   * @see SpinRender_OverlayElem
   * @see SpinRenderLib_MapOverlayElement
   * @see SpinRenderLib_UnmapOverlayElement
   */
  int iInfoOverlay;
  /**
   * Initial V-Sync mode. V-Sync can be toggled on and off during runtime.
   *
   * @see SE_RCOP_Vsync
   * @see SpinRenderLib_Control
   * @see SpinRender_CtrlOpDesc
   */
  SE_VSyncMode eVsyncMode;

  /**
   * Enable dropping of late frames. Late frames are dropped when the it is detected that the
   * rendering speed is the cause. If the dropping algorithm detects that the frames are late
   * because the application cannot produce frames in time with respect their pts, the renderer
   * will not drop such frames and render and display these late frames anyway.
   */
  int bDropLateFrames;

  /** @name Static HDR metadata
    */
  ///@{
  /** Mastering display color volume information */
  SpinSEI_MasteringDisplayColourVolume seiDisp;
  /** Maximum content light level information */
  SpinSEI_ContentLightLevel seiCLL;
  ///@}
} SpinRender_Param;

/**
 * Supported rendering control operations. Control operation are performed asynchronously.
 *
 * @see SpinRenderLib_Control
 * @see SpinRender_CtrlOpDesc
 */
typedef enum SE_RenderCtrlOp
{
  SE_RCOP_None = -1,
  SE_RCOP_Nop,
  SE_RCOP_Exit,           ///< Used internally in SpinRenderLib_Close. It is not recommended to use directly.
  SE_RCOP_Pause,          ///< Pause or play the rendering.
  /**
   * Explicitly set a new reference time. Should be performed after a non-linear operation when the
   * rendering context is configure to use the SE_REFTIME_Explicit reference time mode.
   *
   * @see eRefTimeMode
   */
  SE_RCOP_SetRefTime,
  SE_RCOP_Resize,         ///< Resize the swapchain.
  SE_RCOP_FullScreen,     ///< Set or unset the fullscreen mode.
  SE_RCOP_ShowInfo,       ///< Toggle the onscreen statistics info.
  SE_RCOP_Vsync,          ///< Toggle the v-sync mode
  SE_RCOP_Redraw,         ///< Redraw the last presented frame.
  SE_RCOP_SetRTFormat,    ///< Set the rendering format.
  SE_RCOP_ToneMap,        ///< Set the tone mapping method
  SE_RCOP_SDRLightLevel,  ///< Set the SDR light level factor
  SE_RCOP_Zoom,           ///< Zoom in/out or change vertical FOV. Only available in SE_DEVICEMODE_Single.
  SE_RCOP_Move,           ///< Move visible area when zoomed in. Only available in SE_DEVICEMODE_Single.
  SE_RCOP_Projection,     ///< Set the output projection.
  SE_NUM_RenderCtrlOps
} SE_RenderCtrlOp;


/**
 * Rendering control operation descriptor. Control operation are performed asynchronously.
 *
 * @see SpinRenderLib_Control
 * @see SE_RenderCtrlOp
 */
typedef struct SpinRender_CtrlOpDesc
{
  SE_RenderCtrlOp eOpType; ///< Control operation type.
  uint64_t uiDevMask;      ///< 64-bit device mask for controlling which devices the resize will apply for.
                           ///< This mask is also used when setting the render format.
                           ///< The LSB corresponds to device 0. If the mask is 0 it is assumed to be 1.
                           ///< Currently, it is only considered in the SE_RCOP_Resize and SE_RCOP_SetRTFormat control operations.
  double dTimeStamp;       ///< Message timestamp to be used for latency calculation.
                           ///< The time in seconds must be calculated according to fRefTimeSec.
                           ///< In the callback function is not set, the SpinLib_GetRealTime is used instead.
                           ///< @see fRefTimeSec
                           ///< @see SpinLib_GetRealTime
  union Data {
    int bPause;            ///< Set to 0 to continue rendering and to 1 to bPause the rendering. @see SE_RCOP_Pause
    double dRefTime;       ///< The new reference time. @see SE_RCOP_SetRefTime
    struct {
      int iWidth;
      int iHeight;
      int bMinimized;
    } resize;             ///< Swapchain resize parameter. @see SE_RCOP_Resize
    int iFullscreen;      ///< Set to 0 to toggle the fullscreen mode, and to 1 to set the window to fullscreen.
                          ///< @see SE_RCOP_FullScreen
    SpinRender_WindowDesc sWndDesc;   ///< New window descriptor. @see SE_RCOP_ChangeWnd
    SpinRender_Format sRTFormat;      ///< Set the render format @see SE_RCOP_SetRTFormat.
    int iToneMap;         ///< Set the tone mapping method 0:off 1:PQ domain tone mapping for HDR to SDR
    float fSDRLightLevelFactor;  ///< Set the SDR light level factor.
    struct {
      int bRestoreDefault; ///< In planar mode returns the zoom to 1x, in 360 projection mode resets the vertical FOV.
      /**
       * In planar mode this changes the zoom factor -> is 1 + fUnits*0.01 x
       * In 360 projection mode this changes the vertical field of view -> fUnits degrees
       */
      float fUnits;
      int iPosx;          ///< Zoom position (relative to window). Ignored when in 360 projection mode and multidevice mode.
      int iPosy;
    } zoom;
    struct {
      SE_GeometryType eRtGeom;  ///< Change render target projection. Supports only planar projection, other values will disable the projection.
    } projection;
    struct {
      int bRelAbs;       ///< If set to 0 the iUnitsX and iUnitY are displacements, when set to 1 they are target coordinates.
      int iUnitX;        ///< Pixels to move in horizontal direction, or absolute position to move the origin to. Relative to total input video resolution.
      int iUnitY;        ///< Pixels to move in vertical direction, or absolute position to move the origin to. Relative to total input video resolution.
    } move;
    int iInfoOverlay;     ///< Set 0 to hide all the info overlays, set 1 to show the first overlay, set 2 to show both overlays
  } data;
} SpinRender_CtrlOpDesc;

/**
 * Overlay element structure . Filled by the SpinRenderLib_MapOverlayElement API call.
 *
 * @see SpinRenderLib_MapOverlayElement
 * @see SpinRenderLib_UnmapOverlayElement
 */
typedef struct SpinRender_OverlayElem
{
  /**
   * When the sPic contains the format and CPU accessible memory for the overlay element.
   * The caller can modify its content after it is mapped with SpinRenderLib_MapOverlayElement.
   * To create a new overlay element, the sPic should be filled to describe the desired format.
   * The pPlane and pPlaneAlloc should be set to NULL, and are filled in.
   */
  Spin_Picture sPic;
  /**
   * Normalized rectangle position the overlay should appear at. The order of the array elements
   * is left, top, right, and bottom coordinates. The top-left corner has position (0,0) and the
   * bottom-right corner has position (1,1).
   */
  float afPos[4];
  int   bHide;            ///< Indicates if the overlay should be hidden.
  float fOpacity;         ///< Normalized opacity of the overlay. 0 is transparent and 1 is solid.
  int   bModified;        ///< Indicates if the content of sPic is modified. If modified the
                          ///< the SpinRenderLib_UnmapOverlayElement will issue an update of the
                          ///< overlay texture data.
} SpinRender_OverlayElem;



typedef void* SpinRenderLib_Handle;

/**
 * Return the available render adapters.
 *
 * @param ppAdapters Pointer to hold the array pointer of SpinRender_Adapter.
 * @param piNumAdapters Pointer to the number of adapters found on the system.
 *
 * @see SpinRender_Adapter
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRenderLib_GetAdapters(const SpinRender_Adapter** ppAdapters, int* piNumAdapters);

/**
 * Open a render context.
 *
 * @param phRender The render context handle is returned in *phRender.
 * @param psParam  Pointer to the SpinRender_Param struct to initialize the renderer.
 *
 * @see SpinRender_Param
 * @see SpinRenderLib_Close
 * @see SpinRenderLib_Reinit
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRenderLib_Open(SpinRenderLib_Handle* phRender, const SpinRender_Param* psParam);

/**
 * @deprecated Reinitialize the render context with a new SpinRender_Param configuration.
 * Swapchain, window and window state remain intact.
 * NOTE! Deprecated, internally falls back to destroy and reopen.
 *
 * @param hRender Render context handle.
 * @param psParam Pointer to the SpinRender_Param struct to reinitialize the renderer.
 *
 * @see SpinRender_Param
 * @see SpinRenderLib_Close
 * @see SpinRenderLib_Open
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
DEPRECATED_API SPINLIB_API int SpinRenderLib_Reinit(SpinRenderLib_Handle hRender, const SpinRender_Param* param);

/**
 * Close the render context.
 *
 * @param phRender The render context handle to close. *phRender is set to NULL after successfully
 * closing the renderer.
 *
 * @see SpinRenderLib_Open
 * @see SpinRenderLib_Reinit
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRenderLib_Close(SpinRenderLib_Handle* phRender);

/**
 * Returns if a video frame buffer can be mapped without blocking.
 *
 * @param hRender The render context handle.
 *
 * @see SpinRenderLib_MapVideoFrame
 * @return >0 if a buffer can be mapped without blocking. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRenderLib_HasFreeVideoFrame(const SpinRenderLib_Handle hRender);

/**
 * Maps a video frame buffer in pFrame. The pFrame struct contains after mapping CPU accessible
 * picture buffer(s). This call blocks until the internal render queue has a free slot, or reaches
 * a 1 second timeout.
 *
 * The Spin_Picture inside a mapped SpinRender_Frame can be reformed to a different format or resolution
 * using SpinLib_ReformFrame in case the iAllocSize field is set.
 * Some rendering devices might not support reforming, in this case iAllocSize is set to 0.
 *
 * @see SpinRenderLib_RenderVideoFrame
 * @see iMaxRenderBuffers
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRenderLib_MapVideoFrame(SpinRenderLib_Handle hRender, SpinRender_Frame* pFrame);

/**
 * The pFrame struct contains after mapping CPU accessible picture buffer(s).
 * This call blocks until the internal render queue has a free slot.
 *
 * The Spin_Picture inside a mapped SpinRender_Frame can be reformed to a different format or resolution
 * using SpinLib_ReformFrame in case the iAllocSize field is set.
 * Some rendering devices might not support reforming, in this case iAllocSize is set to 0.
 *
 * @see SpinRenderLib_RenderVideoFrame
 * @see SpinRenderLib_RenderVideoTexture
 * @see iMaxRenderBuffers
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRenderLib_AllocDeviceFrame(SpinRenderLib_Handle hRender, SpinRender_Frame* pFrame);

/**
 *
 * @see SpinRenderLib_AllocDeviceFrame
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRenderLib_FreeDeviceFrame(SpinRenderLib_Handle hRender, SpinRender_Frame* pFrame);


/**
 * Unmaps the video frame buffer pFrame and adds it to the internal rendering queue. The rendering is
 * scheduled based on the the llPts. The data[0] field in Spin_Picture can be used to communicate the
 * original compressed byte size of the frame to be used in the statistics overlay (D3D12 and Vulkan only).
 *
 * In case the resolution and/or format have changed after the picture has been mapped using SpinRenderLib_MapVideoFrame
 * (picture is reformed using SpinLib_ReformFrame after mapping the buffer), the new resolution and format
 * will be automatically used in the renderer.
 *
 * @see SpinRenderLib_MapVideoFrame
 * @see SpinLib_ReformFrame
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
*/
SPINLIB_API int SpinRenderLib_RenderVideoFrame(SpinRenderLib_Handle hRender, SpinRender_Frame* pFrame, SpinRender_Frame** ppFrameOut);


/**
 * A blocking call that flushes the currently rendered pictures and invalidates all the queued pictures. Also
 * all mapped buffers are unmapped and assumed to be reusable again. After this call the renderer context
 * is "empty". This function can be used to implement abrupt seeking. Depending on the eRefTimeMode mode
 * an explicit SE_RCOP_SetRefTime control operation is required to set the new reference time.
 *
 * @param hRender The renderer context handle.
 *
 * @return Returns SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRenderLib_InvalidateInflightFrames(SpinRenderLib_Handle hRender);

/**
 * A blocking call that flushes the internal render queue. This call returns after all queued pictures
 * have been presented.
 *
 * @param hRender The renderer context handle.
 *
 * @return Returns SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRenderLib_FlushInflightFrames(SpinRenderLib_Handle hRender);

/**
 * A blocking call that recreates the swapchain with a new window.
 *
 * @param hRender The renderer context handle.
 * @param pWndDesc Pointer to window descriptor.
 *
 * @return Returns SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRenderLib_ChangeWindow(SpinRenderLib_Handle hRender, const SpinRender_WindowDesc* pWndDesc);

/**
 * Create and/or map rectangular overlay element. To create a new overlay element the SpinRender_OverlayElem must be
 * filled accordingly. The Spin_Picture inside pOverlayElem describes the size and format of the desired overlay
 * element. All attributes should be filled except the iStride field and the pPlane pointers of the Spin_Plane array in
 * Spin_Picture. This API call returns in this case the iStride field and the pPlane pointers filled in to be used
 * by the caller.
 *
 * To remap an earlier created SpinRender_OverlayElem element that was unmapped before, the SpinRender_OverlayElem
 * with the pPlane pointers filled in. It is important to keep the pPlane pointers, since the render context
 * links the exposed SpinRender_OverlayElem with the internal overlay elements using these pointers.
 *
 * After creation or mapping the planes can be filled and modified as regular CPU memory. The Spin_Picture attributes
 * cannot be modified after initial creation.
 *
 * @param hRender The renderer context handle.
 *
 * @return Returns SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRenderLib_MapOverlayElement(SpinRenderLib_Handle hRender, SpinRender_OverlayElem* pOverlayElem);

/**
 * Unmaps an earlier mapped SpinRender_OverlayElem. After unmapping changes the plane data is not guaranteed to be
 * accessible. Before unmapping, the desired updates overlay texture data, the position, the opacity, and visibility
 * of the overlay, must be made for the rendering context to update overlay element.
 *
 * @param hRender The renderer context handle.
 *
 * @return Returns SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRenderLib_UnmapOverlayElement(SpinRenderLib_Handle hRender, SpinRender_OverlayElem* pOverlayElem);

// Not yet implemented.
//SPINLIB_API int SpinRenderLib_DestroyOverlayElement(const SpinRenderLib_Handle hRender, SpinRender_OverlayElem* pOverlayElem);

/**
 * Issue asynchronous control operation. To achieve high performance, the internal implementation of the video
 * rendering pipeline has asynchronous behavior. To keep the high performance and smooth operation the control
 * operations must also be asynchronous when possible.
 *
 * @param hRender The renderer context handle.
 * @param pOpDesc The rendering control operation.
 *
 * @see SpinRender_CtrlOpDesc
 * @see SE_RenderCtrlOp
 *
 */
SPINLIB_API int SpinRenderLib_Control(SpinRenderLib_Handle hRender, const SpinRender_CtrlOpDesc* pOpDesc);

/**
 * Fills SpinRender_Param* psParam with the default values.
 * Typical usage is to fill the default parameters and override specific variables only.
 * @param psParam Pointer to the to be filled SpinRender_Param struct.
 * @see SpinRender_Param
 * @return Returns SD_OK on success. See spinretcodes.h for other return codes.
 */
SPINLIB_API int SpinRenderLib_GetDefaultParams(SpinRender_Param* psParam);

#ifdef __cplusplus
}
#endif

/** @}*/

