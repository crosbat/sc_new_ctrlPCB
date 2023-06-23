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
 * @brief      API implementation of the LED handler.
 *
 */

/*=============================================================================
                                INCLUDES
=============================================================================*/
#include "sf_led.h"

/*=============================================================================
                                STRUCTS
=============================================================================*/
/* LED parameters. */
typedef struct
{
  /* LED identifier */
  leds_num_t ledNum;
  /* True: active
     false: inactive */
  bool active;
  /* On period */
  uint16_t onTicks;
  /* Off period */
  uint16_t offTicks;
} sf_led_t;

/*=============================================================================
                            GLOBAL VARIABLES
=============================================================================*/
/* Storage LED parameters */
static sf_led_t gLedHandler;
/* Storage of the data of LED blink timer*/
static struct etimer gLedBlinkTimer;

/*=============================================================================
                              PROCESSES
=============================================================================*/
PROCESS(led_process, "The LED process");

/*=============================================================================
                              PROCESSES IMPLEMENTATION
=============================================================================*/
/*------------------------------------------------------------------------------
  led_process()
------------------------------------------------------------------------------*/
PROCESS_THREAD(led_process, ev, data)
{
  PROCESS_BEGIN();

  while(1)
  {
    sf_led_on(gLedHandler.ledNum);

    etimer_set(&gLedBlinkTimer, gLedHandler.onTicks);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&gLedBlinkTimer) || ev == PROCESS_EVENT_EXIT);

    leds_single_off(gLedHandler.ledNum);

    if(gLedHandler.offTicks)
    {
    etimer_set(&gLedBlinkTimer, gLedHandler.offTicks);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&gLedBlinkTimer) || ev == PROCESS_EVENT_EXIT);
    }
    else
    {
    /* Exit process, no off time. */
    PROCESS_EXIT();
    }
  }

  PROCESS_END();
} /* led_process() */

/*=============================================================================
                    API FUNCTIONS IMPLEMENTATION
=============================================================================*/
/*------------------------------------------------------------------------------
  sf_led_on()
------------------------------------------------------------------------------*/
void sf_led_on(leds_num_t led)
{
  leds_single_on(led);
}/* sf_led_on() */

/*------------------------------------------------------------------------------
  sf_led_off()
------------------------------------------------------------------------------*/
void sf_led_off(leds_num_t led)
{
  leds_single_off(led);
}/* sf_led_off() */

/*------------------------------------------------------------------------------
  sf_led_startBlink()
------------------------------------------------------------------------------*/
void sf_led_startBlink(leds_num_t led, uint16_t onTime, uint16_t offTime)
{
  gLedHandler.ledNum = led;
  gLedHandler.active = true;
  gLedHandler.onTicks = CLOCK_SECOND * onTime / 1000;
  gLedHandler.offTicks = CLOCK_SECOND * offTime / 1000;

  if(process_is_running(&led_process))
  {
    sf_led_stopBlink();
  }

  process_start(&led_process, NULL);
} /* sf_led_startBlink() */

/*------------------------------------------------------------------------------
  sf_led_stopBlink()
------------------------------------------------------------------------------*/
void sf_led_stopBlink( void )
{
  gLedHandler.ledNum = 0;
  gLedHandler.active = false;
  gLedHandler.onTicks = 0;
  gLedHandler.offTicks = 0;

  process_exit(&led_process);

  leds_single_off(LEDS_CONF_RED);
  leds_single_off(LEDS_CONF_GREEN);
} /* sf_led_stopBlink() */

/*------------------------------------------------------------------------------
  sf_led_onDuration()
------------------------------------------------------------------------------*/
void sf_led_onDuration(leds_num_t led, uint16_t onTime)
{
  sf_led_startBlink(led, onTime , 0);
} /* sf_led_onDuration() */

/*------------------------------------------------------------------------------
  sf_led_waitUntilOff()
------------------------------------------------------------------------------*/
int sf_led_waitUntilOff( void )
{
  int ret;
  ret = etimer_expired(&gLedBlinkTimer);

  if(ret)
  {
    sf_led_stopBlink();
    leds_single_off(gLedHandler.ledNum);
  }

  return ret;
}/* sf_led_waitUntilOff() */

/*------------------------------------------------------------------------------
  sf_led_flash()
------------------------------------------------------------------------------*/
void sf_led_flash(leds_num_t led)
{
  volatile uint32_t i;
  uint32_t on  = 600000;
  uint32_t off = 2500000;

  // 3333333 => ~ 1 sec.
  // 600000 => ~ 0.2 sec.
  // 2500000 => ~ 0.8 sec.

  leds_single_off(LEDS_CONF_RED);
  leds_single_off(LEDS_CONF_GREEN);

  sf_led_on(led);
  for(i = 0; i < on; ++i) {
    __asm__ __volatile__ ("nop");
  }

  sf_led_off(led);
  for(i = 0; i < off; ++i) {
    __asm__ __volatile__ ("nop");
  }
} /* sf_led_flash() */

