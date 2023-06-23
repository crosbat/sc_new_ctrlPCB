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
 @brief      Implementation of the measurement transmission logic.
*/

/*=============================================================================
                                INCLUDES
=============================================================================*/
/* Standard include */
#include "stdint.h"
#include <string.h>
/* Stack include */
#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/packetbuf.h"
#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
#endif
#include "sys/log.h"
/* Project specific include */
#include "sf_frameType.h"
#include "sf_callbackHandler.h"
#include "sf_measSender.h"
#include "sf_configMgmt.h"
#include "measHandler_api.h"
#include "sf_absoluteTime.h"
#include "sf_tsch.h"

/*=============================================================================
                                MACROS
=============================================================================*/
#define LOG_MODULE "Measurement"
#ifndef LOG_CONF_APP
  #define LOG_LEVEL     LOG_LEVEL_NONE
#else
  #define LOG_LEVEL     LOG_CONF_APP
#endif
/* The maximum length of payload */
#define SF_APP_PAYLOAD_LENGTH_MAX        (40U)

/*=============================================================================
                                GLOBAL VARIABLES
=============================================================================*/
/* Stores callback handler context parameters. */
static sf_callbackHandlerCtxt_t gMeasCallbackHandlerCtxt = {sf_measSender_output_callback,
                                                            NULL};
/* Event object */
static process_event_t tx_event;

/*=============================================================================
                                PROCESSES
=============================================================================*/
PROCESS(meas_read_process, "Meas read process");
PROCESS(meas_tx_process, "Meas Tx process");

/*==============================================================================
                         LOCAL FUNCTION DEFINITION
==============================================================================*/
/*============================================================================*/
/**
 * \brief Builds the measurement frame and schedules it to be sent to the BMS-CC.
 *
 * \param pAddr            Destination address.
 * \param pMeas            Pointer to the measurement.
 * \param measLen          Length of the measurement.
 */
/*============================================================================*/
static void loc_sendMeas(linkaddr_t *pAddr, const uint8_t* pMeas, uint8_t measLen);

/*==============================================================================
                         LOCAL FUNCTION IMPLEMENTATION
==============================================================================*/
/*----------------------------------------------------------------------------*/
/*! loc_sendMeas */
/*----------------------------------------------------------------------------*/
static void loc_sendMeas(linkaddr_t *pAddr, const uint8_t* pMeas, uint8_t measLen)
{
  /* Storage for the measurement frame. */
  uint8_t pFrameBuf[SF_APP_PAYLOAD_LENGTH_MAX] = {0x00};
  /* Frame length */
  uint8_t frameLen = 0;

  /* Build measurement frame
     frame type  |  timestamp  |  measurement
     ------------|-------------|-------------
        1byte    |    4byte    |  nbyte     */
  sf_frameType_set(pFrameBuf, E_FRAME_TYPE_MEASUREMENT);
  frameLen += SF_FRAME_TYPE_LEN;

  memcpy(pFrameBuf + frameLen, pMeas, measLen);
  frameLen += measLen;

  LOG_INFO("New packet is transmitted to the BMS-CC; ");
  LOG_INFO_LLADDR(pAddr);
  LOG_INFO_("; ");
  LOG_INFO_BYTES(pFrameBuf, frameLen);
  LOG_INFO_("\n");

  /* Schedule frame Tx */
  sf_tsch_send(pFrameBuf, frameLen, pAddr, &gMeasCallbackHandlerCtxt);
}/* loc_sendMeas() */

/*=============================================================================
                          PROCESSES IMPLEMENTATION
=============================================================================*/
/*------------------------------------------------------------------------------
  meas_tx_process()
------------------------------------------------------------------------------*/
PROCESS_THREAD(meas_read_process, ev, data)
{
  /* Stores timer data. */
  static struct etimer periodicTimer;
  /* Stores measurement interval */
  static uint32_t measInterval;

  PROCESS_BEGIN();

  /* Start absolute counting. */
  sf_absoluteTime_startTimer();

  while(1)
  {
    /* Get configured measurement interval */
    sf_configMgmt_getParam(&measInterval, sizeof(measInterval),
                           E_CONFIGMGMT_PARAM_MEAS_INTERVAL);

    /* Check if measurement interval is at highest rate
       i.e., the measurement interval is set to lowest limit.
       If so the read time will be triggered by TSCH_CALLBACK_CYCLE_START
       callback. Otherwise, we use the etimer interrupt. */
    if(SF_CONFIGMGMT_MEAS_INTERVAL_LIMIT_LOWER >= measInterval)
    {
      LOG_INFO("Blocked until meas read time \n");
      PROCESS_WAIT_EVENT_UNTIL(ev == tx_event);
    }
    else
    {
      /* Wait until it is time to perform measurement. */
      etimer_set(&periodicTimer, measInterval * CLOCK_SECOND);
      /* Blocks until timer expires. */
      LOG_INFO("Blocked until meas read time | Meas interval: %lu second\n",
              measInterval);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodicTimer));
    }

    /* Perform measurement */
    measHandler_performMeas();
  }

  PROCESS_END();
}/* meas_tx_process() */

/*------------------------------------------------------------------------------
  meas_tx_process()
------------------------------------------------------------------------------*/
PROCESS_THREAD(meas_tx_process, ev, data)
{
  /* BMSCC address */
  linkaddr_t bmssccAddr = linkaddr_null;
  /* Storage for the measurement */
  meas_t meas;
  /* Measurement handler return value */
  bool measRet = false;

  PROCESS_BEGIN();

  /* Enable only one packet to be added to the TSCH packet queue. */
  nullnet_enable_single_packet();

  while(1)
  {
    /* Blocks until timer expires. */
    LOG_INFO("Blocked until transmission time...\n");
    PROCESS_WAIT_EVENT_UNTIL(ev == tx_event);

    if(tsch_is_associated && E_CONFIGMGMT_DEVICESTATUS_CONNECTED ==
       sf_configMgmt_getDeviceStatus())
    {
      measRet = measHandler_getMeas(&meas);
      if(true == measRet)
      {
        /* Get the BMSCC linkaddr from the stored configuration. */
        sf_configMgmt_getParam(bmssccAddr.u8, LINKADDR_SIZE,
                                E_CONFIGMGMT_PARAM_GW_ADDR);

        /* Set the max number of transmissions, 2 = 1 transmission + 1 retry. */
        packetbuf_set_attr(PACKETBUF_ATTR_MAX_MAC_TRANSMISSIONS, 2U);

        loc_sendMeas(&bmssccAddr, (uint8_t*)&meas, sizeof(meas_t));
      }
    }
    else
    {
      LOG_INFO("Waiting for connection to the BMS-CC...\n");
    }
  }

  PROCESS_END();
}/* meas_tx_process() */

/*=============================================================================
                                API IMPLEMENTATION
=============================================================================*/
/*------------------------------------------------------------------------------
  sf_measSender_start()
------------------------------------------------------------------------------*/
void sf_measSender_start(void)
{
  tx_event = process_alloc_event();

  if(process_is_running(&meas_read_process))
  {
    process_exit(&meas_read_process);
  }

  process_start(&meas_read_process, NULL);

  if(process_is_running(&meas_tx_process))
  {
    process_exit(&meas_tx_process);
  }

  process_start(&meas_tx_process, NULL);
} /* sf_measSender_start() */

/*------------------------------------------------------------------------------
  sf_measSender_output_callback()
------------------------------------------------------------------------------*/
void sf_measSender_output_callback(void *ptr, nullnet_tx_status_t status)
{
  if(NULLNET_TX_OK == status)
  {
    LOG_INFO("Tx successful\n");
    measHandler_setStatus(E_MEAS_TX_SUCCESS);
  }
  else
  {
    LOG_INFO("Tx failed\n");
    measHandler_setStatus(E_MEAS_TX_FAIL);
  }
} /* sf_measSender_output_callback() */

/*=============================================================================
                                CALLBACK IMPLEMENTATION
=============================================================================*/
/*============================================================================*/
/**
 * \brief A TSCH callback function that indicates about the start of new Tx
 *        cycle i.e., at this time the current slot is slot zero.
 * \details We use this callback to:
 *            - Schedule measurement transmission.
 *            - Perform measurement if the measurement rate is set to highest
                i.e. measurement interval of 3s
 *          It replaces the usage of etimer events as it is impossible to set a
 *          timeout of fraction of seconds using the CONTIKI event timer.
 */
/*============================================================================*/
void sf_measSender_cycleStartCallback(void)
{
  /* Trigger TX process */
  process_post(&meas_tx_process, tx_event, NULL);

  /* Trigger meas read process */
  process_post(&meas_read_process, tx_event, NULL);
}

#ifdef __cplusplus
}
#endif
