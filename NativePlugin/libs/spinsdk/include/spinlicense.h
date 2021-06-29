/**
 * \file    spinlicense.h
 * \brief   Spin Digital license
 * \author  Haifeng Gao <haifeng@spin-digital.com>
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2020, Spin Digital Video Technologies GmbH
 * Berlin, Germany
 */

#pragma once

/** \defgroup common SpinCommon
*  \brief Common API included in all Spin Digital libraries.
*  @{
*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * License subsystem. Specify the subsystems where the licenses can be located.
 *
 * @see SpinLib_EnumerateLicenses
 */
typedef enum SE_LicenseSubsystem
{
  SE_LSS_Local_LAN = 0,             ///< Access to both local and LAN licenses
  SE_LSS_Local,                     ///< Local licenses only
  SE_LSS_LAN,                       ///< Network licenses only
  SE_NUM_LicSubsystems
} SE_LicenseSubsystem;

/**
 * CodeMeter license access mode. Only effective in the CodeMeter license.
 *
 * @see Spin_LicenseConfig
 * @see SpinLib_EnumerateLicenses
 * @see SpinLib_InitLicense
 */
typedef enum SE_LicenseAccessMode
{
  SE_LAM_StationShare = 0,      ///< Only one license per host system is allocated, independent of the number of intances.
  SE_LAM_UserLimit,             ///< For each started instance a license is allocated.
  SE_NUM_LicAccessModes
} SE_LicenseAccessMode;


/**
 * The CodeMeter license container (use dongle or ActLicense) identifier, which is constructed by two integer numbers that
 * are connected by a hyphen '-', for example, 3-3897990. The first number is the box mask, and the second number represents the serial number.
 *
 * @see Spin_LicenseConfig
 * @see SpinLib_InitLicense
 */
typedef struct Spin_LicenseContainerID
{
  uint16_t usCMBoxMask;          ///< Box mask of the CodeMeter container
  uint64_t ulCMSerialNumber;     ///< Searial number of the CodeMeter Container
} Spin_LicenseContainerID;


/**
 * The license configuration.
 *
 * @see SpinLib_InitLicense
 */
typedef struct Spin_LicenseConfig
{
  SE_LicenseSubsystem eSubsystem;                       ///< Used for the CodeMeter license and the Sentinel RMS license.
  char acServer[129];                                   ///< The hostname of the license server for CodeMeter and Sentinel RMS license,
                                                        ///< or the URL of license server for the SCL license.
  char acRMSLicFilePath[260];                           ///< Sentinel RMS local license file name or path. The default license filename is lservrc.
  Spin_LicenseContainerID sCMContainerID;               ///< ID of the CodeMeter license container. Only used for CodeMeter licenses.
  SE_LicenseAccessMode eCMAccessMode;                   ///< Access mode of the CodeMeter license. Only used for CodeMeter licenses.
  char acCustomer[101];                                 ///< Customer ID. Only used for Sentinel SCL licenses.
  char acUser[257];                                     ///< Named user, e.g. for user-specific entitlements. Only used for Sentinel SCL licenses.
  char acProduct[6];                                    ///< Product prefix. Effective for the Sentinel RMS and SCL licenses.
} Spin_LicenseConfig;

/**
 * SpinLib_InitLicense initializes the license specified in the Spin_LicenseConfig.
 * If the null pointer is used for initialization, the function performs an automatic license
 * search in the local system and the local network environment, and uses the first license
 * found for license initialization.
 * If the license is not initialized successfully, the subsequent SpinSDK API calls that perform license checking will fail.
 *
 * Only one license can be active at a given time in a single process.
 * Subsequent calls with the Spin_LicenseConfig will return SD_OK and register internally a new reference to the active license.
 * Calls with different Spin_LicenseConfigs will return error code SD_E_LICENSE_ALREADY_INITIALIZED.
 *
 * @param psLicConfig    Pointer to the license configuration. A NULL pointer will trigger a license search.
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 *
 * @see Spin_LicenseConfig
 */
SPINLIB_API int SpinLib_InitLicense(const Spin_LicenseConfig* psLicConfig);


/**
 * SpinLib_DeInitLicense releases a reference to the active license.
 * When no more references are remaining the license is deactivated and released.
 * 
 * If there is no active license, it returns SD_E_LICENSE_NOT_INITIALIZED. 
 * If the license is still referenced by other instances, it returns SD_E_LICENSE_STILL_IN_USE. 
 *
 * @return SD_OK on success. See spinretcodes.h for other return codes.
 *
 */
SPINLIB_API int SpinLib_DeInitLicense();


#ifdef __cplusplus
}
#endif

/**@}*/