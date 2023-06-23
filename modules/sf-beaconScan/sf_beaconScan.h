#ifdef __cplusplus
extern "C" {
#endif

#ifndef __SF_BEACONSCAN_H__
#define __SF_BEACONSCAN_H__

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
 @brief      This header contains information for using the beacon scan module.

 @details This module scans for the configured beacon type. It filters the
          best RSSI if several beacons are detected. If at least one
          valid beacon is received, it instructs the TSCH to associate to the
          source network.
*/

/*==============================================================================
                            INCLUDES
==============================================================================*/
/* Standard include */
#include <stdint.h>
#include <stdbool.h>
/* Stack specific include */
#include "net/linkaddr.h"
/* Application specific include */
#include "sf_types.h"

/*==============================================================================
                            FUNCTION PROTOTYPES
==============================================================================*/
/*============================================================================*/
/**
 * \brief Set the accepted beacon type.
 *
 * \param beaconType     The beacon flag to be filtered.
 */
/*============================================================================*/
void sf_beaconScan_setBeaconType(E_SF_JOIN_BEACON_t beaconType);

/*============================================================================*/
/**
 * \brief Returns best beacon source link address.
 *
 * \return BMSCC short address
 */
/*============================================================================*/
linkaddr_t* sf_beaconScan_getBestBeaconAddress(void);

#endif /* __SF_BEACONSCAN_H__ */

#ifdef __cplusplus
}
#endif
