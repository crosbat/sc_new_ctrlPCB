/*
 * Copyright (c) 2017, George Oikonomou - http://www.spd.gr
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup main
 * @{
 */
/*---------------------------------------------------------------------------*/
/**
 * \file
 *
 * Implementation of \os's main routine
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "contiki-net.h"
#include "sys/node-id.h"
#include "sys/platform.h"
#include "sys/energest.h"
#if STACK_CHECK_ENABLED
#include "sys/stack-check.h"
#endif /* STACK_CHECK_ENABLED */
#include "dev/watchdog.h"
#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
#endif /* MAC_CONF_WITH_TSCH */

#include "net/queuebuf.h"
#if BUILD_WITH_COAP
#include "net/app-layer/coap/coap-engine.h"
#endif /* BUILD_WITH_COAP */
#if BUILD_WITH_SNMP
#include "net/app-layer/snmp/snmp.h"
#endif /* BUILD_WITH_SNMP */
#if BUILD_WITH_RPL_BORDER_ROUTER
#include "services/rpl-border-router/rpl-border-router.h"
#endif /* BUILD_WITH_RPL_BORDER_ROUTER */
#if BUILD_WITH_ORCHESTRA
#include "services/orchestra/orchestra.h"
#endif /* BUILD_WITH_ORCHESTRA */
#if BUILD_WITH_SHELL
#include "services/shell/serial-shell.h"
#endif /* BUILD_WITH_SHELL */
#if BUILD_WITH_SIMPLE_ENERGEST
#include "services/simple-energest/simple-energest.h"
#endif /* BUILD_WITH_SIMPLE_ENERGEST */
#if BUILD_WITH_TSCH_CS
#include "services/tsch-cs/tsch-cs.h"
#endif /* BUILD_WITH_TSCH_CS */

#include <stdio.h>
#include <stdint.h>

//AK 04.07.23 ADC.h header called
#include <ti/drivers/ADC.h>
//AK 18.07.2023. Include Timer.h from TI drivers for fast ADC/GPIO configuration
#include <ti/drivers/Timer.h>
#include "CC26X2R1_LAUNCHXL.h"
#include <ti/drivers/timer/GPTimerCC26XX.h>

#include <ti/drivers/PWM.h>

void timerCallback(Timer_Handle myHandle, int_fast16_t status);
int ti_timer_check = 0;
float poll_period_test = 0.0;
int pwm_check = 0;

/*---------------------------------------------------------------------------*/
/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "Main"
#define LOG_LEVEL LOG_LEVEL_MAIN
/*---------------------------------------------------------------------------*/
int
#if PLATFORM_MAIN_ACCEPTS_ARGS
main(int argc, char **argv)
{
  platform_process_args(argc, argv);
#else
main(void)
{
#endif
  platform_init_stage_one();



  clock_init();
  rtimer_init();
  process_init();
  process_start(&etimer_process, NULL);
  ctimer_init();
  watchdog_init();

  energest_init();


  //AK 04.07.23 ADC init function added here
  ADC_init();


#if STACK_CHECK_ENABLED
  stack_check_init();
#endif

  platform_init_stage_two();

#if QUEUEBUF_ENABLED
  queuebuf_init();
#endif /* QUEUEBUF_ENABLED */
  netstack_init();
  node_id_init();

  LOG_INFO("Starting " CONTIKI_VERSION_STRING "\n");
  LOG_DBG("TARGET=%s", CONTIKI_TARGET_STRING);
#ifdef CONTIKI_BOARD_STRING
  LOG_DBG_(", BOARD=%s", CONTIKI_BOARD_STRING);
#endif
  LOG_DBG_("\n");
  LOG_INFO("- Routing: disabled\n");
  LOG_INFO("- Net: %s\n", NETSTACK_NETWORK.name);
  LOG_INFO("- MAC: %s\n", NETSTACK_MAC.name);
  LOG_INFO("- 802.15.4 PANID: 0x%04x\n", IEEE802154_PANID);
#if MAC_CONF_WITH_TSCH
  LOG_INFO("- 802.15.4 TSCH default hopping sequence length: %u\n", (unsigned)sizeof(TSCH_DEFAULT_HOPPING_SEQUENCE));
#else /* MAC_CONF_WITH_TSCH */
  LOG_INFO("- 802.15.4 Default channel: %u\n", IEEE802154_DEFAULT_CHANNEL);
#endif /* MAC_CONF_WITH_TSCH */

  LOG_INFO("Node ID: %u\n", node_id);
  LOG_INFO("Link-layer address: ");
  LOG_INFO_LLADDR(&linkaddr_node_addr);
  LOG_INFO_("\n");

#if NETSTACK_CONF_WITH_IPV6
  {
    uip_ds6_addr_t *lladdr;
    memcpy(&uip_lladdr.addr, &linkaddr_node_addr, sizeof(uip_lladdr.addr));
    process_start(&tcpip_process, NULL);

    lladdr = uip_ds6_get_link_local(-1);
    LOG_INFO("Tentative link-local IPv6 address: ");
    LOG_INFO_6ADDR(lladdr != NULL ? &lladdr->ipaddr : NULL);
    LOG_INFO_("\n");
  }
#endif /* NETSTACK_CONF_WITH_IPV6 */

  platform_init_stage_three();




#if BUILD_WITH_RPL_BORDER_ROUTER
  rpl_border_router_init();
  LOG_DBG("With RPL Border Router\n");
#endif /* BUILD_WITH_RPL_BORDER_ROUTER */

#if BUILD_WITH_ORCHESTRA
  orchestra_init();
  LOG_DBG("With Orchestra\n");
#endif /* BUILD_WITH_ORCHESTRA */

#if BUILD_WITH_SHELL
  serial_shell_init();
  LOG_DBG("With Shell\n");
#endif /* BUILD_WITH_SHELL */

#if BUILD_WITH_COAP
  coap_engine_init();
  LOG_DBG("With CoAP\n");
#endif /* BUILD_WITH_COAP */

#if BUILD_WITH_SNMP
  snmp_init();
  LOG_DBG("With SNMP\n");
#endif /* BUILD_WITH_SNMP */

#if BUILD_WITH_SIMPLE_ENERGEST
  simple_energest_init();
#endif /* BUILD_WITH_SIMPLE_ENERGEST */

#if BUILD_WITH_TSCH_CS
  /* Initialize the channel selection module */
  tsch_cs_adaptations_init();
#endif /* BUILD_WITH_TSCH_CS */

  autostart_start(autostart_processes);

  watchdog_start();

/*Timer code:
 * TBD: Move to a separate function
 *
 *                        */

//  Timer_init();
//  Timer_Handle timer0;
//  Timer_Params params;
//
//
//
//  /*
//   * Setting up the timer in continuous callback mode that calls the callback
//   * function every 1,000,000 microseconds, or 1 second.
//   */
//  Timer_Params_init(&params);
//  params.period        = 1000000;
//  params.periodUnits   = Timer_PERIOD_US;
//  params.timerMode     = Timer_CONTINUOUS_CALLBACK;
//  params.timerCallback = timerCallback;
//
//  timer0 = Timer_open(CONFIG_TIMER_0, &params);
//
//  if (timer0 == NULL)
//  {
//      /* Failed to initialized timer */
//      while (1) {}
//  }
//
//  if (Timer_start(timer0) == Timer_STATUS_ERROR)
//  {
//      /* Failed to start timer */
//      while (1) {}
//  }
//
////////








  //Main loop calling all the contiki processes.


#if PLATFORM_PROVIDES_MAIN_LOOP
  platform_main_loop();
#else
  while(1) {
    uint8_t r;
    do {
      r = process_run();
//     Here we can call a function. This is repeated at about 100kHz rate or more.
//     Try timer or any other code.


//      if(pwm_check<5)
//          pwm_check++;
//      else
//          pwm_check=0;
//
//      if(pwm_check<2)
       GPIO_toggleDio(CC26X2R1_LAUNCHXL_SB_PWM);




      watchdog_periodic();
    } while(r > 0);









//    platform_idle(); // Never uncomment this. Gives problems on our PCBs but works on launchpads.
  }
#endif

  return 0;
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 */


/*timer0 call back function*/
//void timerCallback(Timer_Handle myHandle, int_fast16_t status)
//{
//    if(ti_timer_check<60)
//        ti_timer_check++;
//    else
//        ti_timer_check=0;
//}
