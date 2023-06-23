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
 @brief      This header contains the TSCH callback handler interface.
*/

#ifndef __SF_CALLBACK_HANDLER_H__
#define __SF_CALLBACK_HANDLER_H__

/*=============================================================================
                                INCLUDES
=============================================================================*/
/* Standard include */
#include <stdint.h>
#include <stdlib.h>
/* Stack include */
#include "net/nullnet/nullnet.h"

/*=============================================================================
                                STRUCTS
=============================================================================*/
/** Callback handler context structure. */
typedef struct
{
  /* Pointer to the output callback function */
  const void *callbackFctPointer;
  /* Pointer to the application data */
  void *callbackFctDataPointer;
} sf_callbackHandlerCtxt_t;

/*=============================================================================
                                API FUNCTIONS
=============================================================================*/
/*============================================================================*/
/**
 * \brief Input Callback Handler.
 *
 * \param	data      Data Pointer.
 * \param	len       Length of the Data.
 * \param	src       Linkaddress Source Pointer.
 * \param	dest      Linkaddress Destination Pointer.
 */
/*============================================================================*/
void sf_input_callback_handler(const void *data, uint16_t len,
                               const linkaddr_t *src, const linkaddr_t *dest);

/*============================================================================*/
/**
 * \brief Output Callback Handler.
 *
 * \param	ptr            Callback Handler Pointer.
 * \param	status         Nullnet tx status.
 * \param ack_app_data   ack app data integer.
 */
/*============================================================================*/
void sf_output_callback_handler(void *ptr, nullnet_tx_status_t status,
                                uint8_t ack_app_data);

#endif /* __SF_CALLBACK_HANDLER_H__ */

#ifdef __cplusplus
}
#endif
