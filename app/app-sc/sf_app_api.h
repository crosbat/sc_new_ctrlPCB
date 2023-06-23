#ifdef __cplusplus
extern "C" {
#endif

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
 @brief      This header contains the application interfaces.
*/

#ifndef __SF_APP_API_H__
#define __SF_APP_API_H__

/**
 *  @addtogroup SF_APP_API
 *
 *  @details
 *
 *  - <b>SC APP API</b>\n
 *    | API Function                              | Description                                     |
 *    |-------------------------------------------|-------------------------------------------------|
 *    | @ref sf_app_handleDownlink()              | @copybrief sf_app_handleDownlink()              |
 *    | @ref sf_app_shutDownDevice()              | @copybrief sf_app_shutDownDevice()              |
 *  @{
 */

/*=============================================================================
                                INCLUDES
=============================================================================*/
/* Standard include */
#include <stdint.h>

/*=============================================================================
                                API FUNCTIONS
=============================================================================*/
/*============================================================================*/
/**
 * \brief Handle received downlink from the BMS-CC. The customer can add his
 *        specific implementation.
 *
 * \param pInBuf        Pointer to the frame.
 * \param length        Frame length.
 */
/*============================================================================*/
void sf_app_handleDownlink(uint8_t* pInBuf, uint8_t length);

/*============================================================================*/
/**
 * \brief Bring device into shutdown mode. Before hand, it configures
 *        the wakeup pin.
 */
/*============================================================================*/
void sf_app_shutDownDevice(void);

/*! @} */

#endif /* __SF_APP_API_H__ */

#ifdef __cplusplus
}
#endif
