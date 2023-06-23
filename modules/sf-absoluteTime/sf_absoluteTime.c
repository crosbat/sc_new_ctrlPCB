/**
 * @code
 *  ___ _____ _   ___ _  _____ ___  ___  ___ ___
 * / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 * \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 * |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 * embedded.connectivity.solutions.==============
 * @endcode
 *
 * @copyright  STACKFORCE GmbH, Germany, www.stackforce.de
 * @author     STACKFORCE
 * @brief      API implementation of the absolute time module.
 *
 */

/*==============================================================================
                            INCLUDES
==============================================================================*/
/* Stack include */
#include "net/mac/tsch/tsch.h"
#if !CONTIKI_TARGET_COOJA
#include <ti/drivers/dpl/ClockP.h>
#else
#include "process.h"
#endif
/* Stack includes */
#include "tsch.h"
/* Application includes */
#include "sf_types.h"
#include "sf_absoluteTime.h"

/*==============================================================================
                            MACROS
==============================================================================*/
/* Define absolute time increment period in second. */
#define SF_ABSOLUTE_TIME_INCREMENT_PERIOD         1U
/* Second to microsecond*/
#define SF_ABSOLUTE_TIME_TO_MICROSEC              1000000U

/*==============================================================================
                            GLOBAL VARIABLES
==============================================================================*/
#if !CONTIKI_TARGET_COOJA
ClockP_Struct clockStruct;
ClockP_Handle clockHandle;
extern uint32_t ClockP_tickPeriod;
#endif

/*==============================================================================
                                    ISR
==============================================================================*/
/* General purpose timer callback function */
#if !CONTIKI_TARGET_COOJA
static void absoluteTime_isr(uintptr_t arg0);
#endif

/*==============================================================================
                                    PROCESSES
==============================================================================*/
/* Handles absolute time increment */
#if CONTIKI_TARGET_COOJA
PROCESS(absoluteTime_increment_process, "Absolute time increment process");
#endif

/*==============================================================================
                                ISR IMPLEMENTATION
==============================================================================*/
#if !CONTIKI_TARGET_COOJA
/*----------------------------------------------------------------------------*/
/*! absoluteTime_isr */
/*----------------------------------------------------------------------------*/
static void absoluteTime_isr(uintptr_t arg0)
{
  uint32_t internalAbsoluteTime = tsch_get_internal_absolute_time();
  if(internalAbsoluteTime)
  {
    tsch_set_internal_absolute_time(internalAbsoluteTime
                                    + SF_ABSOLUTE_TIME_INCREMENT_PERIOD);
  }
}
#endif

/*==============================================================================
                            API IMPLEMENTATION
==============================================================================*/
/*----------------------------------------------------------------------------*/
/*! sf_absoluteTime_getSeconds */
/*----------------------------------------------------------------------------*/
uint8_t sf_absoluteTime_getSeconds(uint32_t timestamp)
{
  return timestamp % 60;
} /* sf_absoluteTime_getSeconds */

/*----------------------------------------------------------------------------*/
/*! sf_absoluteTime_getMinutes */
/*----------------------------------------------------------------------------*/
uint8_t sf_absoluteTime_getMinutes(uint32_t timestamp)
{
  timestamp /= 60;

  return timestamp % 60;
} /* sf_absoluteTime_getMinutes */

/*----------------------------------------------------------------------------*/
/*! sf_absoluteTime_getHours */
/*----------------------------------------------------------------------------*/
uint8_t sf_absoluteTime_getHours(uint32_t timestamp)
{
  timestamp /= 3600;

  return timestamp % 24;
} /* sf_absoluteTime_getHours */

/*----------------------------------------------------------------------------*/
/*! sf_absoluteTime_getTime */
/*----------------------------------------------------------------------------*/
uint32_t sf_absoluteTime_getTime(void)
{
  return tsch_get_internal_absolute_time();
} /* sf_absoluteTime_getTime */

/*----------------------------------------------------------------------------*/
/*! sf_absoluteTime_startTimer */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_absoluteTime_startTimer(void)
{
#if !CONTIKI_TARGET_COOJA
  ClockP_Params clkParams;
  ClockP_Params_init(&clkParams);

  clkParams.period = SF_ABSOLUTE_TIME_INCREMENT_PERIOD * SF_ABSOLUTE_TIME_TO_MICROSEC
                     / ClockP_tickPeriod;
  clkParams.startFlag = true;

  /* Construct a periodic clock. */
  ClockP_construct(&clockStruct, (ClockP_Fxn)absoluteTime_isr,
                   SF_ABSOLUTE_TIME_INCREMENT_PERIOD * SF_ABSOLUTE_TIME_TO_MICROSEC
                   / ClockP_tickPeriod, &clkParams);

  clockHandle = ClockP_handle(&clockStruct);
  ClockP_start(clockHandle);
#else
  process_start(&absoluteTime_increment_process, 0);
#endif

  return E_SF_SUCCESS;
} /* sf_absoluteTime_startTimer */

/*==============================================================================
                                PROCESS IMPLEMENTATION
==============================================================================*/
#if CONTIKI_TARGET_COOJA
/*----------------------------------------------------------------------------*/
/*! absoluteTime_increment_process */
/*----------------------------------------------------------------------------*/
PROCESS_THREAD(absoluteTime_increment_process, ev, data)
{
  static struct etimer incrementTimer;
  static uint32_t incrementInterval = SF_ABSOLUTE_TIME_INCREMENT_PERIOD * CLOCK_SECOND;
  static uint32_t internalAbsoluteTime = 0;

  PROCESS_BEGIN();

  etimer_set(&incrementTimer, incrementInterval);

  while(1)
  {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&incrementTimer));

    internalAbsoluteTime = tsch_get_internal_absolute_time();
    tsch_set_internal_absolute_time(internalAbsoluteTime
                                    + SF_ABSOLUTE_TIME_INCREMENT_PERIOD);

    etimer_set(&incrementTimer, incrementInterval);
  }

  PROCESS_END();
}

#endif
