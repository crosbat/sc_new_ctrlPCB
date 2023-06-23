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
 * \ingroup net-layer
 * \addtogroup nullnet
A network layer that does nothing. Useful for lower-layer testing and
for non-IPv6 scenarios.
 * @{
 */

#ifndef NULLNET_H_
#define NULLNET_H_

#include "contiki.h"
#include "net/linkaddr.h"

/**
 * Buffer used by the output function
*/
extern uint8_t *nullnet_buf;
extern uint16_t nullnet_len;

/* Generic NULLNET return values. */
typedef enum {
  /**< The transmission was OK. */
  NULLNET_TX_OK,

  /**< The lower layer transmission could not be performed due to a
     collision. */
  NULLNET_TX_COLLISION,

  /**< The lower layer did not get an acknowledgement for the packet. */
  NULLNET_TX_NOACK,

  /**< The lower layer deferred the transmission for a later time. */
  NULLNET_TX_DEFERRED,

  /**< The lower layer transmission could not be performed because of an
     error. The upper layer may try again later. */
  NULLNET_TX_ERR,

  /**< The lower layer transmission could not be performed because of a
     fatal error. The upper layer does not need to try again, as the
     error will be fatal then as well. */
  NULLNET_TX_ERR_FATAL,

  /**< The NULLNET layer is busy. A packet is already scheduled. */
  NULLNET_TX_BUSY,
} nullnet_tx_status_t;

/**
 * Function prototype for NullNet input callback
*/
typedef void (* nullnet_input_callback_t)(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest);

/**
 * Function prototype for NullNet ouput callback
*/
typedef void (* nullnet_output_callback_t)(void *ptr, nullnet_tx_status_t status, uint8_t ack_app_data);

/**
 * Set input callback for NullNet
 *
 * \param callback The input callback
*/
void nullnet_set_input_callback(nullnet_input_callback_t callback);

/**
 * Set ouput callback for NullNet
 *
 * \param callback The output callback
*/
void nullnet_set_output_callback(nullnet_output_callback_t callback);

#if NULLNET_SINGLE_PACKET_ENABLED
/**
 * Restrict the number of queued packets to only one
*/
void nullnet_enable_single_packet(void);

/**
 * Allow the configured number of queued packets
*/
void nullnet_disable_single_packet(void);
#endif /* NULLNET_SINGLE_PACKET_ENABLED */

#endif /* NULLNET_H_ */
/** @} */
