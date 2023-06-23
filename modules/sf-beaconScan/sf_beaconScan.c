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
 @brief      Implementation of the beacon scan interface.
*/

/*==============================================================================
                            INCLUDES
==============================================================================*/
/* Standard include */
#include <string.h>
/* Stack specific include */
#include "dev/radio.h"
#include "dev/leds.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "net/mac/framer/framer-802154.h"
#include "net/mac/tsch/tsch.h"
#if CONTIKI_TARGET_COOJA
#include "random.h"
#endif
/* Application specific includes */
#include "sf_configMgmt.h"
#include "sf_rfSettings.h"
#include "sf_beaconScan.h"
/* Log configuration */
#include "sys/log.h"

/*==============================================================================
                            DEFINES
==============================================================================*/
#define LOG_MODULE "Beacon Scan"
/* Defines log level*/
#ifndef LOG_CONF_APP
  #define LOG_LEVEL     LOG_LEVEL_NONE
#else
  #define LOG_LEVEL     LOG_CONF_APP
#endif
/* Defines beacon channels to be scanned */
#if SF_RF_REGION_GLOB2G4_ENABLE
#define SF_BEACON_SCAN_HOPPING_SEQUENCE  GLOB2G4_TSCH_CONF_JOIN_HOPPING_SEQUENCE
#else
#error "Pls enable GLOB2G4 RF region!"
#endif
/* Duration of beacon scan. */
#ifndef SF_CONF_BEACON_SCAN_DURATION
#define SF_BEACON_SCAN_DURATION          (30 * CLOCK_SECOND)
#else
#define SF_BEACON_SCAN_DURATION          (SF_CONF_BEACON_SCAN_DURATION * CLOCK_SECOND)
#endif
/* Defines the scan time for one channel. */
#define SF_BEACON_SCAN_CHANNEL_TIME      (5 * CLOCK_SECOND)

/*==============================================================================
                            STRUCTS
==============================================================================*/
/* Received beacon information */
typedef struct
{
  /* Timestamp when beacon has been received */
  uint32_t timestampOfRx;
  /* Beacon source address */
  linkaddr_t src_addr;
  /* Beacon source PAN ID */
  uint16_t src_pid;
  /* Channel on which beacon has been received */
  uint8_t channel;
  /* RSSI value */
  int rssi;
  /* Join beacon flag */
  E_SF_JOIN_BEACON_t joinBeaconFlag;
}sf_beaconInfo_t;

/*==============================================================================
                            GLOBAL VARIABLES
==============================================================================*/
/* Received beacon with best RSSI */
static sf_beaconInfo_t gBestBeacon = {0};
/* Stores accepted beacon type.
   E_SF_JOIN_BEACON_ENABLED: Manual join is allowed
   E_SF_JOIN_BEACON_DISABLED: Manual join is not allowed */
static E_SF_JOIN_BEACON_t gBeaconType = E_SF_JOIN_BEACON_DISABLED;

/*==============================================================================
                         LOCAL FUNCTION DECLARATION
==============================================================================*/
/*============================================================================*/
/**
 * \brief Parses beacon information from the given packet.
 *
 * \param pInput_eb            Pointer to the beacon packet.
 * \param output_eb            Pointer to parsed beacon data.
 *
 * \return true if everything is ok
 *         false otherwise
 */
/*============================================================================*/
static bool loc_parseBeaconPacket(const struct input_packet *pInput_eb,
                                  sf_beaconInfo_t *output_eb);

/*==============================================================================
                         LOCAL FUNCTION DEFINITION
==============================================================================*/
/*----------------------------------------------------------------------------*/
/*! loc_parseBeaconPacket */
/*----------------------------------------------------------------------------*/
static bool loc_parseBeaconPacket(const struct input_packet *pInput_eb,
                                  sf_beaconInfo_t *output_eb)
{
  bool parseStatus = false;

  if(pInput_eb && output_eb)
  {
    frame802154_t frame;
    struct ieee802154_ies ies;
    uint8_t hdrlen;

    if(tsch_packet_parse_eb(pInput_eb->payload, pInput_eb->len,
                            &frame, &ies, &hdrlen, 0))
    {
      linkaddr_copy(&output_eb->src_addr, (linkaddr_t*)frame.src_addr);
      memcpy(&output_eb->src_pid, &frame.src_pid, sizeof(uint16_t));

      output_eb->joinBeaconFlag = ies.ie_join_mode;
      parseStatus = true;
    }
  }

  return parseStatus;
}/* loc_parseBeaconPacket() */

/*==============================================================================
                                  THREADS
==============================================================================*/
/*----------------------------------------------------------------------------*/
/*! beaconScanning_process */
/*----------------------------------------------------------------------------*/
PT_THREAD(beacon_scan_process(struct pt *pt))
{
  /* Received beacon packet */
  static struct input_packet inputEb;
  /* Timer to measure duration of scanning */
  static struct etimer channelScanTimer;
  /* Timer to count individual channel scan time */
  static struct etimer individualChannelScanTimer;
  /* Number of accepted beacons */
  static uint8_t beaconCounter;
  /* Last received beacon */
  static sf_beaconInfo_t lastBeacon;
  /* Beacon channel */
  static uint8_t scanCounter;
  static uint8_t channelIdx;
  /* Channels which should be scanned */
  static uint8_t scanChannels[] = SF_BEACON_SCAN_HOPPING_SEQUENCE;
  /* Network ID */
  static uint16_t panId = 0;
  /* BMSCC short address */
  linkaddr_t bmsAddr = linkaddr_null;

  PT_BEGIN(pt);

  /* Disable TSCH beacon scan */
  tsch_scan_disable();
  /* Switch off MAC */
  NETSTACK_MAC.off();
  /* Turn radio off */
  NETSTACK_RADIO.off();
  NETSTACK_RADIO.init();

  /* Initialised best beacon information. */
  gBestBeacon.rssi = INT16_MIN;
  linkaddr_copy(&gBestBeacon.src_addr, &linkaddr_null);
  /* Initialized operational variables */
  beaconCounter = 0;
  scanCounter = 0;
  channelIdx = 0;

  etimer_set(&individualChannelScanTimer, SF_BEACON_SCAN_CHANNEL_TIME);
  etimer_set(&channelScanTimer, SF_BEACON_SCAN_DURATION);

  LOG_INFO("Scan for beacons ");
  if(E_SF_JOIN_BEACON_ENABLED == gBeaconType)
  {
    LOG_INFO_("with manual join flag.\n");
  }
  else if(E_SF_JOIN_BEACON_DISABLED == gBeaconType)
  {
    LOG_INFO_("without manual join flag.\n");
  }

  while(!etimer_expired(&channelScanTimer))
  {
    /* Calculate next beacon channel. */
    channelIdx = scanCounter % 3U;
    scanCounter = scanCounter + 1U;

    LOG_INFO("Scan channel %u: \n", scanChannels[channelIdx]);

    /* Set scanned channel */
    NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, scanChannels[channelIdx]);

    /* Clear radio buffer, if a old packet was not processed. */
    NETSTACK_RADIO.read(inputEb.payload, TSCH_PACKET_MAX_LEN);

    /* Turn radio on and wait for EB */
    NETSTACK_RADIO.on();

    while(!etimer_expired(&individualChannelScanTimer))
    {
      /* Wait until any event. */
      PT_YIELD(pt);

      int isPacketPending = NETSTACK_RADIO.pending_packet();

      while(isPacketPending > 0)
      {
        /* Read packet */
        inputEb.len = NETSTACK_RADIO.read(inputEb.payload, TSCH_PACKET_MAX_LEN);

        if(inputEb.len > 0)
        {
          /* Parse timestamp and source address from beacon */
          if(true == loc_parseBeaconPacket(&inputEb, &lastBeacon))
          {
            /* Store channel on which beacon has been received */
            lastBeacon.channel = scanChannels[channelIdx];

            /* Get RSSI of received packet */
#if !CONTIKI_TARGET_COOJA
            NETSTACK_RADIO.get_value(RADIO_PARAM_LAST_RSSI, &lastBeacon.rssi);
#else
            lastBeacon.rssi = -(random_rand() % 30);
#endif

            LOG_INFO("Rx beacon: ");
            LOG_INFO_LLADDR(&lastBeacon.src_addr);
            LOG_INFO_("; %02X, %u; %d; %u;\n", lastBeacon.src_pid, lastBeacon.channel,
                      lastBeacon.rssi, lastBeacon.joinBeaconFlag);

            /* Check the beacon flag. */
            if(gBeaconType == lastBeacon.joinBeaconFlag)
            {
              if(E_SF_JOIN_BEACON_DISABLED == gBeaconType)
              {
                sf_configMgmt_getParam(&panId, sizeof(panId),
                                       E_CONFIGMGMT_PARAM_PAN_ID);
                sf_configMgmt_getParam(bmsAddr.u8, sizeof(bmsAddr.u8),
                                       E_CONFIGMGMT_PARAM_GW_ADDR);

                if(panId != lastBeacon.src_pid || !linkaddr_cmp(&bmsAddr, &lastBeacon.src_addr))
                {
                  isPacketPending = NETSTACK_RADIO.pending_packet();
                  continue;
                }
              }

              beaconCounter++;

              /* Store beacon with best RSSI */
              if(lastBeacon.rssi > gBestBeacon.rssi)
              {
                gBestBeacon = lastBeacon;
              }
            }
          }
          else
          {
            LOG_INFO("Packet was not beacon!\n");
          }
        }

        isPacketPending = NETSTACK_RADIO.pending_packet();
      }
    }

    /* Turn radio off */
    NETSTACK_RADIO.off();

    etimer_set(&individualChannelScanTimer, SF_BEACON_SCAN_CHANNEL_TIME);
  }

  /* Check if valid beacons are received */
  if(beaconCounter == 0)
  {
    /* Set source address of gBestBeacon to null */
    linkaddr_copy(&gBestBeacon.src_addr, &linkaddr_null);
    LOG_INFO("No beacons rx\n");
  }
  else
  {
    LOG_INFO("Rx %u beacon packets\n", beaconCounter);
    LOG_INFO("Best RSSI from ");
    LOG_INFO_LLADDR(&gBestBeacon.src_addr);
    LOG_INFO_(": %d dBm\n", gBestBeacon.rssi);
  }

  /* Set fixed address for TSCH beacon scan. */
  tsch_set_beaconScan_addr(&gBestBeacon.src_addr);

  /* Set static beacon channel. */
  tsch_set_static_beacon_channel(gBestBeacon.channel);

  /* Turn off radio after channel scan */
  NETSTACK_RADIO.off();

  PT_END(pt);
}/* beaconScanning_process() */

/*==============================================================================
                      API FUNCTION IMPLEMENTATION
==============================================================================*/
/*------------------------------------------------------------------------------
  sf_beaconScan_setBeaconType()
------------------------------------------------------------------------------*/
void sf_beaconScan_setBeaconType(E_SF_JOIN_BEACON_t beaconType)
{
  gBeaconType = beaconType;
}/* sf_beaconScan_setBeaconType() */

/*----------------------------------------------------------------------------*/
/*! sf_beaconScan_getBestBeaconAddress */
/*----------------------------------------------------------------------------*/
linkaddr_t* sf_beaconScan_getBestBeaconAddress(void)
{
  return &gBestBeacon.src_addr;
}/* sf_beaconScan_getBestBeaconAddress() */

#ifdef __cplusplus
}
#endif
