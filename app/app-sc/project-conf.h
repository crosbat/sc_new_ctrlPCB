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
 @brief      Project configurations.
*/

#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

/*****************************************************************************/
/********************************** VERSION **********************************/
/*****************************************************************************/
/* Define software version. */
#if !defined(SF_SC_VERSION_MAJOR) || !defined(SF_SC_VERSION_MINOR) ||\
    !defined(SF_SC_VERSION_PATCH)
#error "Please define a valid firmware version!"
#endif

/**
 *  Smart cell Version.
 */
#define SF_SC_VERSION                           {SF_SC_VERSION_MAJOR,\
                                                 SF_SC_VERSION_MINOR,\
                                                 SF_SC_VERSION_PATCH}

/*****************************************************************************/
/****************************** Configure Radio ******************************/
/*****************************************************************************/

/**
 *  Default timeslot timing.
 */
#define TSCH_CONF_DEFAULT_TIMESLOT_TIMING        tsch_timeslot_timing_us_7500

/**
 *  Path to timeslot Header file.
 */
#define TSCH_CONF_ARCH_HDR_PATH                  "sf-tsch-timeslot.h"

/**
 *  Sets the RF to a CRC of 16-bit in Prop-mode.
 *         0 => 32-bit CRC.
 *         1 => 16-bit CRC.
 */
#define PROP_MODE_CONF_USE_CRC16                 1

/**
 *  Enables IEEE802.15.4g mode.
 *         0 => Disabled
 *         1 => Enabled
 */
#define PROP_MODE_CONF_DOT_15_4G                 0

/** Disable CCA */
#define TSCH_CONF_CCA_ENABLED                    0

/** Set radio bitrate */
#define SF_CONF_RADIO_BIT_RATE                   250

/* Enable EU region*/
#define SF_RF_REGION_GLOB2G4_ENABLE              1

#include "sf_rfSettings.h"

/*****************************************************************************/
/***************************** Configure TSCH ********************************/
/*****************************************************************************/
/** Enable one time packet transmission. */
#define NULLNET_SINGLE_PACKET_ENABLED             1

/** Set Keep alive timeout. */
#define TSCH_CONF_KEEPALIVE_TIMEOUT               0
/** Disable keep alive algorithm. */
#define TSCH_KEEPALIVE_ENABLE                     0

/** Set the link address size to 2 bytes */
#define LINKADDR_CONF_SIZE                        2

#ifndef WITH_SECURITY
/** Set to enable TSCH security */
#define WITH_SECURITY                             0
#endif /* WITH_SECURITY */

/** Disable uIP and IPV6 */
#define UIP_CONF_ENABLE                           0

/** Disable routing */
#define ROUTING_CONF_ENABLE                       0

/** Enable TSCH
 * Note: if TSCH should be disabled, also comment out
 * 'MAKE_MAC = MAKE_MAC_TSCH'in the makefile */
#define TSCH_ENABLED                              1

/**
 * Enabled TSCH get lock while loop wait hardfault.
 * If get lock waits to long block in a while 1 loop.
 * This will give the option to raise the watchdog to
 * reset the device.
 */
#define TSCH_GET_LOCK_HARDFAULT_BLOCK_ENABLED     1

/* USB serial takes space, free more space elsewhere */

/** Enables sicslowpan fragmentation. */
#define SICSLOWPAN_CONF_FRAG                      0
/** Configuration buffer size. */
#define UIP_CONF_BUFFER_SIZE                      160

/** IEEE802.15.4 PANID */
#define IEEE802154_CONF_PANID                     0xABCD

/** Do not start TSCH at init, wait for NETSTACK_MAC.on() */
#define TSCH_CONF_AUTOSTART                       0

/** Disable the 6TiSCH minimal schedule */
#define TSCH_SCHEDULE_CONF_WITH_6TISCH_MINIMAL    0

/** Disable destination address in ACKs */
#define TSCH_PACKET_CONF_EACK_WITH_DEST_ADDR      0

/** Disable destination address in ACKs */
#define TSCH_PACKET_CONF_EACK_WITH_SRC_ADDR       0

/** Disable ACK/NAK time correction IE in EACKS */
#define TSCH_PACKET_CONF_EACK_WITH_ACK_NACK_TCORR 0


/* Schedule selection */
#define SCHEDULE_A                                1
#define SCHEDULE_B                                0
#define SCHEDULE_C                                0

#if !SCHEDULE_A && !SCHEDULE_B && !SCHEDULE_A
#error "A schedule needs to be selected"
#endif

#if (SCHEDULE_A && SCHEDULE_B) || (SCHEDULE_A && SCHEDULE_C) || (SCHEDULE_B && SCHEDULE_C)
#error "Only one schedule can be selected"
#endif /* #if (SCHEDULE_A && SCHEDULE_B) || (SCHEDULE_A && SCHEDULE_C) || (SCHEDULE_B && SCHEDULE_C) */


/** Put all cells on the same slotframe */
#define APP_SLOTFRAME_HANDLE                      1

#if SCHEDULE_A
/** Size of the Slotframe Section */
#define APP_SLOTFRAME_SECTION_SIZE                425
#endif /* #if SCHEDULE_A */

/** Reduces beacons per Slotframe by this number.
 *  Note: Also adapt SF_FW_UPDATE_INDICATION_TIME in the project-conf.h on
 *  BMSCC side. */
#define APP_SLOTFRAME_BEACON_DIV                  1

/** Number of Sections */
#define APP_SLOTFRAME_SECTION_NUM                 1

/** Size of the Slotframe */
#define APP_SLOTFRAME_SIZE                        (APP_SLOTFRAME_SECTION_SIZE *\
                                                   APP_SLOTFRAME_SECTION_NUM)

/** Number of Beacon Slots per Slotframe section */
#define APP_SLOTFRAME_SECTION_BEACON_SLOTS        1

/** Number of Join Request Slots per Slotframe section */
#define APP_SLOTFRAME_SECTION_JOIN_REQUEST_SLOTS  12

/** Number of Join Process Slots per Slotframe section */
#define APP_SLOTFRAME_SECTION_JOIN_PROCESS_SLOTS  12

/** Number of Join Slots per Slotframe section */
#define APP_SLOTFRAME_SECTION_JOIN_SLOTS          (APP_SLOTFRAME_SECTION_JOIN_REQUEST_SLOTS +\
                                                   APP_SLOTFRAME_SECTION_JOIN_PROCESS_SLOTS)

#if SCHEDULE_A
/** Number of Device Tx Slots per Slotframe section */
#define APP_SLOTFRAME_SECTION_DEVICE_TX_SLOTS     100

/** Number of Device Rx Slots per Slotframe section */
#define APP_SLOTFRAME_SECTION_DEVICE_RX_SLOTS     100
#endif /* #if SCHEDULE_A */

/** Maximum number of devices */
#define APP_MAX_DEVICES                           100

/* Maximum number of device slots */
#define APP_MAX_DEVICE_SLOTS                      0

/** Number of links that can be used by the device (10 + 240 Join + 2*APP_MAX_DEVICES Tx + APP_MAX_DEVICES Rx)*/
#define TSCH_SCHEDULE_CONF_MAX_LINKS              (APP_SLOTFRAME_SECTION_NUM + \
                                                   (APP_SLOTFRAME_SECTION_NUM * \
                                                   APP_SLOTFRAME_SECTION_JOIN_SLOTS) \
                                                   + 3 + APP_MAX_DEVICE_SLOTS)

/** Allow also to join other PAN IDs */
#define TSCH_CONF_JOIN_MY_PANID_ONLY              0

/** Disable beacons */
#define TSCH_CONF_EB_PERIOD                       0

/** The beacon transmit period in seconds */
#define TSCH_CONF_BEACON_SEND_PERIOD              3

/** Max time without synchronization before leaving the PAN */
#define TSCH_CONF_DESYNC_THRESHOLD               (4 * (60 * CLOCK_SECOND))

/** Maximum allowed absolute time (AT) drift between beacon AT and internal AT */
#define TSCH_CONF_MAX_AT_DRIFT                   2

/** Defines the callback function called by TSCH when leaving a network. */
#define TSCH_CALLBACK_LEAVING_NETWORK            sf_tsch_leaving_network_callback

/** Define a callback function called by TISCH at new slot cycle. */
#define TSCH_CALLBACK_CYCLE_START                sf_measSender_cycleStartCallback

#if WITH_SECURITY
/** Enable security */
#define LLSEC802154_CONF_ENABLED                 1
/* Do not use explicit security levels */

/** Enable AUX Header. */
#define LLSEC802154_CONF_USES_AUX_HEADER         0
/** Enable Implicit Security level. */
#define LLSEC802154_CONF_USES_IMPLICIT_SECURITY  1

/* Platform-specific (H/W) CCM* implementation */
#ifndef CCM_STAR_CONF
#if CONTIKI_TARGET_COOJA
/** Platform-specific (H/W) CCM* implementation. */
#define CCM_STAR_CONF                            ccm_star_driver
#else
/** Platform-specific (H/W) CCM* implementation. */
#define CCM_STAR_CONF                            hw_ccm_star_driver
#endif
#endif /* CCM_STAR_CONF */

/** Shared secrets Number. */
#define TSCHSECURITY_SHAREDSECRETS_NUM           1

/** Key used for Other (Data, Cmd) */
#define TSCH_SECURITY_CONF_KEY_INDEX_OTHER       3
#endif /* WITH_SECURITY */


/**
 * Location of the IEEE address.
 *         0 => Read from InfoPage.
 *         1 => Use a hardcoded address, configured by IEEE_ADDR_CONF_ADDRESS.
 */
#define IEEE_ADDR_CONF_HARDCODED                 0

/**
 * The hardcoded IEEE address to be used when IEEE_ADDR_CONF_HARDCODED
 * is defined as 1. Must be a byte array of size 8 (long) or 2 (short).
 */
#if LINKADDR_SIZE == 8
/** IEEE Configuration Adress. */
#define IEEE_ADDR_CONF_ADDRESS                   { 0x00, 0x12, 0x4B, 0x00, 0x89, 0xAB, 0xCD, 0xEF }
#else
/** IEEE Configuration Adress. */
#define IEEE_ADDR_CONF_ADDRESS                   { 0x7a, 0x40 }
#endif /* #if LINKADDR_SIZE == 8 */

/** Size of packetbuf module buffer */
#define PACKETBUF_CONF_SIZE                      64

/** Size of queuebuf module buffer */
#define QUEUEBUF_CONF_NUM                        8

/** Maximum count of tsch neighbors */
#define NBR_TABLE_CONF_MAX_NEIGHBORS             4

/*******************************************************/
/************* Other system configuration **************/
/*******************************************************/
/** Defines if SPI is enabled. */
#define TI_SPI_CONF_ENABLE                       0
/** Enables I2C for cooja targets. */
#define TI_I2C_CONF_ENABLE                       0

/* UART Enabled */
/** Enables CC26XX UART. */
#define CC26XX_UART_CONF_ENABLE                  1
/** Enables TI UART. */
#define TI_UART_CONF_ENABLE                      1
/** Disables UART initialisation if UART pins are disconnected. */
#if !CONTIKI_TARGET_COOJA
#define UART_DEPENDS_ON_UART_CONNECTION          0
#else
#define UART_DEPENDS_ON_UART_CONNECTION          0
#endif
#if !CONTIKI_TARGET_COOJA
#define UART_SENSOR_PULLDOWN_AFTER_INIT          0
#else
#define UART_SENSOR_PULLDOWN_AFTER_INIT          0
#endif

#define RF_TEST_APP_WD_DEBUG_DISABLE             1

/* Logging */

/** Log level for RPL. */
#define LOG_CONF_LEVEL_RPL                       LOG_LEVEL_NONE
/** Log level for TCP IP. */
#define LOG_CONF_LEVEL_TCPIP                     LOG_LEVEL_NONE
/** Log level for IPv6. */
#define LOG_CONF_LEVEL_IPV6                      LOG_LEVEL_NONE
/** Log level for 6LOWPAN. */
#define LOG_CONF_LEVEL_6LOWPAN                   LOG_LEVEL_NONE
/** Log level for MAC. */
#define LOG_CONF_LEVEL_MAC                       LOG_LEVEL_INFO
/** Log level for MAC Schedule. */
#define LOG_CONF_LEVEL_MAC_SCHED                 LOG_LEVEL_INFO
/** Log level for null net. */
#define LOG_CONF_LEVEL_NULLNET                   LOG_LEVEL_INFO
/** Log level for Main. */
#define LOG_CONF_LEVEL_MAIN                      LOG_LEVEL_INFO
/** Log Level for Radio. */
#define LOG_CONF_LEVEL_RADIO                     LOG_LEVEL_NONE
/** Log Configuration per slot. */
#define TSCH_LOG_CONF_PER_SLOT                   1
/** The application LOG level. */
#define LOG_CONF_APP                             LOG_LEVEL_INFO
/** The application join LOG level. */
#define LOG_CONF_JOIN                            LOG_LEVEL_INFO

/** Enables LED. */
#define LED_ENABLED                              1

// #define DEBUG_TSCH_TIMING                        1
// #define DEBUG_TSCH_TIMING_TX                     1

#ifdef DEBUG_TSCH_TIMING
/* TSCH Timing Debug */
#undef TSCH_TIMING_PIN_0
#define TSCH_TIMING_PIN_0                   IOID_12

#undef TSCH_TIMING_PIN_1
#define TSCH_TIMING_PIN_1                   IOID_15

#undef TSCH_TIMING_PIN_2
#define TSCH_TIMING_PIN_2                   IOID_21

#undef TSCH_TIMING_PIN_3
#define TSCH_TIMING_PIN_3                   IOID_22

#undef TSCH_TIMING_PIN_4
#define TSCH_TIMING_PIN_4                   IOID_0


#define TSCH_DEBUG_SLOT_CUSTOM_START()      gpio_hal_arch_set_pin(NULL, TSCH_TIMING_PIN_4)
#define TSCH_DEBUG_SLOT_CUSTOM_END()        gpio_hal_arch_clear_pin(NULL, TSCH_TIMING_PIN_4)

#define TSCH_DEBUG_SLOT_SLOT_START()        gpio_hal_arch_set_pin(NULL, TSCH_TIMING_PIN_0)
#define TSCH_DEBUG_SLOT_SLOT_END()          gpio_hal_arch_clear_pin(NULL, TSCH_TIMING_PIN_0)

#if DEBUG_TSCH_TIMING_RX
#define TSCH_DEBUG_RX_SLOT_SLOT_START()     gpio_hal_arch_set_pin(NULL, TSCH_TIMING_PIN_1)
#define TSCH_DEBUG_RX_SLOT_SLOT_END()       gpio_hal_arch_clear_pin(NULL, TSCH_TIMING_PIN_1)

#define TSCH_DEBUG_RX_SLOT_RX_START()       gpio_hal_arch_set_pin(NULL, TSCH_TIMING_PIN_2)
#define TSCH_DEBUG_RX_SLOT_RX_END()         gpio_hal_arch_clear_pin(NULL, TSCH_TIMING_PIN_2)

#define TSCH_DEBUG_RX_SLOT_ACK_SEND_START() gpio_hal_arch_set_pin(NULL, TSCH_TIMING_PIN_3)
#define TSCH_DEBUG_RX_SLOT_ACK_SEND_END()   gpio_hal_arch_clear_pin(NULL, TSCH_TIMING_PIN_3)
#endif /* #if DEBUG_TSCH_TIMING_RX */

#if DEBUG_TSCH_TIMING_TX
#define TSCH_DEBUG_TX_SLOT_SLOT_START()     gpio_hal_arch_set_pin(NULL, TSCH_TIMING_PIN_1)
#define TSCH_DEBUG_TX_SLOT_SLOT_END()       gpio_hal_arch_clear_pin(NULL, TSCH_TIMING_PIN_1)

#define TSCH_DEBUG_TX_SLOT_TX_START()       gpio_hal_arch_set_pin(NULL, TSCH_TIMING_PIN_2)
#define TSCH_DEBUG_TX_SLOT_TX_END()         gpio_hal_arch_clear_pin(NULL, TSCH_TIMING_PIN_2)

#define TSCH_DEBUG_TX_SLOT_ACK_WAIT_START() gpio_hal_arch_set_pin(NULL, TSCH_TIMING_PIN_3)
#define TSCH_DEBUG_TX_SLOT_ACK_WAIT_END()   gpio_hal_arch_clear_pin(NULL, TSCH_TIMING_PIN_3)
#endif /* #if DEBUG_TSCH_TIMING_TX */
#endif /* #if DEBUG_TSCH_TIMING */

#endif /* PROJECT_CONF_H_ */
