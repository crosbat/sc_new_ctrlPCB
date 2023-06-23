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
 @brief      Implementation of the joining process logic.
*/

/*=============================================================================
                                INCLUDES
=============================================================================*/
/* Stack specific includes */
#include "net/nullnet/nullnet.h"
#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
#include "sf-tsch-schedule.h"
#endif
#include "sys/log.h"
/* Application specific includes */
#include "project-conf.h"
#include "sf_join.h"
#include "sf_joinRequester.h"
#include "sf_frameType.h"
#include "sf_stateManager.h"
#include "sf_configMgmt.h"
#include "sf_tsch.h"
#include "sf_led.h"
#include "sf_beaconScan.h"
#include "sf_app_api.h"

/*=============================================================================
                                MACROS
=============================================================================*/
#define LOG_MODULE "Join"
#ifndef LOG_CONF_JOIN
  #define LOG_LEVEL     LOG_LEVEL_NONE
#else
  #define LOG_LEVEL     LOG_CONF_JOIN
#endif
/*============================================================================*/
/*!
 * @brief Converts an uint16_t to an uint8_t array.
 * @param x           uint8_t array to write the value in.
 * @param y           Value to convert.
 */
/*============================================================================*/
#define UINT16_TO_UINT8(x, y)  ((x)[0] = (uint8_t)((uint8_t)((y)>>8U) & 0xFFU)); \
                               ((x)[1] = (uint8_t)((y) & 0xFFU))
/* The join frame retransmit time */
#ifndef SF_JOIN_CONF_RETRY_TX_TIME
#define SF_JOIN_RETRY_TX_TIME      (3 * CLOCK_SECOND)
#else
#define SF_JOIN_RETRY_TX_TIME      (SF_JOIN_CONF_RETRY_TX_TIME * CLOCK_SECOND)
#endif
/* Timeout for join request Tx state */
#define SF_JOIN_REQ_TIMEOUT        (120 * CLOCK_SECOND)
/* Timeout for join response Rx state */
#define SF_JOIN_RESP_TIMEOUT       (420 * CLOCK_SECOND)
/* Timeout for join successful Tx state */
#define SF_JOIN_SUCC_TIMEOUT       (30 * CLOCK_SECOND)
/* Overall join process timeout. */
#define SF_JOIN_TIMEOUT            SF_JOIN_REQ_TIMEOUT + SF_JOIN_RESP_TIMEOUT +\
                                   SF_JOIN_SUCC_TIMEOUT
/* Define the number of the join states. */
#define SF_JOIN_STATES_COUNT       (3U)
/* Defines rejoin retry interval. */
#define SF_JOIN_REJOIN_INTERVAL    (5 * 60 * CLOCK_SECOND)

/*=============================================================================
                                ENUMS
=============================================================================*/
/* Defines the join process states. */
typedef enum
{
  /* Join request transmission state. */
  E_JOIN_STATE_REQ_TX,
  /* Join response reception state. */
  E_JOIN_STATE_RESP_RX,
  /* Join successful transmission state. */
  E_JOIN_STATE_SUCC_TX
} E_JOIN_STATE_t;

/*=============================================================================
                          LOCAL FUNCTION DECLARATION
=============================================================================*/
static void loc_txJoinRequest(void);
static void loc_rxJoinResponse(void);
static void loc_txJoinSuccessful(void);

/*=============================================================================
                                GLOBAL VARIABLES
=============================================================================*/
/* Pointer to the gw address */
static linkaddr_t *gpDestinationAddress = NULL;
/* Store the state manager context. */
static stateManager_ctx_t gJoinStateCtx = {0};
/* Store whether a join request is ongoing or not.
   false: to say no join request ongoing in the mac.
   true: to say there is join request ongoing and we are waiting for a callback
         from the mac. */
static bool gReqActive = false;
/* Store whether a join successful is ongoing or not.
   false: to say no join successful ongoing in the mac.
   true: to say there is join successful ongoing and we are waiting for a callback
         from the mac. */
static bool gSuccActive = false;
/* Stores the status of request Tx. */
static bool gReqTxFailed = false;
/* Stores the status of successful Tx. */
static bool gSuccTxFailed = false;
/* Join request timeout handler. */
static struct etimer gJoinRequestTimer;
/* Join successful timeout handler. */
static struct etimer gJoinSuccessfulTimer;
/* Join response timeout handler. */
static struct ctimer gJoinResponseTimer;
/* Overall join process timer. */
static struct ctimer gJoinTimer;
/* Array of state-manager states. */
static stateManager_state_t gpStates[] =
{
  {E_JOIN_STATE_REQ_TX, &loc_txJoinRequest},
  {E_JOIN_STATE_RESP_RX, &loc_rxJoinResponse},
  {E_JOIN_STATE_SUCC_TX, &loc_txJoinSuccessful}
};

/*=============================================================================
                                PROCESSES
=============================================================================*/
PROCESS(manaul_join_process, "Manual join process");
PROCESS(rejoin_process, "Rejoin process");
PROCESS(handshake_process, "Handshake process");
PROCESS(request_tx_process, "Req Tx process");
PROCESS(successful_tx_process, "Succ Tx process");
PT_THREAD(beacon_scan_process(struct pt *pt));

/*=============================================================================
                          LOCAL FUNCTION IMPLEMENTATION
=============================================================================*/
/*------------------------------------------------------------------------------
  loc_join_timeout_callback()
------------------------------------------------------------------------------*/
static void loc_join_timeout_callback(void * ptr)
{
  LOG_INFO("Join timeout expired. Exit join\n");

  /* Enable back single packet. */
  nullnet_enable_single_packet();

  /* Remove the join request slots from the schedule. */
  sf_tsch_schedule_delete_jreq_slots();
  /* Remove the join process slots from the schedule. */
  sf_tsch_schedule_delete_jproc_slots(NULL);

  /* Stop overall timeout. */
  ctimer_stop(&gJoinTimer);

  /* Exit all join processes. */
  process_exit(&request_tx_process);
  process_exit(&successful_tx_process);

  /* Trigger the join process to continue */
  process_post(&manaul_join_process, PROCESS_EVENT_CONTINUE, NULL);
} /* loc_join_timeout_callback() */

/*------------------------------------------------------------------------------
  loc_txJoinRequest()
------------------------------------------------------------------------------*/
static void loc_txJoinRequest( void )
{
  /* Schedule join request slots. */
  sf_tsch_schedule_add_jreq_slots();

  /* Start state timeout. */
  etimer_set(&gJoinRequestTimer, SF_JOIN_REQ_TIMEOUT);

  /* Start request Tx process. */
  process_start(&request_tx_process, NULL);
}/* loc_txJoinRequest() */

/*------------------------------------------------------------------------------
  loc_txJoinSuccessful()
------------------------------------------------------------------------------*/
static void loc_txJoinSuccessful(void)
{
  /* Set join state timeout. */
  etimer_set(&gJoinSuccessfulTimer, SF_JOIN_SUCC_TIMEOUT);

  /* Start successful Tx process. */
  process_start(&successful_tx_process, NULL);
}/* loc_txJoinSuccessful() */

/*------------------------------------------------------------------------------
  loc_rxJoinResponse()
------------------------------------------------------------------------------*/
static void loc_rxJoinResponse(void)
{
  /* Start join state timeout. */
  ctimer_set(&gJoinResponseTimer, SF_JOIN_RESP_TIMEOUT, loc_join_timeout_callback, NULL);

  /* Open join process slots to the gw. */
  sf_tsch_schedule_add_jproc_slots((const linkaddr_t*)gpDestinationAddress);

  LOG_INFO("Wait for resp Rx\n");
}/* loc_rxJoinResponse() */

/*=============================================================================
                          PROCESSES IMPLEMENTATION
=============================================================================*/
/*------------------------------------------------------------------------------
  request_tx_process()
------------------------------------------------------------------------------*/
PROCESS_THREAD(request_tx_process, ev, data)
{
  /* Timer handler. */
  static struct etimer periodicTimer;

  PROCESS_BEGIN();

  while(1)
  {
    /* Blocks until timeout expires. */
    LOG_INFO("Block until req Tx timeout expires; %lu sec\n",
             (long unsigned)(SF_JOIN_RETRY_TX_TIME / CLOCK_SECOND));
    /* Set Tx timeout. */
    etimer_set(&periodicTimer, SF_JOIN_RETRY_TX_TIME);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodicTimer));

    /* Check the correctness of the join state */
    if(E_JOIN_STATE_REQ_TX == (E_JOIN_STATE_t)sf_stateManager_getState(&gJoinStateCtx))
    {
      /* Check if the state timeout is already expired */
      if(etimer_expired(&gJoinRequestTimer) && false == gReqActive)
      {
        loc_join_timeout_callback(NULL);

        break;
      }

      /* Check if the device is still associated */
      if(tsch_is_associated)
      {
        /* Check if there is join request pending in the lower layer */
        if(false == gReqActive)
        {
          gReqActive = true;
          gReqTxFailed = false;
          /* Use as destination the device from where it gets the time source. */
          gpDestinationAddress = tsch_queue_get_nbr_address(tsch_queue_get_time_source());
          if(E_SF_SUCCESS != sf_join_request_send(gpDestinationAddress))
          {
            gReqActive = false;
            LOG_ERR("!Failed req Tx\n");
          }
        }
        else
        {
          LOG_ERR("!Failed req Tx; Active req \n");
        }
      }
      else
      {
        LOG_INFO("Wait for cxn to GW\n");
      }
    }
    else
    {
      PROCESS_EXIT();
    }
  }

  PROCESS_END();
}/* request_tx_process() */

/*------------------------------------------------------------------------------
  successful_tx_process()
------------------------------------------------------------------------------*/
PROCESS_THREAD(successful_tx_process, ev, data)
{
  /* Timer handler. */
  static struct etimer periodicTimer;

  PROCESS_BEGIN();

  while(1)
  {
    /* Blocks until timer expires. */
    LOG_INFO("Blocked until succ Tx timeout expires; %lu sec\n",
            (long unsigned)(SF_JOIN_RETRY_TX_TIME / CLOCK_SECOND));
    /* Set timeout. */
    etimer_set(&periodicTimer, SF_JOIN_RETRY_TX_TIME);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodicTimer));

    if((E_CONFIGMGMT_DEVICESTATUS_NOT_CONNECTED == sf_configMgmt_getDeviceStatus()) &&
       (E_JOIN_STATE_SUCC_TX == (E_JOIN_STATE_t)sf_stateManager_getState(&gJoinStateCtx)) )
    {
      /* Check if state timeout is already expired */
      if(etimer_expired(&gJoinSuccessfulTimer) && false == gSuccActive)
      {
        loc_join_timeout_callback(NULL);

        break;
      }

      /* Check if the TSCH is still associated */
      if(tsch_is_associated)
      {
        /* Check if a successful frame is pending in the lower layer */
        if(false == gSuccActive)
        {
          gSuccActive = true;
          gSuccTxFailed = false;
          /* Use as destination the device from where it gets the time source. */
          gpDestinationAddress = tsch_queue_get_nbr_address(tsch_queue_get_time_source());
          if(E_SF_SUCCESS != sf_join_successful_send(gpDestinationAddress))
          {
            gSuccActive = false;
            LOG_ERR("!Fail Tx succ to gw\n");
          }
        }
        else
        {
          LOG_ERR("!Fail Tx succ. Active succ\n");
        }
      }
      else
      {
        LOG_INFO("Wait for cxn to gw \n");
      }
    }
    else
    {
      PROCESS_EXIT();
    }
  }

  PROCESS_END();
}/* successful_tx_process() */

/*------------------------------------------------------------------------------
  handshake_process()
------------------------------------------------------------------------------*/
PROCESS_THREAD(handshake_process, ev, data)
{
  PROCESS_BEGIN();

  /* Initialize global variables */
  gReqActive = false;
  gSuccActive = false;

  /* Use as destination the device where it gets the time source from. */
  gpDestinationAddress = tsch_queue_get_nbr_address(tsch_queue_get_time_source());

  /* Initialize join status. */
  sf_configMgmt_setDeviceStatus(E_CONFIGMGMT_DEVICESTATUS_NOT_CONNECTED);

  /* Initialize state manager. */
  sf_stateManager_init(&gJoinStateCtx, gpStates, SF_JOIN_STATES_COUNT);

  /* Disable single packet. */
  nullnet_disable_single_packet();

  /* Check E_JOIN_STATE_REQ_TX state. */
  sf_stateManager_setState(&gJoinStateCtx, (uint8_t)E_JOIN_STATE_REQ_TX);
  sf_stateManager_execState(&gJoinStateCtx);

  /* Set overall timeout. If this timeout expires. The process will abort */
  ctimer_set(&gJoinTimer, SF_JOIN_TIMEOUT, loc_join_timeout_callback, NULL);
  PROCESS_YIELD_UNTIL(E_CONFIGMGMT_DEVICESTATUS_CONNECTED ==
                      sf_configMgmt_getDeviceStatus());

  /* Stop over all timer */
  ctimer_stop(&gJoinTimer);

  /* Enable single packet. */
  nullnet_enable_single_packet();

  /* Trigger the join process to continue */
  process_post(&manaul_join_process, PROCESS_EVENT_CONTINUE, NULL);

  PROCESS_END();
} /* handshake_process() */


/*------------------------------------------------------------------------------
  manaul_join_process()
------------------------------------------------------------------------------*/
PROCESS_THREAD(manaul_join_process, ev, data)
{
  /* Beacon scan protothread reference. */
  static struct pt beacon_scan_pt;

  PROCESS_BEGIN();

  LOG_INFO("Start manual join; The Manual join window should be open on the BMSCC;\n");

  /* Green LED blinks during manual beacon scan*/
  sf_led_startBlink(LEDS_CONF_GREEN, 200, 800);

  /* Start beacon scan. It will be scanning for beacons with manual join flag.
    If more than one beacon is received, the highest RSSI value will be retained.*/
  sf_beaconScan_setBeaconType(E_SF_JOIN_BEACON_ENABLED);
  PROCESS_PT_SPAWN(&beacon_scan_pt, beacon_scan_process(&beacon_scan_pt));

  sf_led_stopBlink();

  if(!linkaddr_cmp(&linkaddr_null, sf_beaconScan_getBestBeaconAddress()))
  {
    /* Initiate TSCH */
    sf_tsch_init();

    /* Start TSCH as end point*/
    sf_tsch_start();

    /* Wait until TSCH get synchronized */
    PROCESS_YIELD_UNTIL(tsch_is_associated);

    /* Red LED blinks during join process */
    sf_led_startBlink(LEDS_CONF_RED, 500, 500);

    /* Start handshake process. */
    process_start(&handshake_process, NULL);
    /* Wait until handshake process returns */
    PROCESS_YIELD_UNTIL(PROCESS_EVENT_CONTINUE == ev);

    sf_led_stopBlink();

    if(E_CONFIGMGMT_DEVICESTATUS_CONNECTED == sf_configMgmt_getDeviceStatus())
    {
      sf_led_onDuration(LEDS_CONF_GREEN, 2000);
      PROCESS_WAIT_EVENT_UNTIL(sf_led_waitUntilOff() ||
                                ev == PROCESS_EVENT_EXIT);
    }
    else
    {
      LOG_ERR("!Failed handshake \n");

      sf_led_onDuration(LEDS_CONF_RED, 2000);
      PROCESS_WAIT_EVENT_UNTIL(sf_led_waitUntilOff() ||
                                ev == PROCESS_EVENT_EXIT);
#if !CONTIKI_TARGET_COOJA
      sf_app_shutDownDevice();
#endif

    }
  }
  else
  {
    LOG_ERR("!Failed beacon scan \n");

    sf_led_onDuration(LEDS_CONF_RED, 2000);
    PROCESS_WAIT_EVENT_UNTIL(sf_led_waitUntilOff() ||
                              ev == PROCESS_EVENT_EXIT);
#if !CONTIKI_TARGET_COOJA
    sf_app_shutDownDevice();
#endif
  }

  PROCESS_END();
}/* manaul_join_process() */

/*------------------------------------------------------------------------------
  rejoin_process()
------------------------------------------------------------------------------*/
PROCESS_THREAD(rejoin_process, ev, data)
{
  /* Device address */
  linkaddr_t deviceAddress;
  /* Beacon scan protothread reference. */
  static struct pt beacon_scan_pt;
  /* Rejoin retry timer */
  static struct etimer retryTimer;

  PROCESS_BEGIN();

  LOG_INFO("Start rejoin process; No handshake will be performed;\n");

  while(E_CONFIGMGMT_DEVICESTATUS_NOT_CONNECTED == sf_configMgmt_getDeviceStatus())
  {
    /* Red LED blinks during beacon scan*/
    sf_led_startBlink(LEDS_CONF_RED, 200, 800);

    /* Start beacon scan. It will be scanning for beacons without manual join flag
       coming from the stored BMS-CC within the stored network.*/
    sf_beaconScan_setBeaconType(E_SF_JOIN_BEACON_DISABLED);
    PROCESS_PT_SPAWN(&beacon_scan_pt, beacon_scan_process(&beacon_scan_pt));

    sf_led_stopBlink();

    if(!linkaddr_cmp(&linkaddr_null, sf_beaconScan_getBestBeaconAddress()))
    {
      /* Initiate TSCH */
      sf_tsch_init();

      /* Start TSCH as end point*/
      sf_tsch_start();

      /* Wait until TSCH get synchronized */
      PROCESS_YIELD_UNTIL(tsch_is_associated);

      if(E_SF_ERROR == sf_configMgmt_getParam(deviceAddress.u8,
                                              sizeof(deviceAddress.u8),
                                              E_CONFIGMGMT_PARAM_DEVICE_ADDR))
      {
        LOG_ERR("!Failed to read device address \n");
      }
      else
      {
        LOG_INFO("Stored device address %i \n", deviceAddress.u16);
      }

      /* Add data slots. */
      sf_tsch_addDataSlots((const linkaddr_t*)&deviceAddress);

      /* Set the global connected state to connected. */
      sf_configMgmt_setDeviceStatus(E_CONFIGMGMT_DEVICESTATUS_CONNECTED);

      sf_led_onDuration(LEDS_CONF_GREEN, 2000);
      PROCESS_WAIT_EVENT_UNTIL(sf_led_waitUntilOff() ||
                                ev == PROCESS_EVENT_EXIT);
    }
    else
    {
      LOG_ERR("!Failed beacon scan \n");

      sf_led_onDuration(LEDS_CONF_RED, 2000);
      PROCESS_WAIT_EVENT_UNTIL(sf_led_waitUntilOff() ||
                                ev == PROCESS_EVENT_EXIT);

      /* Retry to rejoin */
      LOG_INFO("Wait for %u sec then retry to rejoin again\n", SF_JOIN_REJOIN_INTERVAL /
              CLOCK_SECOND);
      etimer_set(&retryTimer, SF_JOIN_REJOIN_INTERVAL);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&retryTimer));
    }
  }

  PROCESS_END();
}/* rejoin_process() */

/*=============================================================================
                          API IMPLEMENTATION
=============================================================================*/
/*------------------------------------------------------------------------------
  sf_joinRequester_join()
------------------------------------------------------------------------------*/
void sf_joinRequester_join(void)
{
  if(process_is_running(&manaul_join_process))
  {
    process_exit(&manaul_join_process);
  }

  process_start(&manaul_join_process, NULL);
}/* sf_joinRequester_join() */

/*------------------------------------------------------------------------------
  sf_joinRequester_rejoin()
------------------------------------------------------------------------------*/
void sf_joinRequester_rejoin(void)
{
  if(process_is_running(&rejoin_process))
  {
    process_exit(&rejoin_process);
  }

  process_start(&rejoin_process, NULL);
}/* sf_joinRequester_rejoin() */

/*=============================================================================
                          CALLBACKS IMPLEMENTATION
=============================================================================*/
/*------------------------------------------------------------------------------
  sf_join_output_callback()
------------------------------------------------------------------------------*/
void sf_join_output_callback(void *ptr, nullnet_tx_status_t status)
{
  if(E_JOIN_STATE_REQ_TX == (E_JOIN_STATE_t)sf_stateManager_getState(&gJoinStateCtx))
  {
    if((NULLNET_TX_OK != status) && (NULLNET_TX_BUSY != status))
    {
      /* A request fragment not successfully transmitted. */
      LOG_ERR("!Failed Tx req frame\n");
      gReqTxFailed = true;
    }

    /* If we reach here, then we are sure that no active join request
      exists in the mac. */
    gReqActive = false;

    if(false == gReqTxFailed)
    {
      LOG_INFO("Tx req to gw ");
      LOG_INFO_LLADDR(gpDestinationAddress);
      LOG_INFO_(".\n");

      /* Remove the join request slots from the schedule. */
      sf_tsch_schedule_delete_jreq_slots();
      /* Stop state timeout */
      etimer_stop(&gJoinRequestTimer);

      /* Set next state to E_JOIN_STATE_RESP_RX */
      sf_stateManager_setState(&gJoinStateCtx,(uint8_t)E_JOIN_STATE_RESP_RX);
      sf_stateManager_execState(&gJoinStateCtx);
    }
    else
    {
      /* Check if state timeout is expired */
      if(etimer_expired(&gJoinRequestTimer))
      {
        loc_join_timeout_callback(NULL);
      }
    }
  }
  else if(E_JOIN_STATE_SUCC_TX == (E_JOIN_STATE_t)sf_stateManager_getState(&gJoinStateCtx))
  {
    if((NULLNET_TX_OK != status) && (NULLNET_TX_BUSY != status))
    {
      /* A successful fragment not successfully transmitted. */
      LOG_ERR("!Failed Tx Succ frame \n");
      gSuccTxFailed = true;
    }

    /* If reach here, then we are sure that no active join successful
      exists in the mac. */
    gSuccActive = false;

    if((false == gSuccTxFailed) &&
        (E_CONFIGMGMT_DEVICESTATUS_NOT_CONNECTED == sf_configMgmt_getDeviceStatus()))
    {
      LOG_INFO("Tx succ to gw ");
      LOG_INFO_LLADDR(gpDestinationAddress);
      LOG_INFO_(".\n");

      /* Remove the join process slots from the schedule as
        the device has already joined. */
      sf_tsch_schedule_delete_jproc_slots(NULL);

      /* Schedule data slots. */
      sf_tsch_schedule_add_data_slots((const linkaddr_t*)&linkaddr_node_addr);

      /* Stop state timeout */
      etimer_stop(&gJoinSuccessfulTimer);

      /* Store device configuration.  */
      uint16_t panId = frame802154_get_pan_id();
      LOG_INFO("Set PAN ID of device to 0x%04x\n", panId);

      /* Set communication params */
      sf_configMgmt_setParam(&panId, sizeof(panId), E_CONFIGMGMT_PARAM_PAN_ID);
      sf_configMgmt_setParam(linkaddr_node_addr.u8, sizeof(linkaddr_node_addr.u8),
                              E_CONFIGMGMT_PARAM_DEVICE_ADDR);
      sf_configMgmt_setParam(gpDestinationAddress->u8, sizeof(gpDestinationAddress->u8),
                              E_CONFIGMGMT_PARAM_GW_ADDR);
      sf_configMgmt_writeConfig();

      LOG_INFO("Device joined NW successfully; New device ID; ");
      LOG_INFO_LLADDR(&linkaddr_node_addr);
      LOG_INFO_("\n");

      /* Set the global connected state to connected. */
      sf_configMgmt_setDeviceStatus(E_CONFIGMGMT_DEVICESTATUS_CONNECTED);
    }
    else
    {
      /* Check if the state timeout is expired */
      if(etimer_expired(&gJoinSuccessfulTimer))
      {
        loc_join_timeout_callback(NULL);
      }
    }
  }
}/* sf_join_output_callback() */


/*============================================================================*/
/**
 * \brief Let the app knows that TSCH is leaving the network.
 */
/*============================================================================*/
void sf_tsch_leaving_network_callback( void )
{
  LOG_INFO("TSCH left network\n");

  if(E_CONFIGMGMT_DEVICESTATUS_CONNECTED == sf_configMgmt_getDeviceStatus())
  {
    sf_configMgmt_setDeviceStatus(E_CONFIGMGMT_DEVICESTATUS_NOT_CONNECTED);
    sf_joinRequester_rejoin();
  }
}

/*=============================================================================
                          INDICATIONS IMPLEMENTATION
=============================================================================*/
/*------------------------------------------------------------------------------
  sf_join_response_receive()
------------------------------------------------------------------------------*/
void sf_join_response_receive(void)
{
  /* Response frame parameters. */
  joinResponse_t resFrameParams = {0};
  /* Assigned device address by the gw. */
  linkaddr_t assignedDeviceAddr = {0};

  /* Set the device MAC address to the attributed ID and schedule data slots
      as the device has successfully joined the network. */
  resFrameParams = sf_joinFramer_getResponseParams();
  UINT16_TO_UINT8(assignedDeviceAddr.u8, resFrameParams.deviceId);
  linkaddr_set_node_addr((linkaddr_t*)&assignedDeviceAddr);

  LOG_INFO("Rx resp; New device address;");
  LOG_INFO_LLADDR(&assignedDeviceAddr);
  LOG_INFO_("\n");

  /* Stop state timoeout */
  ctimer_stop(&gJoinResponseTimer);

  /* Set next state to E_JOIN_STATE_SUCC_TX */
  sf_stateManager_setState(&gJoinStateCtx, (uint8_t)E_JOIN_STATE_SUCC_TX);
  sf_stateManager_execState(&gJoinStateCtx);
}/* sf_join_response_receive() */

#ifdef __cplusplus
}
#endif
