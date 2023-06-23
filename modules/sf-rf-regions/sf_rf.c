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
 @brief      Implementation of the RF interface
*/

#include <stdint.h>
#include <stdbool.h>

#if !CONTIKI_TARGET_COOJA
#include "dot-15-4g.h"
#endif
#include "net/mac/tsch/tsch.h"
#include "lib/random.h"
#include "sf_rfSettings.h"
#include "sf_rf.h"

/*****************************************************************************/
/*                              MACROS                                       */
/*****************************************************************************/
#if !SF_RF_REGION_EU_ENABLE && !SF_RF_REGION_USCAN_ENABLE && !SF_RF_REGION_CHN_ENABLE \
    && !SF_RF_REGION_JP_ENABLE && !SF_RF_REGION_SK_ENABLE && !SF_RF_REGION_IN_ENABLE \
    && !SF_RF_REGION_GLOB2G4_ENABLE
#error "Pls enable RF region!"
#endif

/*****************************************************************************/
/*                            GLOBAL VARIABLES                               */
/*****************************************************************************/
#if SF_RF_REGION_EU_ENABLE
/* EU frequency band parameter. */
const uint8_t eu_tsch_hopping_sequence[] = EU_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE;
const uint8_t eu_tsch_hopping_sequence_size = sizeof(EU_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE);
const uint8_t eu_tsch_hopping_sequence_adv[] = EU_TSCH_CONF_JOIN_HOPPING_SEQUENCE;
const uint8_t eu_tsch_hopping_sequence_adv_size = sizeof(EU_TSCH_CONF_JOIN_HOPPING_SEQUENCE);

#if !CONTIKI_TARGET_COOJA
const dot_15_4g_radioConf_t eu_dot_15_4g_radioConf = {
  DOT_15_4G_EU_CHAN_MIN,
  DOT_15_4G_EU_CHAN_MAX,
  DOT_15_4G_EU_FREQ_SPACING,
  DOT_15_4G_EU_CHAN0_FREQ,
  PROP_MODE_EU_CONF_CENTER_FREQ,
  PROP_MODE_EU_CONF_LO_DIVIDER
};
#endif
#endif

#if SF_RF_REGION_USCAN_ENABLE
/* US frequency band parameter. */
const uint8_t us_tsch_hopping_sequence[] = US_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE;
const uint8_t us_tsch_hopping_sequence_size = sizeof(US_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE);
const uint8_t us_tsch_hopping_sequence_adv[] = US_TSCH_CONF_JOIN_HOPPING_SEQUENCE;
const uint8_t us_tsch_hopping_sequence_adv_size = sizeof(US_TSCH_CONF_JOIN_HOPPING_SEQUENCE);

#if !CONTIKI_TARGET_COOJA
const dot_15_4g_radioConf_t us_dot_15_4g_radioConf = {
  DOT_15_4G_US_CHAN_MIN,
  DOT_15_4G_US_CHAN_MAX,
  DOT_15_4G_US_FREQ_SPACING,
  DOT_15_4G_US_CHAN0_FREQ,
  PROP_MODE_US_CONF_CENTER_FREQ,
  PROP_MODE_US_CONF_LO_DIVIDER
};
#endif
#endif

#if SF_RF_REGION_CHN_ENABLE
/* CHN frequency band parameter. */
const uint8_t chn_tsch_hopping_sequence[] = CHN_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE;
const uint8_t chn_tsch_hopping_sequence_size = sizeof(CHN_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE);
const uint8_t chn_tsch_hopping_sequence_adv[] = CHN_TSCH_CONF_JOIN_HOPPING_SEQUENCE;
const uint8_t chn_tsch_hopping_sequence_adv_size = sizeof(CHN_TSCH_CONF_JOIN_HOPPING_SEQUENCE);

#if !CONTIKI_TARGET_COOJA
const dot_15_4g_radioConf_t chn_dot_15_4g_radioConf = {
  DOT_15_4G_CHN_CHAN_MIN,
  DOT_15_4G_CHN_CHAN_MAX,
  DOT_15_4G_CHN_FREQ_SPACING,
  DOT_15_4G_CHN_CHAN0_FREQ,
  PROP_MODE_CHN_CONF_CENTER_FREQ,
  PROP_MODE_CHN_CONF_LO_DIVIDER
};
#endif
#endif

#if SF_RF_REGION_JP_ENABLE
/* Japan frequency band parameter. */
const uint8_t jp_tsch_hopping_sequence[] = JP_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE;
const uint8_t jp_tsch_hopping_sequence_size = sizeof(JP_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE);
const uint8_t jp_tsch_hopping_sequence_adv[] = JP_TSCH_CONF_JOIN_HOPPING_SEQUENCE;
const uint8_t jp_tsch_hopping_sequence_adv_size = sizeof(JP_TSCH_CONF_JOIN_HOPPING_SEQUENCE);

#if !CONTIKI_TARGET_COOJA
const dot_15_4g_radioConf_t jp_dot_15_4g_radioConf = {
  DOT_15_4G_JP_CHAN_MIN,
  DOT_15_4G_JP_CHAN_MAX,
  DOT_15_4G_JP_FREQ_SPACING,
  DOT_15_4G_JP_CHAN0_FREQ,
  PROP_MODE_JP_CONF_CENTER_FREQ,
  PROP_MODE_JP_CONF_LO_DIVIDER
};
#endif
#endif

#if SF_RF_REGION_SK_ENABLE
/* South Korea frequency band parameter. */
const uint8_t sk_tsch_hopping_sequence[] = SK_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE;
const uint8_t sk_tsch_hopping_sequence_size = sizeof(SK_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE);
const uint8_t sk_tsch_hopping_sequence_adv[] = SK_TSCH_CONF_JOIN_HOPPING_SEQUENCE;
const uint8_t sk_tsch_hopping_sequence_adv_size = sizeof(SK_TSCH_CONF_JOIN_HOPPING_SEQUENCE);

#if !CONTIKI_TARGET_COOJA
const dot_15_4g_radioConf_t sk_dot_15_4g_radioConf = {
  DOT_15_4G_SK_CHAN_MIN,
  DOT_15_4G_SK_CHAN_MAX,
  DOT_15_4G_SK_FREQ_SPACING,
  DOT_15_4G_SK_CHAN0_FREQ,
  PROP_MODE_SK_CONF_CENTER_FREQ,
  PROP_MODE_SK_CONF_LO_DIVIDER
};
#endif
#endif

#if SF_RF_REGION_IN_ENABLE
/* South Korea frequency band parameter. */
const uint8_t in_tsch_hopping_sequence[] = IN_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE;
const uint8_t in_tsch_hopping_sequence_size = sizeof(IN_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE);
const uint8_t in_tsch_hopping_sequence_adv[] = IN_TSCH_CONF_JOIN_HOPPING_SEQUENCE;
const uint8_t in_tsch_hopping_sequence_adv_size = sizeof(IN_TSCH_CONF_JOIN_HOPPING_SEQUENCE);

#if !CONTIKI_TARGET_COOJA
const dot_15_4g_radioConf_t in_dot_15_4g_radioConf = {
  DOT_15_4G_IN_CHAN_MIN,
  DOT_15_4G_IN_CHAN_MAX,
  DOT_15_4G_IN_FREQ_SPACING,
  DOT_15_4G_IN_CHAN0_FREQ,
  PROP_MODE_IN_CONF_CENTER_FREQ,
  PROP_MODE_IN_CONF_LO_DIVIDER
};
#endif
#endif

#if SF_RF_REGION_GLOB2G4_ENABLE
/* Global 2.4GHz frequency band parameter. */
const uint8_t glob2g4_tsch_hopping_sequence[] = GLOB2G4_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE;
const uint8_t glob2g4_tsch_hopping_sequence_size = sizeof(GLOB2G4_TSCH_CONF_DEFAULT_HOPPING_SEQUENCE);
const uint8_t glob2g4_tsch_hopping_sequence_adv[] = GLOB2G4_TSCH_CONF_JOIN_HOPPING_SEQUENCE;
const uint8_t glob2g4_tsch_hopping_sequence_adv_size = sizeof(GLOB2G4_TSCH_CONF_JOIN_HOPPING_SEQUENCE);

#if !CONTIKI_TARGET_COOJA
const dot_15_4g_radioConf_t glob2g4_dot_15_4g_radioConf = {
  DOT_15_4G_GLOB2G4_CHAN_MIN,
  DOT_15_4G_GLOB2G4_CHAN_MAX,
  DOT_15_4G_GLOB2G4_FREQ_SPACING,
  DOT_15_4G_GLOB2G4_CHAN0_FREQ,
  0,
  0
};
#endif
#endif

/*****************************************************************************/
/*                            API IMPLEMENTATION                             */
/*****************************************************************************/
bool sf_rf_selectRegion(sf_rf_region_t region)
{
  bool regionSelected = false;

  switch (region)
  {
#if SF_RF_REGION_USCAN_ENABLE
    case E_SF_RF_REGION_USCAN:
    {
      /* Set CCA THRESHOLD */
      NETSTACK_RADIO.set_value(RADIO_PARAM_CCA_THRESHOLD, RF_REGION_US_CCA_RSSI_THRESHOLD);

      /* Frequency band USA/Canada. */
      /* Set hopping sequence related to freq. band.  */
      tsch_set_hopping_sequence(us_tsch_hopping_sequence, us_tsch_hopping_sequence_size);
      tsch_set_hopping_sequence_adv(us_tsch_hopping_sequence_adv, us_tsch_hopping_sequence_adv_size);

      #if !CONTIKI_TARGET_COOJA
      gDot_15_4g_radioConf = &us_dot_15_4g_radioConf;
      #endif

      /* Set random beacon channel. */
      uint8_t beaconChannelIndex = random_rand() % us_tsch_hopping_sequence_adv_size;
      tsch_set_static_beacon_channel(us_tsch_hopping_sequence_adv[beaconChannelIndex]);

      tsch_disable_cca();

      regionSelected = true;
      break;
    }
#endif

#if SF_RF_REGION_EU_ENABLE
    case E_SF_RF_REGION_EU:
    {
      /* Set CCA THRESHOLD */
      NETSTACK_RADIO.set_value(RADIO_PARAM_CCA_THRESHOLD, RF_REGION_EU_CCA_RSSI_THRESHOLD);

      /* Frequency band EU. */
      /* Set hopping sequence related to freq. band.  */
      tsch_set_hopping_sequence(eu_tsch_hopping_sequence, eu_tsch_hopping_sequence_size);
      tsch_set_hopping_sequence_adv(eu_tsch_hopping_sequence_adv, eu_tsch_hopping_sequence_adv_size);

      #if !CONTIKI_TARGET_COOJA
      gDot_15_4g_radioConf = &eu_dot_15_4g_radioConf;
      #endif

      /* Set random beacon channel. */
      uint8_t beaconChannelIndex = random_rand() % eu_tsch_hopping_sequence_adv_size;
      tsch_set_static_beacon_channel(eu_tsch_hopping_sequence_adv[beaconChannelIndex]);

      tsch_disable_cca();

      regionSelected = true;
      break;
    }
#endif

#if SF_RF_REGION_CHN_ENABLE
    case E_SF_RF_REGION_CHN:
    {
      /* Set CCA THRESHOLD */
      NETSTACK_RADIO.set_value(RADIO_PARAM_CCA_THRESHOLD, RF_REGION_CHN_CCA_RSSI_THRESHOLD);

      /* Frequency band China. */
      /* Set hopping sequence related to freq. band.  */
      tsch_set_hopping_sequence(chn_tsch_hopping_sequence, chn_tsch_hopping_sequence_size);
      tsch_set_hopping_sequence_adv(chn_tsch_hopping_sequence_adv, chn_tsch_hopping_sequence_adv_size);

      #if !CONTIKI_TARGET_COOJA
      gDot_15_4g_radioConf = &chn_dot_15_4g_radioConf;
      #endif

      /* Set random beacon channel. */
      uint8_t beaconChannelIndex = random_rand() % chn_tsch_hopping_sequence_adv_size;
      tsch_set_static_beacon_channel(chn_tsch_hopping_sequence_adv[beaconChannelIndex]);

      tsch_disable_cca();

      regionSelected = true;
      break;
    }
#endif

#if SF_RF_REGION_JP_ENABLE
    case E_SF_RF_REGION_JP:
    {
      /* Set CCA THRESHOLD */
      NETSTACK_RADIO.set_value(RADIO_PARAM_CCA_THRESHOLD, RF_REGION_JP_CCA_RSSI_THRESHOLD);

      /* Frequency band Japan. */
      /* Set hopping sequence related to freq. band.  */
      tsch_set_hopping_sequence(jp_tsch_hopping_sequence, jp_tsch_hopping_sequence_size);
      tsch_set_hopping_sequence_adv(jp_tsch_hopping_sequence_adv, jp_tsch_hopping_sequence_adv_size);

      #if !CONTIKI_TARGET_COOJA
      gDot_15_4g_radioConf = &jp_dot_15_4g_radioConf;
      #endif

      /* Set random beacon channel. */
      uint8_t beaconChannelIndex = random_rand() % jp_tsch_hopping_sequence_adv_size;
      tsch_set_static_beacon_channel(jp_tsch_hopping_sequence_adv[beaconChannelIndex]);

      tsch_enable_cca();

      regionSelected = true;
      break;
    }
#endif

#if SF_RF_REGION_SK_ENABLE
    case E_SF_RF_REGION_SK:
    {
      /* Set CCA THRESHOLD */
      NETSTACK_RADIO.set_value(RADIO_PARAM_CCA_THRESHOLD, RF_REGION_SK_CCA_RSSI_THRESHOLD);

      /* Frequency band South Korea. */
      /* Set hopping sequence related to freq. band.  */
      tsch_set_hopping_sequence(sk_tsch_hopping_sequence, sk_tsch_hopping_sequence_size);
      tsch_set_hopping_sequence_adv(sk_tsch_hopping_sequence_adv, sk_tsch_hopping_sequence_adv_size);

      #if !CONTIKI_TARGET_COOJA
      gDot_15_4g_radioConf = &sk_dot_15_4g_radioConf;
      #endif

      /* Set random beacon channel. */
      uint8_t beaconChannelIndex = random_rand() % sk_tsch_hopping_sequence_adv_size;
      tsch_set_static_beacon_channel(sk_tsch_hopping_sequence_adv[beaconChannelIndex]);

      tsch_disable_cca();

      regionSelected = true;
      break;
    }
#endif

#if SF_RF_REGION_IN_ENABLE
    case E_SF_RF_REGION_IN:
    {
      /* Set CCA THRESHOLD */
      NETSTACK_RADIO.set_value(RADIO_PARAM_CCA_THRESHOLD, RF_REGION_IN_CCA_RSSI_THRESHOLD);

      /* Frequency band India. */
      /* Set hopping sequence related to freq. band.  */
      tsch_set_hopping_sequence(in_tsch_hopping_sequence, in_tsch_hopping_sequence_size);
      tsch_set_hopping_sequence_adv(in_tsch_hopping_sequence_adv, in_tsch_hopping_sequence_adv_size);

      #if !CONTIKI_TARGET_COOJA
      gDot_15_4g_radioConf = &in_dot_15_4g_radioConf;
      #endif

      /* Set random beacon channel. */
      uint8_t beaconChannelIndex = random_rand() % in_tsch_hopping_sequence_adv_size;
      tsch_set_static_beacon_channel(in_tsch_hopping_sequence_adv[beaconChannelIndex]);

      tsch_disable_cca();

      regionSelected = true;
      break;
    }
#endif

#if SF_RF_REGION_GLOB2G4_ENABLE
    case E_SF_RF_REGION_GLOB2G4:
    {
      /* Set CCA THRESHOLD */
      NETSTACK_RADIO.set_value(RADIO_PARAM_CCA_THRESHOLD, RF_REGION_GLOB2G4_CCA_RSSI_THRESHOLD);

      /* Frequency band Global 2.4GHz. */
      /* Set hopping sequence related to freq. band.  */
      tsch_set_hopping_sequence(glob2g4_tsch_hopping_sequence, glob2g4_tsch_hopping_sequence_size);
      tsch_set_hopping_sequence_adv(glob2g4_tsch_hopping_sequence_adv, glob2g4_tsch_hopping_sequence_adv_size);

      #if !CONTIKI_TARGET_COOJA
      gDot_15_4g_radioConf = &glob2g4_dot_15_4g_radioConf;
      #endif

      /* Set random beacon channel. */
      uint8_t beaconChannelIndex = random_rand() % glob2g4_tsch_hopping_sequence_adv_size;
      tsch_set_static_beacon_channel(glob2g4_tsch_hopping_sequence_adv[beaconChannelIndex]);

      tsch_disable_cca();

      regionSelected = true;
      break;
    }
#endif

    default:
      /* No valid region found. */
      regionSelected = false;
      break;
  }

  if(true  == regionSelected)
  {
    /* Re-Initialization of RF driver so that the new rf configs takes effect.*/
    NETSTACK_RADIO.init();
  }

  return regionSelected;
}

#ifdef __cplusplus
}
#endif
