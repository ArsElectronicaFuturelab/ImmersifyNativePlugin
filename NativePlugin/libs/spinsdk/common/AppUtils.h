/**
 * \file    AppUtils.h
 * \brief   Spin Digital utility functions
 * \author  Haifeng Gao <haifeng@spin-digital.com>
 * \date    4/24/2020

 * Copyright/Rights of Use (Nutzungsrechte):
 * 2020, Spin Digital Video Technologies GmbH
 * Berlin, Germany
 */

#pragma once

#include "StringUtils.h"
#include "spincommon.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif


static const std::vector<std::string> vstrSubsystems = { "local_lan", "local", "lan" };
static const std::vector<std::string> vstrAccessModes = { "stationshare", "userlimit" };

static int readLicenseConfig(const char* pConfigPath, Spin_LicenseConfig& rLicenseConfig) {

  memset(&rLicenseConfig, 0, sizeof(rLicenseConfig));

  std::ifstream infile(pConfigPath);
  if (!infile.good()) {
    printf("Error: License configuration file not found: %s\n", pConfigPath);
    return SD_E_INVALID_SOURCE;
  }

  std::string sLine;
  int rc = SD_OK;
  while (std::getline(infile, sLine))
  {
    sLine.erase(std::remove_if(sLine.begin(), sLine.end(), ::isspace), sLine.end());
    if (sLine.empty()) {
      continue;
    }
    if (sLine.at(0) == '#') { //# comment
      continue;
    }

    std::vector<std::string> vargs = split(sLine, '=');
    if (vargs.size() == 1) { //only has parameter name, or parameter value is empty
      continue;
    }

    if (vargs.size() == 2) {

      std::string strArgValue = trim(vargs[1], "\""); //parameter value can be surranded by quotation mark ""
      if (strArgValue.empty()) {//if parameter value is empty ""
        continue;
      }

      if (vargs[0] == "subsystem") {
        auto it = std::find(vstrSubsystems.begin(), vstrSubsystems.end(), strArgValue);
        if (it != vstrSubsystems.end()) {
          rLicenseConfig.eSubsystem = (SE_LicenseSubsystem)(it - vstrSubsystems.begin());
        } else {
          printf("Error: license config parsing - the parameter \"%s\" has an invalid value: \"%s\" \n", vargs[0].c_str(), vargs[1].c_str());
          rc = SD_E_LICENSE_SYNTAX_UNEXPECTED;
          break;
        }
        continue;
      }

      if (vargs[0] == "server") {
        if (strArgValue.size() > sizeof(rLicenseConfig.acServer) - 1 ) {
          printf("Error: license config parsing - the parameter \"%s\" has a too long value: %s\n", vargs[0].c_str(), vargs[1].c_str());
          rc = SD_E_LICENSE_SYNTAX_UNEXPECTED;
          break;
        } else {
          strncpy(rLicenseConfig.acServer, strArgValue.c_str(), sizeof(rLicenseConfig.acServer) - 1);
          rLicenseConfig.acServer[sizeof(rLicenseConfig.acServer) - 1] = '\0';
        }
        continue;
      }

      if (vargs[0] == "licfile") {
        if (strArgValue.size() > sizeof(rLicenseConfig.acRMSLicFilePath) - 1) {
          printf("Error: license config parsing - the parameter \"%s\" has a too long value: %s\n", vargs[0].c_str(), vargs[1].c_str());
          rc = SD_E_LICENSE_SYNTAX_UNEXPECTED;
          break;
        } else {
          strncpy(rLicenseConfig.acRMSLicFilePath, strArgValue.c_str(), sizeof(rLicenseConfig.acRMSLicFilePath) - 1 );
          rLicenseConfig.acRMSLicFilePath[sizeof(rLicenseConfig.acRMSLicFilePath) - 1] = '\0';
        }
        continue;
      }

      if (vargs[0] == "container_id") {
        std::vector<std::string> tokens = split(strArgValue, '-');
        if (tokens.size() == 2 && is_digits(tokens[0]) && is_digits(tokens[1])){
          std::istringstream(tokens[0]) >> rLicenseConfig.sCMContainerID.usCMBoxMask;
          std::istringstream(tokens[1]) >> rLicenseConfig.sCMContainerID.ulCMSerialNumber;
        } else {
          printf("Error: license config parsing - the parameter \"%s\" has an invalid value: \"%s\" \n", vargs[0].c_str(), vargs[1].c_str());
          rc = SD_E_LICENSE_SYNTAX_UNEXPECTED;
          break;
        }
        continue;
      }

      if (vargs[0] == "access_mode") {
        auto it = std::find(vstrAccessModes.begin(), vstrAccessModes.end(), strArgValue);
        if (it != vstrAccessModes.end()) {
          rLicenseConfig.eCMAccessMode = (SE_LicenseAccessMode)(it - vstrAccessModes.begin());
        } else {
          printf("Error: license config parsing - the parameter \"%s\" has an invalid value: \"%s\" \n", vargs[0].c_str(), vargs[1].c_str());
          rc = SD_E_LICENSE_SYNTAX_UNEXPECTED;
          break;
        }
        continue;
      }

      if (vargs[0] == "product") {
        if (strArgValue.size() > sizeof(rLicenseConfig.acProduct) - 1) {
          printf("Error: license config parsing - the parameter \"%s\" has a too long value: %s\n", vargs[0].c_str(), vargs[1].c_str());
          rc = SD_E_LICENSE_SYNTAX_UNEXPECTED;
          break;
        } else {
          strncpy(rLicenseConfig.acProduct, strArgValue.c_str(), sizeof(rLicenseConfig.acProduct) - 1);
          rLicenseConfig.acProduct[sizeof(rLicenseConfig.acProduct) - 1] = '\0';
        }
        continue;
      }

      if (vargs[0] == "customer") {
        if (strArgValue.size() > sizeof(rLicenseConfig.acCustomer) - 1) {
          printf("Error: license config parsing - the parameter \"%s\" has a too long value: %s\n", vargs[0].c_str(), vargs[1].c_str());
          rc = SD_E_LICENSE_SYNTAX_UNEXPECTED;
          break;
        } else {
          strncpy(rLicenseConfig.acCustomer, strArgValue.c_str(), sizeof(rLicenseConfig.acCustomer) - 1);
          rLicenseConfig.acCustomer[sizeof(rLicenseConfig.acCustomer) - 1] = '\0';
        }
        continue;
      }

      if (vargs[0] == "user") {
        if (strArgValue.size() > sizeof(rLicenseConfig.acUser) - 1) {
          printf("Error: license config parsing - the parameter \"%s\" has a too long value: %s\n", vargs[0].c_str(), vargs[1].c_str());
          rc = SD_E_LICENSE_SYNTAX_UNEXPECTED;
          break;
        } else {
          strncpy(rLicenseConfig.acUser, strArgValue.c_str(), sizeof(rLicenseConfig.acUser) - 1);
          rLicenseConfig.acUser[sizeof(rLicenseConfig.acUser) - 1] = '\0';
        }
        continue;
      }

    }
  }

  infile.close();
  return rc;
}


static int readLicenseConfig(Spin_LicenseConfig& rLicenseConfig) {
  memset(&rLicenseConfig, 0, sizeof(rLicenseConfig));
  std::string sConfigFilePath = "";
#ifdef _WIN32
  char path[MAX_PATH];
  size_t length = GetModuleFileNameA(NULL, path, MAX_PATH);
  if (length > 0) {
    sConfigFilePath = path;
    sConfigFilePath = sConfigFilePath.substr(0, sConfigFilePath.find_last_of("\\") + 1);
  }
#else
  char path[PATH_MAX];
  size_t length = readlink("/proc/self/exe", path, PATH_MAX);
  if (length > 0) {
    path[length] = '\0';
    sConfigFilePath = path;
    sConfigFilePath = sConfigFilePath.substr(0, sConfigFilePath.find_last_of("/") + 1);
  }
#endif
  sConfigFilePath += "licenseconfig.txt";
  return readLicenseConfig(sConfigFilePath.c_str(), rLicenseConfig);

}

