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
 @brief      Holds rf settings of each rf region.
*/

#ifndef _SF_RF_SETTINGS_H_
#define _SF_RF_SETTINGS_H_

/*******************************************************/
/******************* Configure Radio *******************/
/*******************************************************/

/**
 *  Radio bitrate. Default is 50kbps.
 */
#ifndef SF_CONF_RADIO_BIT_RATE
#define SF_RADIO_BIT_RATE  50
#else
#define SF_RADIO_BIT_RATE  SF_CONF_RADIO_BIT_RATE
#endif

/**
 *  Used to modify symbolrate defined in smartrf-settings.c
 *  Set 'symbolRate.rateWord' in smartrf-settings.c to
 *  SF_RADIO_BIT_RATE_WORD
 */
#define SF_RADIO_BIT_RATE_WORD ((0x4000 * SF_RADIO_BIT_RATE) / 25)

/**
 *  Sets the RSSI RX threshold in dBm in Prop-mode.
 *         Defaults to -110 dBm.
 */
#define PROP_MODE_CONF_CCA_RSSI_THRESHOLD      0x90

/**
 *  Sets the RSSI RX threshold in dBm in Prop-mode.
 *         Defaults to -110 dBm.
 */
#define PROP_MODE_CONF_CCA_RX_RSSI_THRESHOLD   0x90

/**
 *  Frequency band configuration
 */

/* EU */
/** Channel minimum. */
#define DOT_15_4G_EU_CHAN_MIN                  0
/** Channel maximum. */
#define DOT_15_4G_EU_CHAN_MAX                  14
/** Frequency spacing. */
#define DOT_15_4G_EU_FREQ_SPACING              200
/** Frequency of Channel 0. */
#define DOT_15_4G_EU_CHAN0_FREQ                865100
/** Center Frequency. */
#define PROP_MODE_EU_CONF_CENTER_FREQ          0x0362
/** LO divider. */
#define PROP_MODE_EU_CONF_LO_DIVIDER           0x05
/**
 *  Sets the RSSI threshold in dBm in Prop-mode. Defaults to -110 dBm.
 */
#define RF_REGION_EU_CCA_RSSI_THRESHOLD        0x90

/* US */
/** Channel minimum. */
#define DOT_15_4G_US_CHAN_MIN                  0
/** Channel maximum. */
#define DOT_15_4G_US_CHAN_MAX                  63
/** Frequency spacing. */
#define DOT_15_4G_US_FREQ_SPACING              200
/** Frequency of Channel 0. */
#define DOT_15_4G_US_CHAN0_FREQ                915200
/** Center Frequency. */
#define PROP_MODE_US_CONF_CENTER_FREQ          0x0399
/** LO divider. */
#define PROP_MODE_US_CONF_LO_DIVIDER           0x05
/**
 *  Sets the RSSI threshold in dBm in Prop-mode. Defaults to -110 dBm.
 */
#define RF_REGION_US_CCA_RSSI_THRESHOLD        0x90

/* CHN */
/** Channel minimum. */
#define DOT_15_4G_CHN_CHAN_MIN                 0
/** Channel maximum. */
#define DOT_15_4G_CHN_CHAN_MAX                 2
/** Frequency spacing. */
#define DOT_15_4G_CHN_FREQ_SPACING             200
/** Frequency of Channel 0. */
#define DOT_15_4G_CHN_CHAN0_FREQ               868100
/** Center Frequency. */
#define PROP_MODE_CHN_CONF_CENTER_FREQ         0x0364
/** LO divider. */
#define PROP_MODE_CHN_CONF_LO_DIVIDER          0x05
/**
 *  Sets the RSSI threshold in dBm in Prop-mode. Defaults to -110 dBm.
 */
#define RF_REGION_CHN_CCA_RSSI_THRESHOLD        0x90

/* SK */
/** Channel minimum. */
#define DOT_15_4G_SK_CHAN_MIN                  0
/** Channel maximum. */
#define DOT_15_4G_SK_CHAN_MAX                  4
/** Frequency spacing. */
#define DOT_15_4G_SK_FREQ_SPACING              200
/** Frequency of Channel 0. */
#define DOT_15_4G_SK_CHAN0_FREQ                922300
/** Center Frequency. */
#define PROP_MODE_SK_CONF_CENTER_FREQ          0x039A
/** LO divider. */
#define PROP_MODE_SK_CONF_LO_DIVIDER           0x05
/**
 *  Sets the RSSI threshold in dBm in Prop-mode. Defaults to -110 dBm.
 */
#define RF_REGION_SK_CCA_RSSI_THRESHOLD        0x90

/* IN */
/** Channel minimum. */
#define DOT_15_4G_IN_CHAN_MIN                  0
/** Channel maximum. */
#define DOT_15_4G_IN_CHAN_MAX                  9
/** Frequency spacing. */
#define DOT_15_4G_IN_FREQ_SPACING              200
/** Frequency of Channel 0. */
#define DOT_15_4G_IN_CHAN0_FREQ                865100
/** Center Frequency. */
#define PROP_MODE_IN_CONF_CENTER_FREQ          0x0361
/** LO divider. */
#define PROP_MODE_IN_CONF_LO_DIVIDER           0x05
/**
 *  Sets the RSSI threshold in dBm in Prop-mode. Defaults to -110 dBm.
 */
#define RF_REGION_IN_CCA_RSSI_THRESHOLD        0x90

/* JP */
/** Channel minimum. */
#define DOT_15_4G_JP_CHAN_MIN                  0
/** Channel maximum. */
#define DOT_15_4G_JP_CHAN_MAX                  28
/** Frequency spacing. */
#define DOT_15_4G_JP_FREQ_SPACING              200
/** Frequency of Channel 0. */
#define DOT_15_4G_JP_CHAN0_FREQ                922400
/** Center Frequency. */
#define PROP_MODE_JP_CONF_CENTER_FREQ          0x039D
/** LO divider. */
#define PROP_MODE_JP_CONF_LO_DIVIDER           0x05
/**
 *  Sets the RSSI threshold in dBm in Prop-mode. Defaults to -80 dBm.
 */
#define RF_REGION_JP_CCA_RSSI_THRESHOLD        0xB0

/* Global 2.4GHz  */
/** Channel minimum. */
#define DOT_15_4G_GLOB2G4_CHAN_MIN             11
/** Channel maximum. */
#define DOT_15_4G_GLOB2G4_CHAN_MAX             26
/** Frequency spacing. */
#define DOT_15_4G_GLOB2G4_FREQ_SPACING         5000
/** Frequency of Channel 0. */
#define DOT_15_4G_GLOB2G4_CHAN0_FREQ           2405000
/**
 *  Sets the RSSI threshold in dBm. Defaults to -90 dBm.
 */
#define RF_REGION_GLOB2G4_CCA_RSSI_THRESHOLD   0xA6

/* Default frequency band parameters. */
/** Frequency Band ID. */
#define DOT_15_4G_CONF_FREQ_BAND_ID         DOT_15_4G_FREQ_BAND_CUSTOM
/** Channel minimum. */
#define DOT_15_4G_CHAN_MIN                  DOT_15_4G_EU_CHAN_MIN
/** Channel maximum. */
#define DOT_15_4G_CHAN_MAX                  DOT_15_4G_EU_CHAN_MAX
/** Frequency spacing. */
#define DOT_15_4G_FREQ_SPACING              DOT_15_4G_EU_FREQ_SPACING
/** Frequency of Channel 0. */
#define DOT_15_4G_CHAN0_FREQ                DOT_15_4G_EU_CHAN0_FREQ
// /** Center Frequency. */
#define PROP_MODE_CONF_CENTER_FREQ          PROP_MODE_EU_CONF_CENTER_FREQ
/** LO divider. */
#define PROP_MODE_CONF_LO_DIVIDER           PROP_MODE_EU_CONF_LO_DIVIDER


/*******************************************************/
/******************* Configure TSCH ********************/
/*******************************************************/

/* Channel hopping sequence  */
/** Set pre-defined default channel. */
#define IEEE802154_CONF_DEFAULT_CHANNEL 0

#ifndef TSCH_CONF_CCA_ENABLED
/** Enable CCA for shared slots */
#define TSCH_CONF_CCA_ENABLED 1
#endif

/* EU */
/** Hopping Sequence 12 12. */
#define EU_TSCH_HOPPING_SEQUENCE_12_12 (uint8_t[]){ 6, 10, 5, 11, 3, 2, 12, 9, 13, 1, 4, 8 }
/** Default Hopping Sequence. */
#define EU_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE EU_TSCH_HOPPING_SEQUENCE_12_12
/** Join Hopping Sequence. */
#define EU_TSCH_CONF_JOIN_HOPPING_SEQUENCE (uint8_t[]){ 0, 7, 14 }

/* US */
/** Hopping Sequence 61 61. */
#define US_TSCH_HOPPING_SEQUENCE_61_61 (uint8_t[]){ 5, 37, 7,  25, 21, 62, 26, 33, 39, 42, 22, 29, 55, 14, 58, 20,\
                                                    13, 40, 45, 43, 61, 23, 46, 10, 52, 51, 38, 4, 19, 50, 35, 56,\
                                                    1,  18, 6,  41, 48, 9,  44, 30, 34, 60, 27, 15, 47, 59, 12, 54,\
                                                    16, 53, 57, 3, 2, 36, 11, 31, 28, 17, 8, 49, 24 }
/** Default Hopping Sequence. */
#define US_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE US_TSCH_HOPPING_SEQUENCE_61_61
/** Join Hopping Sequence. */
#define US_TSCH_CONF_JOIN_HOPPING_SEQUENCE (uint8_t[]){ 0, 32, 63 }

/* China */
/** Hopping Sequence 2 2. */
#define CHN_TSCH_HOPPING_SEQUENCE_2_2 (uint8_t[]){ 2, 1 }
/** Default Hopping Sequence. */
#define CHN_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE CHN_TSCH_HOPPING_SEQUENCE_2_2
/** Join Hopping Sequence. */
#define CHN_TSCH_CONF_JOIN_HOPPING_SEQUENCE (uint8_t[]){ 0 }

/* South Korea */
/** Hopping Sequence 2 2. */
#define SK_TSCH_HOPPING_SEQUENCE_2_2 (uint8_t[]){ 3, 1 }
/** Default Hopping Sequence. */
#define SK_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE SK_TSCH_HOPPING_SEQUENCE_2_2
/** Join Hopping Sequence. */
#define SK_TSCH_CONF_JOIN_HOPPING_SEQUENCE (uint8_t[]){ 0, 2, 4 }

/* India */
/** Hopping Sequence 7 7. */
#define IN_TSCH_HOPPING_SEQUENCE_7_7 (uint8_t[]){ 1, 8, 2, 3, 9, 7, 4 }
/** Default Hopping Sequence. */
#define IN_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE IN_TSCH_HOPPING_SEQUENCE_7_7
/** Join Hopping Sequence. */
#define IN_TSCH_CONF_JOIN_HOPPING_SEQUENCE (uint8_t[]){ 5, 6, 0 }

/* Japan */
/** Hopping Sequence 26 26. */
#define JP_TSCH_HOPPING_SEQUENCE_26_26 (uint8_t[]){ 7, 6, 1, 19, 25, 14, 12, 15, 2, 17, 26, 10, 9, 13, 0, 24,\
                                                    23, 11, 4, 20, 21, 27, 3, 16, 5, 28 }
/** Default Hopping Sequence. */
#define JP_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE JP_TSCH_HOPPING_SEQUENCE_26_26
/** Join Hopping Sequence. */
#define JP_TSCH_CONF_JOIN_HOPPING_SEQUENCE (uint8_t[]){ 18, 8, 22 }

/* Global */
/** Hopping Sequence . */
#define GLOB2G4_TSCH_HOPPING_SEQUENCE_16_16 (uint8_t[]){ 16, 17, 23, 18, 26, 15, 25, 22, 19, 11, 12, 13, 24, 14, 20, 21 }
/** Default Hopping Sequence. */
#define GLOB2G4_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE GLOB2G4_TSCH_HOPPING_SEQUENCE_16_16
/** Join Hopping Sequence. */
#define GLOB2G4_TSCH_CONF_JOIN_HOPPING_SEQUENCE (uint8_t[]){ 18, 11, 22 }

#if !CONTIKI_TARGET_COOJA
/** Default hopping sequence */
#define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE EU_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE
/** Default join hopping sequence */
#define TSCH_CONF_JOIN_HOPPING_SEQUENCE EU_TSCH_CONF_JOIN_HOPPING_SEQUENCE
/** Maximum hopping sequence lengt. Hopping sequence with the maximum elements. */
#define TSCH_CONF_HOPPING_SEQUENCE_MAX_LEN sizeof(US_TSCH_HOPPING_SEQUENCE_61_61)
#else
/** Default hopping sequence */
#define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE GLOB2G4_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE
/** Default join hopping sequence */
#define TSCH_CONF_JOIN_HOPPING_SEQUENCE GLOB2G4_TSCH_CONF_JOIN_HOPPING_SEQUENCE
/** Maximum hopping sequence lengt. Hopping sequence with the maximum elements. */
#define TSCH_CONF_HOPPING_SEQUENCE_MAX_LEN sizeof(US_TSCH_HOPPING_SEQUENCE_61_61)
#endif

#endif /* _SF_RF_SETTINGS_H_ */
