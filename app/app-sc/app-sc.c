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
 @brief      Implementation of minimal application for smart cell.
*/

/*=============================================================================
                                INCLUDES
=============================================================================*/
/* Standard include */
#include "stdint.h"
#include <string.h>
#include <stdio.h>
/* Stack include */
#include "contiki.h"
#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
#endif
#include "dev/button-hal.h"
#include "sys/log.h"
#if !CONTIKI_TARGET_COOJA
#include "Board.h"
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#endif
/* Project specific include */
#include "project-conf.h"
#include "sf_rfSettings.h"
#include "sf_rf.h"
#include "sf_types.h"
#include "sf_configMgmt.h"
#include "sf_persistentDataStorage.h"
#include "sf_frameType.h"
#include "sf_joinRequester.h"
#include "sf_measSender.h"
#include "sf_tsch.h"
#include "sf_app_api.h"
#include "sf_led.h"
//reboot includes
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include <ti/drivers/GPIO.h>
#include <ti/drivers/ADC.h>
//AK 18.07.2023. Include Timer.h from TI drivers for fast ADC/GPIO configuration
#include <ti/drivers/Timer.h>

/*=============================================================================
                               MACROS
=============================================================================*/
/* Defines log source */
#define LOG_MODULE                 "App"
/* Defines log level*/
#ifndef LOG_CONF_APP
  #define LOG_LEVEL     LOG_LEVEL_NONE
#else
  #define LOG_LEVEL     LOG_CONF_APP
#endif
/* 1: LED enabled
   0: LED disabled */
#ifndef LED_ENABLED
#define LED_ENABLED                1
#endif

/*=============================================================================
                                GLOBAL VARIABLES
=============================================================================*/
#if !CONTIKI_TARGET_COOJA
/* Wake-up Button pin table */
PIN_Config ButtonTableWakeUp[] = {
    Board_PIN_BUTTON0 | PIN_INPUT_EN | PIN_PULLUP | PINCC26XX_WAKEUP_NEGEDGE,
    PIN_TERMINATE
};
#endif

// Handling the data from Master

uint8_t dispDownlink = 0;
int sb_process_test = 0;

uint16_t adcValue0;
uint32_t adcValue0MicroVolt;
uint16_t adcValue1;
uint32_t adcValue1MicroVolt;


/*=============================================================================
                              PROCESS DECLARATION
=============================================================================*/
PROCESS(sc_app_process, "The main smart cell process");
PROCESS(adc_sb_process, "Fast adc read SB");
AUTOSTART_PROCESSES(&sc_app_process);

PROCESS(button_process, "The button handler");






/*=============================================================================
                        LOCAL FUNCTIONS IMPLEMENTATION
=============================================================================*/


/*============================================================================*/
/**
 * \brief Perform factory reset then go into shutdown mode.
 */
/*============================================================================*/
static void device_factoryReset(void)
{
  /* Set device status to not connected */
  sf_configMgmt_setDeviceStatus(E_CONFIGMGMT_DEVICESTATUS_NOT_CONNECTED);

  /* Switch of MAC. */
  sf_tsch_stop();

  /* Remove configs */
  if(E_SF_SUCCESS != sf_persistentDataStorage_removeConfig())
  {
    LOG_INFO("!Factory reset failed; Soft reset\n");
#if !CONTIKI_TARGET_COOJA
    /* Restart. */
    SysCtrlSystemReset();
#endif
  }
  else
  {
    /* Turn off device and wait for button press. */
    sf_led_flash(LEDS_CONF_RED);
    sf_led_flash(LEDS_CONF_GREEN);
    sf_led_flash(LEDS_CONF_RED);
    sf_led_flash(LEDS_CONF_GREEN);

#if !CONTIKI_TARGET_COOJA
    sf_app_shutDownDevice();
#endif
  }
} /* device_factoryReset() */

/*=============================================================================
                              PROCESSES IMPLEMENTATION
=============================================================================*/
/*------------------------------------------------------------------------------
  button_process()
------------------------------------------------------------------------------*/
PROCESS_THREAD(button_process, ev, data)
{
  PROCESS_BEGIN();

  /* Check if the button is pressed since startup.
     If yes, toggle manual button event. */
#if !CONTIKI_TARGET_COOJA
  if((BUTTON_HAL_STATE_PRESSED == button_hal_get_state(button_hal_get_by_id(BUTTON_HAL_ID_KEY_LEFT))) &&
     (ev == PROCESS_EVENT_INIT))
  {
    gpio_hal_event_handler(gpio_hal_pin_to_mask(Board_PIN_BUTTON0));
  }
#endif

  while(1)
  {
    /* Wait for button to be pressed. */
    PROCESS_YIELD_UNTIL( (ev == button_hal_release_event) ||
                         (ev == button_hal_periodic_event) ||
                         (ev == button_hal_press_event) );

    button_hal_button_t *button = (button_hal_button_t*) data;
    if(BUTTON_HAL_ID_USER_BUTTON == button->unique_id)
    {
      uint8_t pressDuration = button->press_duration_seconds;

      sf_led_stopBlink();

      LOG_INFO("Button press sec: %u\n", pressDuration);

      /* Check the press duration after release event. */
      if(ev == button_hal_release_event)
      {
        if(pressDuration > 10U)
        {
          /* Factory reset and go off. All configuration will be deleted */
          device_factoryReset();
        }
        else if(pressDuration > 3U)
        {
          sf_led_flash(LEDS_CONF_RED);
          sf_led_flash(LEDS_CONF_GREEN);

          /* Sleep mode */
#if !CONTIKI_TARGET_COOJA
          sf_app_shutDownDevice();
#endif
        }
        else if(pressDuration <= 3U)
        {
          /* Show current device state.
            Green LED flash if connected and associated
            Red LED flash otherwise */
          if(E_CONFIGMGMT_DEVICESTATUS_NOT_CONNECTED == sf_configMgmt_getDeviceStatus() ||
            !tsch_is_associated)
          {
            sf_led_flash(LEDS_CONF_RED);
          }
          else
          {
            sf_led_flash(LEDS_CONF_GREEN);
          }
          GPIO_toggleDio(CC26X2R1_LAUNCHXL_SB_PWM);
          GPIO_toggleDio(CC26X2R1_LAUNCHXL_SB_LED1);
        }
      }
    }

  }

  PROCESS_END();
}/* button_process() */



/*------------------------------------------------------------------------------
  sc_app_process()
------------------------------------------------------------------------------*/

PROCESS_THREAD(sc_app_process, ev, data)
{
  /* Network ID */
  uint16_t panId = 0;
  /* Device address */
  linkaddr_t deviceAddress = linkaddr_null;

  PROCESS_BEGIN();

  LOG_INFO("Starting as smart cell\n");
  LOG_INFO("Version: 1.0.0\n");
  LOG_INFO("Bitrate: %u kbps\n", SF_RADIO_BIT_RATE);

  /* Get reboot source of last restart. */
    uint32_t resetSource = SysCtrlResetSourceGet();
    if(RSTSRC_WAKEUP_FROM_SHUTDOWN == resetSource)
    {
      LOG_ERR("Reboot shut, button\n");
    }
    else if(RSTSRC_WARMRESET == resetSource)
    {
      LOG_ERR("Reboot watchdog\n");
    }
    else if(RSTSRC_PIN_RESET == resetSource)
    {
      /* Hardware reset by the reset pin. */
      LOG_INFO("Reboot pin reset\n");
    }
    else if(RSTSRC_SYSRESET == resetSource)
    {
      /* Reboot source is a software reset. */
      LOG_INFO("Reboot SW reset\n");
    }
    else if(RSTSRC_WAKEUP_FROM_SHUTDOWN == resetSource)
    {
      /* Reboot from shutdown mode. */
      LOG_INFO("Reboot shutdown\n");
    }
    else
    {
      /* Reboot source is unknown. */
      LOG_ERR("Reboot unk\n");
    }

  /* Select 2.4GHz radio frequency region */
  if(false == sf_rf_selectRegion(E_SF_RF_REGION_GLOB2G4))
  {
    LOG_ERR("!Failed to select GLOB2G4 RF region\n");
  }

  /* Set Tx power to Max */
  sf_tsch_setTxPowerMax();

  /* Start button handle process. */
  process_start(&button_process, NULL);



  /* Read device configuration */
  if(E_SF_SUCCESS == sf_configMgmt_readConfig())
  {
    LOG_INFO("Valid configurations are found in the Flash\n");
  }
  else
  {
    LOG_INFO("No valid configurations are found in the Flash\n");

    /* This is a new born device. We need to set here the device configs.
       This includes: The device short address, the device serial number
       and the measurement interval.
       As for testing, we will use the default configurations. */
    if(E_SF_SUCCESS != sf_configMgmt_setDefaultConfig())
    {
      LOG_ERR("!Failed to set the default configs\n");
    }
    else
    {
      LOG_INFO("The default configurations are set \n");
    }
  }

  /* Set configured device link address, if valid, into the TSCH stack */
  if(E_SF_SUCCESS != sf_configMgmt_getParam(deviceAddress.u8,
                                            LINKADDR_SIZE,
                                            E_CONFIGMGMT_PARAM_DEVICE_ADDR))
  {
    LOG_ERR("!Failed to read device address \n");
  }
  else
  {
    LOG_INFO("Stored device address ");
    LOG_INFO_LLADDR(&deviceAddress);
    LOG_INFO_("\n");

    /* Set SC link address into the stack */
    sf_tsch_setDeviceAddress((linkaddr_t*)&deviceAddress);
  }

  /* Get the configured network ID */
  if(E_SF_SUCCESS != sf_configMgmt_getParam(&panId, sizeof(panId),
                                            E_CONFIGMGMT_PARAM_PAN_ID))
  {
    LOG_ERR("!Failed to get network ID param \n");
  }
  else
  {
    LOG_INFO("Stored network ID 0x%02X \n", panId);
  }

  /* Check network ID validity and determine start mode
     + Mode Cold: invalid network id
     + Mode Warm: valid network id */
  if(0 != panId)
  {
    LOG_INFO("Valid PAN ID; Warm start\n");

    /* Reconnect to the same stored network ID.
       Only beacon scan and TSCH association are required.
       No handshake procedure needed */
    sf_joinRequester_rejoin();
  }
  else
  {
    LOG_INFO("Invalid PAN ID; Cold start\n");

    /* Connect to the best available network.
       The Manual join window should be open on the BMSCC*/
    sf_joinRequester_join();
  }

  /* Wait until device is connected to start sending measurements */
  PROCESS_YIELD_UNTIL(E_CONFIGMGMT_DEVICESTATUS_CONNECTED ==
                        sf_configMgmt_getDeviceStatus());





  /* Start measurement Tx process */
  sf_measSender_start();

#ifdef TSCH_TIMING_PIN_0
  gpio_hal_arch_pin_set_output( NULL, TSCH_TIMING_PIN_0 );
#endif

#ifdef TSCH_TIMING_PIN_1
  gpio_hal_arch_pin_set_output( NULL, TSCH_TIMING_PIN_1 );
#endif

#ifdef TSCH_TIMING_PIN_2
  gpio_hal_arch_pin_set_output( NULL, TSCH_TIMING_PIN_2 );
#endif

#ifdef TSCH_TIMING_PIN_3
  gpio_hal_arch_pin_set_output( NULL, TSCH_TIMING_PIN_3 );
#endif

#ifdef TSCH_TIMING_PIN_4
  gpio_hal_arch_pin_set_output( NULL, TSCH_TIMING_PIN_4 );
#endif





  PROCESS_END();
}/* sc_app_process() */


/*
 adc_sb_process
 Here we sample 2 ADC channels: channel index 6 for cell current and channel index 7 for cell voltage.
 Right now gMeas.value takes only the cell voltage output in microvolts.
 We are not sending the cell current information to the master.

 TBD: How to send different data from slave to master instead of just single float variable.
 *
 */

PROCESS_THREAD(adc_sb_process, ev, data)
{
    static struct etimer et;
    int sampling_freq = 10000; //use 10000 for 10kHz sampling

    PROCESS_BEGIN();
    etimer_set(&et, (clock_time_t)CLOCK_SECOND/sampling_freq); /* Trigger a timer after 0.1 millisecond. */
    while(1) {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
      etimer_reset(&et);

      /* Just a dummy variable to see if the process is working fine*/
      if(sb_process_test < 10000)
          sb_process_test++;
      else
          sb_process_test = 0;

      if(dispDownlink < 18)
      {
          GPIO_toggleDio(CC26X2R1_LAUNCHXL_SB_PWM); //This should create a PWM initially once this process starts
      }


      //MY ADC CODE: For reading cell current and voltages.

      ADC_Handle adc_ct;
      ADC_Handle adc_vtg;
      ADC_Params params;
      int_fast16_t res_ct;
      int_fast16_t res_vtg;
      uint_least8_t adc_index_ct = 6; // index = 6 for current, IOID_29 and index = 7 for voltage, IOID_30.
      uint_least8_t adc_index_vtg = 7; // index = 6 for current, IOID_29 and index = 7 for voltage, IOID_30.

      ADC_Params_init(&params);
      adc_ct = ADC_open(adc_index_ct, &params);
      adc_vtg = ADC_open(adc_index_vtg, &params);

      /* Blocking mode conversion */
      res_ct = ADC_convert(adc_ct, &adcValue1);


      if (res_ct == ADC_STATUS_SUCCESS)
      {

          adcValue1MicroVolt = ADC_convertRawToMicroVolts(adc_ct, adcValue1);
      }
      ADC_close(adc_ct);

      res_vtg = ADC_convert(adc_vtg, &adcValue0);

      if (res_vtg == ADC_STATUS_SUCCESS)
      {

          adcValue0MicroVolt = ADC_convertRawToMicroVolts(adc_vtg, adcValue0);
      }


      ADC_close(adc_vtg);

    }

    PROCESS_END();

}

void sb_processTest(void)
{
    process_start(&adc_sb_process,NULL); //starts the adc process
}





/*=============================================================================
                              API IMPLEMENTATION
=============================================================================*/
/*------------------------------------------------------------------------------
  sf_app_handleDownlink()
------------------------------------------------------------------------------*/
__attribute__((weak)) void sf_app_handleDownlink(uint8_t* pInBuf, uint8_t length)
{
  /* Received downlink from the BMS-CC */
  /*      byte 1    |   nbyte
      --------------|-----------
        frame type  |  payload       */
  LOG_INFO("Downlink received; ");
  LOG_INFO_BYTES(pInBuf, length);
  LOG_INFO_("\n");
  dispDownlink = pInBuf[1];

  //we will use a condition to bypass based on the value of dispDownlink.
  if(dispDownlink > 18)
  {
      GPIO_toggleDio(CC26X2R1_LAUNCHXL_SB_PWM);
      GPIO_toggleDio(CC26X2R1_LAUNCHXL_SB_LED1);
  }

} /* sf_app_handleDownlink() */

/*------------------------------------------------------------------------------
  sf_app_shutDownDevice()
------------------------------------------------------------------------------*/
#if !CONTIKI_TARGET_COOJA
void sf_app_shutDownDevice(void)
{
  int16_t status = Power_EFAIL;

  /* Set up wake up pins */
  PINCC26XX_setWakeup(ButtonTableWakeUp);

  /* Re-allow shut down */
  Power_releaseConstraint(PowerCC26XX_DISALLOW_SHUTDOWN);

  /* Go to shutdown */
  status = Power_shutdown(0, 0);
  LOG_INFO("!Shutdown failed %i \n", status);

  /* Should never get here, since shutdown will reset. */
  /* Restart sensor. */
  SysCtrlSystemReset();
}/* sf_app_shutDownDevice() */
#endif
