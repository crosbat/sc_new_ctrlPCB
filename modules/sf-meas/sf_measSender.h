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
 @brief      This header contains the measurement sender interface.
*/

#ifndef __SF_MEASSENDER_H__
#define __SF_MEASSENDER_H__

/**
 *  @addtogroup SF_MEAS_SENDER
 *
 *  @details
 *
 *  - <b>SF MEAS SENDER API</b>\n
 *    | API Function                              | Description                                     |
 *    |-------------------------------------------|-------------------------------------------------|
 *    | @ref sf_measSender_start()                | @copybrief sf_measSender_start()                |
 *    | @ref sf_measSender_output_callback()      | @copybrief sf_measSender_output_callback()      |
 *  @{
 */

/*=============================================================================
                                INCLUDES
=============================================================================*/
/* Standard include */
#include <stdint.h>
#include <stdlib.h>
/* Stack specific includes */
#include "nullnet.h"

/*=============================================================================
                                API FUNCTIONS
=============================================================================*/
/*============================================================================*/
/**
 * \brief Kick off the measurement sender module. This triggers tow processes
 *        meas_read_process and meas_tx_process. The first process periodically
 *        performs new measurement based on the configured measurement interval
 *        by calling @ref measHandler_performMeas.
 *        The second process controls the transmission time. At every slot cycle
 *        it checks for measurement by calling @ref measHandler_getMeas
 *        and sends it if available.
 */
/*============================================================================*/
void sf_measSender_start(void);

/*============================================================================*/
/**
 * \brief This is a callback function that TSCH calls to inform about
 *        measurement packet Tx status. This function, basically, forwards
 *        the status to the measurement handler by calling
 *        @ref measHandler_setStatus.
 *
 * \param ptr         Pointer to the  data.
 * \param status      Status of nullnet Tx.
 */
/*============================================================================*/
void sf_measSender_output_callback(void *ptr, nullnet_tx_status_t status);

/*! @} */

#endif /* __SF_MEASSENDER_H__ */

#ifdef __cplusplus
}
#endif
