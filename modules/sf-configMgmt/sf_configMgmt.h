#ifdef __cplusplus
  extern "C" {
#endif

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
 * @brief      API definition of the configuration management.
 *
 */

#ifndef SF_CONFIGMGMT_H_
#define SF_CONFIGMGMT_H_

/**
 *  @addtogroup SF_CONFIG_MGMT
 *
 *  @details
 *
 *  - <b>CONFIG MGMT API</b>\n
 *    | API Function                              | Description                                     |
 *    |-------------------------------------------|-------------------------------------------------|
 *    | @ref sf_configMgmt_setParam()             | @copybrief sf_configMgmt_setParam()             |
 *    | @ref sf_configMgmt_getParam()             | @copybrief sf_configMgmt_getParam()             |
 *    | @ref sf_configMgmt_setDefaultConfig()     | @copybrief sf_configMgmt_setDefaultConfig()     |
 *    | @ref sf_configMgmt_writeConfig()          | @copybrief sf_configMgmt_writeConfig()          |
 *    | @ref sf_configMgmt_readConfig()           | @copybrief sf_configMgmt_readConfig()           |
 *    | @ref sf_configMgmt_setDeviceStatus()      | @copybrief sf_configMgmt_setDeviceStatus()      |
 *    | @ref sf_configMgmt_getDeviceStatus()      | @copybrief sf_configMgmt_getDeviceStatus()      |
 *  @{
 */

/*****************************************************************************/
/*                         INCLUDES                                          */
/*****************************************************************************/
/* Standard include */
#include <stddef.h>
/* Device specific includes */
#include "sf_types.h"
#include "linkaddr.h"

/*****************************************************************************/
/*                         MACROS                                            */
/*****************************************************************************/
/*! Measurement interval lower limit.
   The highest measurement rate i.e. a new measurement at every Tx cycle.
   As it is impossible to set a timeout of fraction of seconds using CONTIKI
   event timer, we define the lower limit in seconds. */
#define SF_CONFIGMGMT_MEAS_INTERVAL_LIMIT_LOWER    (3U)

/*****************************************************************************/
/*                         STRUCTS                                           */
/*****************************************************************************/
/*! Device binding configurations. */
typedef struct
{
  /*! Serial number of the device */
  uint32_t deviceSerial;
  /*! Link address of the SC. */
  linkaddr_t deviceAddr;
  /*! Measurement interval in seconds.
      The minimum value should be equal to
      the cycle length e.g. for schedule A
      min measInterval = 3s */
  uint32_t measIntervalSec;
  /*! Link address of the BMSCC. */
  linkaddr_t bmsccAddr;
  /*! Network ID. */
  uint16_t panId;
} sf_deviceConfig_t;

/*****************************************************************************/
/*                         ENUMS                                             */
/*****************************************************************************/
/*! Device to BMS-CC connection status */
typedef enum
{
  /*! Device disconnected from the network */
  E_CONFIGMGMT_DEVICESTATUS_NOT_CONNECTED,
  /*! Device connected to the network */
  E_CONFIGMGMT_DEVICESTATUS_CONNECTED
} SF_GONFIGMGMT_DEVICESTATUS_t;

/*! Binding configurations identifiers */
typedef enum
{
  /*! Device serial number */
  E_CONFIGMGMT_PARAM_DEVICE_SERIAL,
  /*! Measurement interval */
  E_CONFIGMGMT_PARAM_MEAS_INTERVAL,
  /*! SC short address */
  E_CONFIGMGMT_PARAM_DEVICE_ADDR,
  /*! BMSSCC short address */
  E_CONFIGMGMT_PARAM_GW_ADDR,
  /*! Network ID */
  E_CONFIGMGMT_PARAM_PAN_ID,
} SF_CONFIGMGMT_PARAM_t;

/*****************************************************************************/
/*                         API FUNCTIONS                                     */
/*****************************************************************************/
/*============================================================================*/
/**
 * \brief Set the configuration parameter.
 *
 * \param pParamData       Pointer to the parameter value.
 * \param paramSize        Size of the parameter value.
 * \param paramType        The type of the parameter.
 *
 * \return  @ref E_SF_RETURN_t
 */
/*============================================================================*/
E_SF_RETURN_t sf_configMgmt_setParam(const void* pParamData, size_t paramSize,
                                     SF_CONFIGMGMT_PARAM_t paramType);

/*============================================================================*/
/**
 * \brief Get the configuration parameter.
 *
 * \param pParamData       Pointer to the parameter storage.
 * \param paramSize        Size of the parameter value.
 * \param paramType        The type of the parameter.
 *
 * \return  @ref E_SF_RETURN_t
 */
/*============================================================================*/
E_SF_RETURN_t sf_configMgmt_getParam(void* pParamData, size_t paramSize,
                                     SF_CONFIGMGMT_PARAM_t paramType);

/*============================================================================*/
/**
 * \brief Set the configurations to the default values. The assigned values will
 *        be stored persistently into the internal flash.
 *
 * \return  @ref E_SF_RETURN_t
 */
/*============================================================================*/
E_SF_RETURN_t sf_configMgmt_setDefaultConfig(void);

/*============================================================================*/
/**
 * \brief Write device configuration into the NVM. This function needs to be
 *        called every time @ref sf_configMgmt_setParam is executed to store
 *        the new parameter value into the flash.
 *
 * \return  @ref E_SF_RETURN_t
 */
/*============================================================================*/
E_SF_RETURN_t sf_configMgmt_writeConfig(void);

/*============================================================================*/
/**
 * \brief Read stored device configuration out of the NVM.
 *
 * \return  @ref E_SF_RETURN_t
 */
/*============================================================================*/
E_SF_RETURN_t sf_configMgmt_readConfig(void);

/*============================================================================*/
/**
 * \brief Set the device status.
 *
 * \param deviceStatus     @ref SF_GONFIGMGMT_DEVICESTATUS_t;
 */
/*============================================================================*/
void sf_configMgmt_setDeviceStatus(SF_GONFIGMGMT_DEVICESTATUS_t deviceStatus);

/*============================================================================*/
/**
 * \brief Get the device status.
 *
 * \return  @ref SF_GONFIGMGMT_DEVICESTATUS_t
 */
/*============================================================================*/
SF_GONFIGMGMT_DEVICESTATUS_t sf_configMgmt_getDeviceStatus(void);

/*! @} */

#endif /* SF_CONFIGMGMT_H_ */

#ifdef __cplusplus
}
#endif
