/**
 @code
  ___ _____ _   ___ _  _____ ___  ___  ___ ___
 / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 embedded.connectivity.solutions.==============
 @endcode

 @file
 @copyright  STACKFORCE GmbH, Germany, www.stackforce.de
 @author     STACKFORCE
 @brief      Implementation of the persistent data storage module logic.
*/

/*==============================================================================
                            INCLUDES
==============================================================================*/
/* Standard library. */
#include <stddef.h>
/* SDK includes */
#if !CONTIKI_TARGET_COOJA
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/flash.h)
#include "flash_interface.h"
#else
#include "eeprom.h"
#endif
/* Module specific include. */
#include "sf_persistentDataStorage.h"
#include "contiki-crc16.h"

/*==============================================================================
                          MACROS
==============================================================================*/
/* The size of flash page in bytes */
#define SF_PERSISTENTDATASTORAGE_PAGE_SIZE          0x2000
/* The flash size */
#ifndef SF_PERSISTENTDATASTORAGE_CONF_FLASH_SIZE
#define SF_PERSISTENTDATASTORAGE_FLASH_SIZE         0x58000
#else
#define SF_PERSISTENTDATASTORAGE_FLASH_SIZE         SF_PERSISTENTDATASTORAGE_CONF_FLASH_SIZE
#endif
/* Configuration storage base address.
   The last page of the flash is reserved for the CCFG configurations.
   The device configurations are stored in one page before the last */
#define SF_PERSISTENTDATASTORAGE_CONFIG_BASEADDR    SF_PERSISTENTDATASTORAGE_FLASH_SIZE -\
                                                    2 * SF_PERSISTENTDATASTORAGE_PAGE_SIZE
/* Device configuration flag */
#define SF_PERSISTENTDATASTORAGE_CONFIG_FLAG        0xB0

/*==============================================================================
                          GLOBAL PARAMS
==============================================================================*/
/* Device life time life time configuration address. */
static const uint32_t gDeviceConfigAddress = (uint32_t)SF_PERSISTENTDATASTORAGE_CONFIG_BASEADDR;

/*==============================================================================
                      LOCAL FUNCTION
==============================================================================*/
/*----------------------------------------------------------------------------*/
/*! loc_writeFlash */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t loc_writeFlash(uint32_t address, uint8_t *pData, uint32_t datalen)
{
  /* Return value. */
  E_SF_RETURN_t writeStatus = E_SF_ERROR;

#if CONTIKI_TARGET_COOJA
  eeprom_write((eeprom_addr_t)0, (unsigned char*)pData, datalen);

  writeStatus = E_SF_SUCCESS;
#else
  /* Write into the flash. */
  if(FLASH_SUCCESS == writeFlash(address, pData, datalen))
  {
    writeStatus = E_SF_SUCCESS;
  }
#endif

  return writeStatus;
}/* loc_writeFlash */

/*----------------------------------------------------------------------------*/
/*! loc_readFlash */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t loc_readFlash(uint32_t address, uint8_t *pData, uint32_t datalen)
{
  /* Return value. */
  E_SF_RETURN_t readStatus = E_SF_ERROR;

#if CONTIKI_TARGET_COOJA
  eeprom_read((eeprom_addr_t)0, (unsigned char*)pData, datalen);

  readStatus = E_SF_SUCCESS;
#else
  /* Read read from flash. */
  if(FLASH_SUCCESS == readFlash(address, pData, datalen))
  {
    readStatus = E_SF_SUCCESS;
  }
#endif

  return readStatus;
}/* loc_readFlash */

/*==============================================================================
                      API FUNCTION IMPLEMENTATION
==============================================================================*/
/*----------------------------------------------------------------------------*/
/*! sf_persistentDataStorage_writeConfig */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_persistentDataStorage_writeConfig(sf_persistent_deviceConfig_t*
                                                   pPersistentDeviceConfig)
{
  /* Return value. */
  E_SF_RETURN_t writeStatus = E_SF_ERROR;

  if(NULL == pPersistentDeviceConfig)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  if(E_SF_SUCCESS == sf_persistentDataStorage_removeConfig())
  {
    pPersistentDeviceConfig->flag = SF_PERSISTENTDATASTORAGE_CONFIG_FLAG;

    /* Compute CRC */
    pPersistentDeviceConfig->crc = crc16_data((uint8_t*)pPersistentDeviceConfig,
                                            sizeof(sf_persistent_deviceConfig_t) -
                                            sizeof(uint16_t), 0);

    /* Write to NVM */
    writeStatus = loc_writeFlash(gDeviceConfigAddress,
                                  (uint8_t*)pPersistentDeviceConfig,
                                  sizeof(sf_persistent_deviceConfig_t));
  }

  return writeStatus;
}/* sf_persistentDataStorage_writeConfig() */

/*----------------------------------------------------------------------------*/
/*! sf_persistentDataStorage_readConfig */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_persistentDataStorage_readConfig(sf_persistent_deviceConfig_t*
                                                  pPersistentDeviceConfig)
{
  /* Return value. */
  E_SF_RETURN_t readStatus = E_SF_ERROR;
  /* Computed crc*/
  uint16_t crc;

  if(!pPersistentDeviceConfig)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  /* Read from NVM */
  readStatus = loc_readFlash(gDeviceConfigAddress,
                             (uint8_t*)pPersistentDeviceConfig,
                              sizeof(sf_persistent_deviceConfig_t));

  /* Check CRC */
  crc = crc16_data((uint8_t*)pPersistentDeviceConfig,
                   sizeof(sf_persistent_deviceConfig_t) -
                   sizeof(uint16_t), 0);

  if((pPersistentDeviceConfig->crc != crc) ||
     (SF_PERSISTENTDATASTORAGE_CONFIG_FLAG != pPersistentDeviceConfig->flag))
  {
    readStatus = E_SF_ERROR;
  }

  return readStatus;
}/* sf_persistentDataStorage_readConfig() */

/*----------------------------------------------------------------------------*/
/*! sf_persistentDataStorage_removeConfig */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_persistentDataStorage_removeConfig(void)
{
  E_SF_RETURN_t  eraseStatus = E_SF_ERROR;
#if !CONTIKI_TARGET_COOJA
  uint8_t page = gDeviceConfigAddress / SF_PERSISTENTDATASTORAGE_PAGE_SIZE;

  if(FLASH_SUCCESS == eraseFlashPg(page))
  {
    eraseStatus = E_SF_SUCCESS;
  }
#else
  eraseStatus = E_SF_SUCCESS;
#endif

  return eraseStatus;
}/* sf_persistentDataStorage_removeConfig() */

