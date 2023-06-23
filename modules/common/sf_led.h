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
 @brief      This header defines the LED interface.
*/

#ifndef __SF_LED_H__
#define __SF_LED_H__

/**
 *  @addtogroup SF_LED
 *
 *  @details
 *
 *  - <b>SF LED API</b>\n
 *    | API Function                              | Description                                     |
 *    |-------------------------------------------|-------------------------------------------------|
 *    | @ref sf_led_on()                          | @copybrief sf_led_on()                          |
 *    | @ref sf_led_off()                         | @copybrief sf_led_off()                         |
 *    | @ref sf_led_startBlink()                  | @copybrief sf_led_startBlink()                  |
 *    | @ref sf_led_stopBlink()                   | @copybrief sf_led_stopBlink()                   |
 *    | @ref sf_led_onDuration()                  | @copybrief sf_led_onDuration()                  |
 *    | @ref sf_led_waitUntilOff()                | @copybrief sf_led_waitUntilOff()                |
 *    | @ref sf_led_flash()                       | @copybrief sf_led_flash()                       |
 *  @{
 */

/*=============================================================================
                                INCLUDES
=============================================================================*/
/* Standard include */
#include <stdint.h>
#include <stdbool.h>
/* Stack includes */
#include "dev/leds.h"

/*=============================================================================
                                API FUNCTIONS
=============================================================================*/
/*============================================================================*/
/**
 * \brief Turn the given LED on.
 *
 * \param led            LED id
 */
/*============================================================================*/
void sf_led_on(leds_num_t led);

/*============================================================================*/
/**
 * \brief Turn the given LED off.
 *
 * \param led            LED id
 */
/*============================================================================*/
void sf_led_off(leds_num_t led);

/*============================================================================*/
/**
 * \brief Start LED blinking according to the given on/off time.
 *
 * \param led           LED ID
 * \param onTime        On time in milliseconds
 * \param offTime       Off time in milliseconds
 */
/*============================================================================*/
void sf_led_startBlink(leds_num_t led, uint16_t onTime, uint16_t offTime);

/*============================================================================*/
/**
 * \brief Stop LED blinking.
 */
/*============================================================================*/
void sf_led_stopBlink(void);

/*============================================================================*/
/**
 * \brief Switch the given LED on for a given period of time.
 *
 * \param led           LED ID
 * \param onTime        On time in milliseconds
 */
/*============================================================================*/
void sf_led_onDuration(leds_num_t led, uint16_t onTime);

/*============================================================================*/
/**
 * \brief Check LED off state. It switch off the LED if ON time elapsed.
 *        The @ref sf_led_onDuration function is supposed to be called earlier.
 *
 * \return 1: LED switched OFF
 *         0: LED still ON
 */
/*============================================================================*/
int sf_led_waitUntilOff(void);

/*============================================================================*/
/**
 * \brief Flash the given LED.
 */
/*============================================================================*/
void sf_led_flash(leds_num_t led);

/*! @} */

#endif /* __SF_LED_H__ */

#ifdef __cplusplus
}
#endif
