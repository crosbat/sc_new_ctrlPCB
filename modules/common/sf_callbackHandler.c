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
 @brief      Implementation of the input callback handler logic.
*/

/*=============================================================================
                                INCLUDES
=============================================================================*/
/* Stack include */
#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
#endif
#include "net/nullnet/nullnet.h"
/* Application include */
#include "sf_frameType.h"
#include "sf_join.h"
#include "sf_measSender.h"
#include "sf_callbackHandler.h"
#include "sf_app_api.h"

/* Log configuration */
#include "sys/log.h"

/*=============================================================================
                                  MACROS
=============================================================================*/
#define LOG_MODULE "CbH"
#ifndef LOG_CONF_APP
  #define LOG_LEVEL     LOG_LEVEL_NONE
#else
  #define LOG_LEVEL     LOG_CONF_APP
#endif

/*=============================================================================
                              API IMPLEMENTATION
=============================================================================*/
/*----------------------------------------------------------------------------*/
/*! sf_output_callback_handler */
/*----------------------------------------------------------------------------*/
void sf_output_callback_handler(void *ptr, nullnet_tx_status_t status,
                                uint8_t ack_app_data)
{
  sf_callbackHandlerCtxt_t *callbackHandlerCtxt;

  /* Null pointer check. */
  if(NULL == ptr)
  {
    return;
  }

  /* Cast ptr into callback handler context. */
  callbackHandlerCtxt = (sf_callbackHandlerCtxt_t*) ptr;

  if(callbackHandlerCtxt->callbackFctPointer == sf_join_output_callback)
  {
    /* Output callback is from a join message. */
    sf_join_output_callback(callbackHandlerCtxt->callbackFctDataPointer, status);
  }
  else if(callbackHandlerCtxt->callbackFctPointer == sf_measSender_output_callback)
  {
    /* Output callback is from application message. */
    sf_measSender_output_callback(callbackHandlerCtxt->callbackFctDataPointer, status);
  }
} /* sf_output_callback_handler() */

/*----------------------------------------------------------------------------*/
/*! sf_input_callback_handler */
/*----------------------------------------------------------------------------*/
void sf_input_callback_handler(const void *data, uint16_t len,
                               const linkaddr_t *src, const linkaddr_t *dest)
{
  /* Stores frame type. */
  E_FRAME_TYPE_t frameType = E_FRAME_TYPE_UNDEFINED;
  E_SF_RETURN_t packetHandleStatus = E_SF_ERROR;

  /* Get RSSI of received packet */
  int rssi = 0;

  NETSTACK_RADIO.get_value(RADIO_PARAM_LAST_RSSI, &rssi);

  LOG_INFO(" Src: ");
  LOG_INFO_LLADDR(src);
  LOG_INFO_("; RSSI (dBm); %d;\n", rssi);

  packetHandleStatus = sf_frameType_get(data, &frameType);

  if(packetHandleStatus == E_SF_SUCCESS)
  {
    switch(frameType)
    {
      case E_FRAME_TYPE_REQUEST:
      case E_FRAME_TYPE_SUCCESSFUL:
      case E_FRAME_TYPE_RESPONSE:
        sf_join_handleFrame((uint8_t*)data, len, (linkaddr_t *)src);
        break;
      case E_FRAME_TYPE_REMOTE:
        sf_app_handleDownlink((uint8_t*)data, len);
        break;
      case E_FRAME_TYPE_UNDEFINED:
        LOG_INFO("Rx invalid 1\n");
        break;

      default:
        LOG_INFO("Rx invalid 2\n");
        break;
    }
  }
  else
  {
    LOG_ERR("!Can not get frame type");
  }

}/* sf_input_callback_handler() */

#ifdef __cplusplus
}
#endif
