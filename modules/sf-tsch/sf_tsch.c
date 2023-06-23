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
 @brief      Implementation of communication stack abstraction interface.
*/

/*==============================================================================
                            INCLUDES
==============================================================================*/
/* Stack include */
#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
#endif
/* Application include */
#include "sf_callbackHandler.h"
#include "sf-tsch-schedule.h"
#include "sf_tsch.h"

/*==============================================================================
                      API FUNCTION IMPLEMENTATION
==============================================================================*/
/*----------------------------------------------------------------------------*/
/*! sf_tsch_init */
/*----------------------------------------------------------------------------*/
void sf_tsch_init(void)
{
  /* Set nullnet Rx callback function. */
  nullnet_set_input_callback(sf_input_callback_handler);
  /* Set transmission callback. */
  nullnet_set_output_callback(sf_output_callback_handler);

  /* Initialize schedule */
  sf_tsch_schedule_init();

  /* Add beacon slots */
  sf_tsch_schedule_add_beacon_slots();
}/* sf_tsch_init() */

/*----------------------------------------------------------------------------*/
/*! sf_tsch_start */
/*----------------------------------------------------------------------------*/
void sf_tsch_start(void)
{
  /* Set device as end point */
  tsch_set_coordinator(0);

  /* Turn on MAC */
  NETSTACK_MAC.on();

  /* Enable TSCH beacon scan. */
  tsch_scan_enable();
}/* sf_tsch_start() */

/*----------------------------------------------------------------------------*/
/*! sf_tsch_stop */
/*----------------------------------------------------------------------------*/
void sf_tsch_stop(void)
{
  /* Switch of MAC. */
  NETSTACK_MAC.off();
}/* sf_tsch_stop() */

/*----------------------------------------------------------------------------*/
/*! sf_tsch_setTxPowerMax */
/*----------------------------------------------------------------------------*/
void sf_tsch_setTxPowerMax(void)
{
  /* Radio TX Power */
  radio_value_t value;

  NETSTACK_RADIO.get_value(RADIO_CONST_TXPOWER_MAX, &value);
  NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, value);
}/* sf_tsch_setTxPowerMax() */

/*----------------------------------------------------------------------------*/
/*! sf_tsch_setDeviceAddress */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_tsch_setDeviceAddress(linkaddr_t* pAddr)
{
  if(!pAddr)
  {
    return E_SF_ERROR_NPE;
  }

  if(!linkaddr_cmp(pAddr, &linkaddr_null))
  {
    linkaddr_set_node_addr(pAddr);

    return E_SF_SUCCESS;
  }

  return E_SF_ERROR;
}/* sf_tsch_setDeviceAddress() */

/*----------------------------------------------------------------------------*/
/*! sf_tsch_addDataSlots */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_tsch_addDataSlots(const linkaddr_t* pAddr)
{
  if(!pAddr)
  {
    return E_SF_ERROR_NPE;
  }

  /* Add data slots. */
  if(!sf_tsch_schedule_add_data_slots(pAddr))
  {
    return E_SF_SUCCESS;
  }

  return E_SF_ERROR;
}/* sf_tsch_addDataSlots() */

/*----------------------------------------------------------------------------*/
/*! sf_tsch_send */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_tsch_send(uint8_t* pFrame, uint8_t frameLen,
                           linkaddr_t* pDestAddr,
                           sf_callbackHandlerCtxt_t* pCallbackHandlerCtx)
{
  if(!pFrame || !pDestAddr || !pCallbackHandlerCtx)
  {
    return E_SF_ERROR_NPE;
  }

  /* Fill out nullnet buffer. */
  nullnet_buf = pFrame;
  nullnet_len = frameLen;

  /* Transmit payload to the destination address */
  NETSTACK_NETWORK.output(pDestAddr, (void*) pCallbackHandlerCtx,
                          FRAME802154_DATAFRAME);

  return E_SF_SUCCESS;
} /* sf_tsch_send() */

#ifdef __cplusplus
}
#endif
