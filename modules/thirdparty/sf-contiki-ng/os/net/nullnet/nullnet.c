/*
 * Copyright (c) 2017, RISE SICS.
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         NullNet, a minimal network layer.
 * \author
 *         Simon Duquennoy <simon.duquennoy@ri.se>
 *
 */

/**
 * \addtogroup nullnet
 * @{
 */

#include "contiki.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#if LLSEC802154_ENABLED
#include "tsch-security.h"
#endif

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "NullNet"
#define LOG_LEVEL LOG_LEVEL_NULLNET

uint8_t *nullnet_buf;
uint16_t nullnet_len;
static nullnet_input_callback_t current_input_callback = NULL;
static nullnet_output_callback_t current_output_callback = NULL;

#if NULLNET_SINGLE_PACKET_ENABLED
bool busy_singlePacket = false;
bool enabled_singlePacket = true;
#endif /* NULLNET_SINGLE_PACKET_ENABLED */
/*--------------------------------------------------------------------*/
#if NULLNET_SINGLE_PACKET_ENABLED
void
nullnet_enable_single_packet()
{
  enabled_singlePacket = true;
}
/*--------------------------------------------------------------------*/
void
nullnet_disable_single_packet()
{
  enabled_singlePacket = false;
}
#endif /* NULLNET_SINGLE_PACKET_ENABLED */
/*--------------------------------------------------------------------*/
static void
init(void)
{
  LOG_INFO("init\n");
  current_input_callback = NULL;
  current_output_callback = NULL;
}
/*--------------------------------------------------------------------*/
static void
input(void)
{
  if(current_input_callback != NULL) {
    LOG_INFO("received %u bytes from ", packetbuf_datalen());
    LOG_INFO_LLADDR(packetbuf_addr(PACKETBUF_ADDR_SENDER));
    LOG_INFO_("\n");
    current_input_callback(packetbuf_dataptr(), packetbuf_datalen(),
      packetbuf_addr(PACKETBUF_ADDR_SENDER), packetbuf_addr(PACKETBUF_ADDR_RECEIVER));
  }
}
/*--------------------------------------------------------------------*/
void
nullnet_set_input_callback(nullnet_input_callback_t callback)
{
 current_input_callback = callback;
}
/*--------------------------------------------------------------------*/
void
nullnet_set_output_callback(nullnet_output_callback_t callback)
{
 current_output_callback = callback;
}
/*--------------------------------------------------------------------*/
static void
nullnet_mac_callback(void *ptr, int status, int transmissions, uint8_t ack_app_data)
{

#if NULLNET_SINGLE_PACKET_ENABLED
  busy_singlePacket = false;
#endif /* NULLNET_SINGLE_PACKET_ENABLED */

  if(current_output_callback != NULL) {
    nullnet_tx_status_t tx_status = NULLNET_TX_ERR;

    switch (status)
    {
    case MAC_TX_OK:
      tx_status = NULLNET_TX_OK;
      break;

    case MAC_TX_COLLISION:
      tx_status = NULLNET_TX_COLLISION;
      break;

    case MAC_TX_NOACK:
      tx_status = NULLNET_TX_NOACK;
      break;

    case MAC_TX_DEFERRED:
      tx_status = NULLNET_TX_DEFERRED;
      break;

    case MAC_TX_ERR:
      tx_status = NULLNET_TX_ERR;
      break;

    case MAC_TX_ERR_FATAL:
      tx_status = NULLNET_TX_ERR_FATAL;
      break;

    case MAC_TX_ERR1:
      tx_status = NULLNET_TX_ERR;
      break;

    case MAC_TX_ERR2:
      tx_status = NULLNET_TX_ERR;
      break;

    default:
      break;
    }

    current_output_callback(ptr, tx_status, ack_app_data);
  }


  return;
}
/*--------------------------------------------------------------------*/
static uint8_t
output(const linkaddr_t *dest, void *ptr, const uint8_t frameType)
{
  packetbuf_attr_t transmissionsAttr;

#if NULLNET_SINGLE_PACKET_ENABLED
  if(true == enabled_singlePacket){
    if(true == busy_singlePacket) {
      if(current_output_callback != NULL) {
          current_output_callback(ptr, NULLNET_TX_BUSY, 0);
      }
      return 0;
    }
  }

  busy_singlePacket = true;
#endif /* NULLNET_SINGLE_PACKET_ENABLED */

  transmissionsAttr = packetbuf_attr(PACKETBUF_ATTR_MAX_MAC_TRANSMISSIONS);
  packetbuf_clear();
  packetbuf_copyfrom(nullnet_buf, nullnet_len);
  if(dest != NULL) {
    packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, dest);
  } else {
    packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, &linkaddr_null);
  }
  packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &linkaddr_node_addr);

  packetbuf_set_attr(PACKETBUF_ATTR_MAX_MAC_TRANSMISSIONS, transmissionsAttr);

  packetbuf_set_attr(PACKETBUF_ATTR_FRAME_TYPE, frameType);

#if LLSEC802154_ENABLED
  tsch_security_set_packetbuf_attr(frameType);
#endif /* LLSEC802154_ENABLED */

  LOG_INFO("sending %u bytes to ", packetbuf_datalen());
  LOG_INFO_LLADDR(packetbuf_addr(PACKETBUF_ADDR_RECEIVER));
  LOG_INFO_("\n");
  NETSTACK_MAC.send(nullnet_mac_callback, ptr);
  return 1;
}
/*--------------------------------------------------------------------*/
const struct network_driver nullnet_driver = {
  "nullnet",
  init,
  input,
  output
};
/*--------------------------------------------------------------------*/
/** @} */
