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
 @brief      RF interface that handles rf region configuration.
*/

#ifndef _SF_RF_H_
#define _SF_RF_H_

/*****************************************************************************/
/*                              ENUMS                                        */
/*****************************************************************************/

/** Regions enumeration */
typedef enum
{
  E_SF_RF_REGION_EU,
  E_SF_RF_REGION_USCAN,
  E_SF_RF_REGION_CHN,
  E_SF_RF_REGION_JP,
  E_SF_RF_REGION_SK,
  E_SF_RF_REGION_IN,

  E_SF_RF_REGION_GLOB2G4,
} sf_rf_region_t;

/*****************************************************************************/
/*                         API FUNCTIONS                                     */
/*****************************************************************************/
/*============================================================================*/
/*!
  @brief  Sets the appropriate radio and TSCH parameters corresponding to the
          given RF region @ref sf_rf_region_t.

  @return true if successful, false otherwise.
 */
/*============================================================================*/
bool sf_rf_selectRegion(sf_rf_region_t region);

#endif /* _SF_RF_H_ */
