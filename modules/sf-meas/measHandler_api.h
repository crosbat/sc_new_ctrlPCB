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
 @brief      This header contains the measurement handler interface.

 @details  This module is out of scope of STACKFORCE work package. It's main
           task is to manage the measurements and feed the application with
           the next measurement to be transmitted.
 @note     measHandler.c provides a dummy implementation. The customer should expand it
           by his specific implementation.
*/

#ifndef __MEAS_HANDLER_H__
#define __MEAS_HANDLER_H__

/**
 *  @addtogroup CUSTOMER_MODULE
 *
 *  @details
 *
 *  - <b>MEASUREMENT HANDLER API</b>\n
 *    | API Function                              | Description                                     |
 *    |-------------------------------------------|-------------------------------------------------|
 *    | @ref measHandler_performMeas()            | @copybrief measHandler_performMeas()            |
 *    | @ref measHandler_getMeas()                | @copybrief measHandler_getMeas()                |
 *    | @ref measHandler_setStatus()              | @copybrief measHandler_setStatus()              |
 *  @{
 */

/*=============================================================================
                                INCLUDES
=============================================================================*/
/* Standard include */
#include "stdint.h"

/*=============================================================================
                                ENUMS
=============================================================================*/
/*! Defines the measurement status */
typedef enum
{
  /*! The measurement is successfully transmitted */
  E_MEAS_TX_SUCCESS,
  /*! The measurement is not transmitted */
  E_MEAS_TX_FAIL
} E_MEAS_STATUS_t;

/*=============================================================================
                                STRUCTS
=============================================================================*/
/*! Defines the measurement data */
typedef struct
{
  /* Timestamp */
  uint32_t timeStamp;
  /* Measurement value */
  float value;
} meas_t;

/*=============================================================================
                                API FUNCTIONS
=============================================================================*/
/*============================================================================*/
/**
 * \brief Trigger measurement read. A new measurement should be performed and
 *        prepared to be sent. If the previous measurement is not sent yet,
 *        it should be stored in the buffer for later retry.
 *
 * \return True - Everything is fine
 *         False - Otherwise.
 */
/*============================================================================*/
bool measHandler_performMeas(void);

/*============================================================================*/
/**
 * \brief Provide the newest measurement to be sent.
 *        If no new measurement available, the oldest measurement in the buffer
 *        should be returned.
 *
 * \param  pMeas     Pointer to the measurement storage.
 *
 * \return True : There is an available measurement.
 *         False: No available measurement.
 */
/*============================================================================*/
bool measHandler_getMeas(meas_t* pMeas);

/*============================================================================*/
/**
 * \brief Set the status of the last scheduled measurement. The measurement
 *        sender calls this functions to indicate about the status of the last
 *        transmitted measurement.
 *
 * \param  status   @ref E_MEAS_STATUS_t
 */
/*============================================================================*/
void measHandler_setStatus(E_MEAS_STATUS_t status);

/*! @} */

#endif /* __MEAS_HANDLER_H__ */

#ifdef __cplusplus
}
#endif
