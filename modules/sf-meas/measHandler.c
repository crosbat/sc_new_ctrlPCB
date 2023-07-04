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
 @brief      A dummy Implementation of the measurement handler interface.
*/

/*=============================================================================
                                INCLUDES
=============================================================================*/
#include <stdbool.h>
#include <string.h>
#include "measHandler_api.h"
#include "lib/random.h"
#include "sf_absoluteTime.h"
#include <ti/drivers/GPIO.h>
#include <ti/drivers/adc.h>
#include "CC26X2R1_LAUNCHXL.h"


#include "sys/log.h"
#include <stdio.h>
#include <ti/drivers/ADC.h>

//                          GLOBAL VARIABLES
//=============================================================================*/
/* Measurement storage */
static meas_t gMeas = {0};
/* Measurement type
   false - old
   true - new */
static bool gMeasType = false;
//we need to assign gMeas to ADC read value...

unsigned int en_val = 1; // for making the LED on

uint16_t adcValue;

uint16_t adcValue0;
uint32_t adcValue0MicroVolt;
/*=============================================================================
                          API IMPLEMENTATION
=============================================================================*/
/*------------------------------------------------------------------------------
  measHandler_performMeas()
------------------------------------------------------------------------------*/
__attribute__((weak)) bool measHandler_performMeas(void)
{




  /* Get internal absolute time */
  gMeas.timeStamp = sf_absoluteTime_getTime();

  //MY ADC CODE

  ADC_Handle adc;
  ADC_Params params;
  int_fast16_t res;
  uint_least8_t adc_index = 7; // index = 6 for current, IOID_29 and index = 7 for voltage, IOID_30.

  ADC_Params_init(&params);
  //adc = ADC_open(CC26X2R1_LAUNCHXL_DIO29_ANALOG, &params);
  adc = ADC_open(adc_index, &params);

  /* Blocking mode conversion */
  res = ADC_convert(adc, &adcValue0);

  if (res == ADC_STATUS_SUCCESS)
  {

      adcValue0MicroVolt = ADC_convertRawToMicroVolts(adc, adcValue0);
  }
  ADC_close(adc);
  gMeas.value = adcValue0MicroVolt;


  ////////

  /* Provide dummy measurement*/
//  gMeas.value = random_rand();



  //Make an LED turn-on
  //GPIO_write(IOID_5,en_val);

  /* Set meas as new */
  gMeasType = true;

  return true;
}/* measHandler_performMeas() */

/*------------------------------------------------------------------------------
  measHandler_getMeas()
------------------------------------------------------------------------------*/
__attribute__((weak)) bool measHandler_getMeas(meas_t* pMeas)
{
  if(true == gMeasType)
  {
    /* Map the measurement */
    memcpy(pMeas, &gMeas, sizeof(gMeas));

    /* Set meas as old */
    gMeasType = false;

    return true;
  }

  return false;
}/* measHandler_getMeas() */

/*------------------------------------------------------------------------------
  measHandler_setStatus()
------------------------------------------------------------------------------*/
__attribute__((weak)) void measHandler_setStatus(E_MEAS_STATUS_t status)
{
  switch(status)
  {
    case E_MEAS_TX_SUCCESS:
      /* Nothing to do for now*/
      break;

    case E_MEAS_TX_FAIL:
      /* Nothing to do for now*/
      break;

  default:
    break;
  }
}/* measHandler_setStatus() */


#ifdef __cplusplus
}
#endif
