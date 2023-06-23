/**
 * @code
 *  ___ _____ _   ___ _  _____ ___  ___  ___ ___
 * / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 * \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 * |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 * embedded.connectivity.solutions.==============
 * @endcode
 *
 * @copyright  STACKFORCE GmbH, Germany, www.stackforce.de
 * @author     STACKFORCE
 * @brief      API implementation of the config management.
 *
 */

/*****************************************************************************/
/*                         INCLUDES                                          */
/*****************************************************************************/
/* Standard include */
#include <stdio.h>
#include <string.h>
/* Stack includes */
#if !CONTIKI_TARGET_COOJA
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_fcfg1.h)
#include DeviceFamily_constructPath(inc/hw_ccfg.h)
#include "ieee-addr.h"
#else
#include <dev/moteid.h>
#endif
/* Module specific include */
#include "sf_persistentDataStorage.h"
#include "sf_configMgmt.h"
#include "sf_types.h"

/*****************************************************************************/
/*                         MACROS                                            */
/*****************************************************************************/
/* Default measurement interval in seconds */
#ifndef SF_CONF_MEAS_INTERVAL_DEFAULT
#define SF_CONFIGMGMT_MEAS_INTERVAL_DEFAULT        3
#else
#define SF_CONFIGMGMT_MEAS_INTERVAL_DEFAULT        SF_CONF_MEAS_INTERVAL_DEFAULT
#endif
/* IEEE 802.15.4 MAC address. This is a unique entity provided by the vendor.
   We will use it to configure the default device serial number. */
#define SF_CONFIGMGMT_MAC_PRIMARY_ADDRESS          (FCFG1_BASE + FCFG1_O_MAC_15_4_0)

/*****************************************************************************/
/*                         GLOBAL VARIABLES                                  */
/*****************************************************************************/
/* SC configuration parameters */
sf_deviceConfig_t gDeviceConfig = {0};
/* Stores device status */
static SF_GONFIGMGMT_DEVICESTATUS_t gDeviceStatus = E_CONFIGMGMT_DEVICESTATUS_NOT_CONNECTED;

/*****************************************************************************/
/*                         API FUNCTIONS                                     */
/*****************************************************************************/
/*------------------------------------------------------------------------------
  sf_configMgmt_setParam()
------------------------------------------------------------------------------*/
E_SF_RETURN_t sf_configMgmt_setParam(const void* pParamData, size_t paramSize,
                                     SF_CONFIGMGMT_PARAM_t paramType)
{
  E_SF_RETURN_t returnValue = E_SF_ERROR_INVALID_PARAM;

  if(NULL == pParamData)
  {
    return returnValue;
  }

  switch (paramType)
  {
    case E_CONFIGMGMT_PARAM_DEVICE_SERIAL:
      if(paramSize == sizeof(gDeviceConfig.deviceSerial))
      {
        memcpy(&gDeviceConfig.deviceSerial, pParamData, paramSize);
        returnValue = E_SF_SUCCESS;
      }
      break;

    case E_CONFIGMGMT_PARAM_MEAS_INTERVAL:
      if(paramSize == sizeof(gDeviceConfig.measIntervalSec) &&
         SF_CONFIGMGMT_MEAS_INTERVAL_LIMIT_LOWER <= *((uint32_t*)pParamData))
      {
        memcpy(&gDeviceConfig.measIntervalSec, pParamData, paramSize);
        returnValue = E_SF_SUCCESS;
      }
      break;

    case E_CONFIGMGMT_PARAM_DEVICE_ADDR:
      if(paramSize == sizeof(gDeviceConfig.deviceAddr))
      {
        memcpy(&gDeviceConfig.deviceAddr, pParamData, paramSize);
        returnValue = E_SF_SUCCESS;
      }
      break;

    case E_CONFIGMGMT_PARAM_GW_ADDR:
      if(paramSize == sizeof(gDeviceConfig.bmsccAddr))
      {
        memcpy(&gDeviceConfig.bmsccAddr, pParamData, paramSize);
        returnValue = E_SF_SUCCESS;
      }
      break;

    case E_CONFIGMGMT_PARAM_PAN_ID:
      if(paramSize == sizeof(gDeviceConfig.panId))
      {
        memcpy(&gDeviceConfig.panId, pParamData, paramSize);
        returnValue = E_SF_SUCCESS;
      }
      break;

    default:
      break;
  }

  return returnValue;
}/* sf_configMgmt_setParam() */


/*------------------------------------------------------------------------------
  sf_configMgmt_getDeviceSerial()
------------------------------------------------------------------------------*/
E_SF_RETURN_t sf_configMgmt_getParam(void* pParamData, size_t paramSize,
                                     SF_CONFIGMGMT_PARAM_t paramType)
{
  E_SF_RETURN_t returnValue = E_SF_ERROR_INVALID_PARAM;

  if(NULL == pParamData)
  {
    return returnValue;
  }

  switch (paramType)
  {
    case E_CONFIGMGMT_PARAM_DEVICE_SERIAL:
      if(paramSize == sizeof(gDeviceConfig.deviceSerial))
      {
        memcpy(pParamData, &gDeviceConfig.deviceSerial, paramSize);
        returnValue = E_SF_SUCCESS;
      }
      break;

    case E_CONFIGMGMT_PARAM_MEAS_INTERVAL:
      if(paramSize == sizeof(gDeviceConfig.measIntervalSec))
      {
        memcpy(pParamData, &gDeviceConfig.measIntervalSec, paramSize);
        returnValue = E_SF_SUCCESS;
      }
      break;

    case E_CONFIGMGMT_PARAM_DEVICE_ADDR:
      if(paramSize == sizeof(gDeviceConfig.deviceAddr))
      {
        memcpy(pParamData, gDeviceConfig.deviceAddr.u8, paramSize);
        returnValue = E_SF_SUCCESS;
      }
      break;

    case E_CONFIGMGMT_PARAM_GW_ADDR:
      if(paramSize == sizeof(gDeviceConfig.bmsccAddr))
      {
        memcpy(pParamData, &gDeviceConfig.bmsccAddr, paramSize);
        returnValue = E_SF_SUCCESS;
      }
      break;

    case E_CONFIGMGMT_PARAM_PAN_ID:
      if(paramSize == sizeof(gDeviceConfig.panId))
      {
        memcpy(pParamData, &gDeviceConfig.panId, paramSize);
        returnValue = E_SF_SUCCESS;
      }
      break;

    default:
      break;
  }

  return returnValue;
}

/*------------------------------------------------------------------------------
  sf_configMgmt_setDefaultConfig()
------------------------------------------------------------------------------*/
E_SF_RETURN_t sf_configMgmt_setDefaultConfig(void)
{
  /* Return value */
  E_SF_RETURN_t returnVal = E_SF_ERROR;
  /* Measurement interval */
  uint32_t paramMeasInterval = SF_CONFIGMGMT_MEAS_INTERVAL_DEFAULT;
  /* Serial number */
  uint32_t serialNr = 0;
#if !CONTIKI_TARGET_COOJA
  /* Pointer to IEEE MAC address */
  uint8_t *pMacAddr = NULL;
#endif
  /* Short address */
  linkaddr_t shortAddr = linkaddr_null;

  returnVal = sf_configMgmt_setParam(&paramMeasInterval,
                                     sizeof(paramMeasInterval),
                                     E_CONFIGMGMT_PARAM_MEAS_INTERVAL);

  if(E_SF_SUCCESS == returnVal)
  {
  #if !CONTIKI_TARGET_COOJA
    /* The device serial number is the device's fingerprint therefore it
      should be unique, otherwise it will be a duplicated node in the network.
      As a default value, we use the IEEE MAC address. */
    pMacAddr = (uint8_t*)SF_CONFIGMGMT_MAC_PRIMARY_ADDRESS;
    memcpy(&serialNr, pMacAddr, sizeof(serialNr));
#else
    serialNr = (uint32_t)simMoteID;
#endif
    returnVal = sf_configMgmt_setParam(&serialNr, sizeof(serialNr),
                                       E_CONFIGMGMT_PARAM_DEVICE_SERIAL);
  }

  if(E_SF_SUCCESS == returnVal)
  {
#if !CONTIKI_TARGET_COOJA
    /* Set MAC addr as node addr. */
    ieee_addr_cpy_to(shortAddr.u8, LINKADDR_SIZE);
#else
    shortAddr.u16 = (uint16_t)simMoteID;
#endif
    returnVal = sf_configMgmt_setParam(shortAddr.u8,
                                       LINKADDR_SIZE,
                                       E_CONFIGMGMT_PARAM_DEVICE_ADDR);
  }

  if(E_SF_SUCCESS == returnVal)
  {
    /* Stores the default config into the flash */
    returnVal = sf_configMgmt_writeConfig();
  }

  return returnVal;
} /* sf_configMgmt_setDefaultConfig() */

/*------------------------------------------------------------------------------
  sf_configMgmt_writeConfig()
------------------------------------------------------------------------------*/
E_SF_RETURN_t sf_configMgmt_writeConfig(void)
{
  E_SF_RETURN_t writeStatus = E_SF_ERROR;
  /* Buffer to store the configuration to write into the flash. */
  sf_persistent_deviceConfig_t persistentDeviceConfig = {0};

  persistentDeviceConfig.measIntervalSec = gDeviceConfig.measIntervalSec;
  persistentDeviceConfig.deviceSerial = gDeviceConfig.deviceSerial;
  memcpy(persistentDeviceConfig.bmsccAddr, gDeviceConfig.bmsccAddr.u8,
        sizeof(persistentDeviceConfig.bmsccAddr));
  persistentDeviceConfig.panId = gDeviceConfig.panId;
  memcpy(persistentDeviceConfig.deviceAddr, gDeviceConfig.deviceAddr.u8,
        sizeof(persistentDeviceConfig.deviceAddr));

  writeStatus = sf_persistentDataStorage_writeConfig(&persistentDeviceConfig);

  return writeStatus;
} /* sf_configMgmt_writeConfig() */

/*------------------------------------------------------------------------------
  sf_configMgmt_readConfig()
------------------------------------------------------------------------------*/
E_SF_RETURN_t sf_configMgmt_readConfig(void)
{
  E_SF_RETURN_t readStatus = E_SF_ERROR;
  /* Buffer to store the data device lifetime configuration read from
     the flash. */
  sf_persistent_deviceConfig_t persistentDeviceConfig = {0};

  readStatus = sf_persistentDataStorage_readConfig(&persistentDeviceConfig);
  if(E_SF_SUCCESS == readStatus)
  {
    gDeviceConfig.measIntervalSec = persistentDeviceConfig.measIntervalSec;
    gDeviceConfig.deviceSerial = persistentDeviceConfig.deviceSerial;
    memcpy(gDeviceConfig.bmsccAddr.u8, persistentDeviceConfig.bmsccAddr,
            sizeof(persistentDeviceConfig.bmsccAddr));
    gDeviceConfig.panId = persistentDeviceConfig.panId;
    memcpy(gDeviceConfig.deviceAddr.u8, persistentDeviceConfig.deviceAddr,
            sizeof(persistentDeviceConfig.deviceAddr));
  }

  return readStatus;
} /* sf_configMgmt_readConfig() */

/*------------------------------------------------------------------------------
  sf_configMgmt_setDeviceStatus()
------------------------------------------------------------------------------*/
void sf_configMgmt_setDeviceStatus(SF_GONFIGMGMT_DEVICESTATUS_t deviceStatus)
{
  gDeviceStatus = deviceStatus;
} /* sf_configMgmt_setDeviceStatus() */

/*------------------------------------------------------------------------------
  sf_configMgmt_getDeviceStatus()
------------------------------------------------------------------------------*/
SF_GONFIGMGMT_DEVICESTATUS_t sf_configMgmt_getDeviceStatus(void)
{
  return gDeviceStatus;
} /* sf_configMgmt_getDeviceStatus() */
