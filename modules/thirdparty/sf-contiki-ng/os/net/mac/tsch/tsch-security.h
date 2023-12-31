/*
 * Copyright (c) 2014, SICS Swedish ICT.
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
 * \addtogroup tsch
 * @{
 * \file
 *	TSCH security
*/

#ifndef __TSCH_SECURITY_H__
#define __TSCH_SECURITY_H__

/********** Includes **********/

#include "contiki.h"
#include "net/mac/framer/frame802154.h"
#include "net/mac/framer/frame802154e-ie.h"
#include "net/mac/llsec802154.h"

/********** Configurarion *********/

/* To enable TSCH security:
* - set LLSEC802154_CONF_ENABLED
* - set LLSEC802154_CONF_USES_EXPLICIT_KEYS
* */

/* K1, defined in 6TiSCH minimal, is well-known (offers no security) and used for EBs only */
#ifdef TSCH_SECURITY_CONF_K1
#define TSCH_SECURITY_K1 TSCH_SECURITY_CONF_K1
#else /* TSCH_SECURITY_CONF_K1 */
#define TSCH_SECURITY_K1 { 0x36, 0x54, 0x69, 0x53, 0x43, 0x48, 0x20, 0x6D, 0x69, 0x6E, 0x69, 0x6D, 0x61, 0x6C, 0x31, 0x35 }
#endif /* TSCH_SECURITY_CONF_K1 */

/* K2, defined in 6TiSCH minimal, is used for all but EB traffic */
#ifdef TSCH_SECURITY_CONF_K2
#define TSCH_SECURITY_K2 TSCH_SECURITY_CONF_K2
#else /* TSCH_SECURITY_CONF_K2 */
#define TSCH_SECURITY_K2 { 0xde, 0xad, 0xbe, 0xef, 0xfa, 0xce, 0xca, 0xfe, 0xde, 0xad, 0xbe, 0xef, 0xfa, 0xce, 0xca, 0xfe }
#endif /* TSCH_SECURITY_CONF_K2 */

/* Key used for EBs */
#ifdef TSCH_SECURITY_CONF_KEY_INDEX_EB
#define TSCH_SECURITY_KEY_INDEX_EB TSCH_SECURITY_CONF_KEY_INDEX_EB
#else
#define TSCH_SECURITY_KEY_INDEX_EB 1 /* Use K1 as per 6TiSCH minimal */
#endif

/* Security level for EBs */
#ifdef TSCH_SECURITY_CONF_SEC_LEVEL_EB
#define TSCH_SECURITY_KEY_SEC_LEVEL_EB TSCH_SECURITY_CONF_SEC_LEVEL_EB
#else
#define TSCH_SECURITY_KEY_SEC_LEVEL_EB 1 /* MIC-32, as per 6TiSCH minimal */
#endif

/* Key used for ACK */
#ifdef TSCH_SECURITY_CONF_KEY_INDEX_ACK
#define TSCH_SECURITY_KEY_INDEX_ACK TSCH_SECURITY_CONF_KEY_INDEX_ACK
#else
#define TSCH_SECURITY_KEY_INDEX_ACK 2 /* Use K2 as per 6TiSCH minimal */
#endif

/* Security level for ACKs */
#ifdef TSCH_SECURITY_CONF_SEC_LEVEL_ACK
#define TSCH_SECURITY_KEY_SEC_LEVEL_ACK TSCH_SECURITY_CONF_SEC_LEVEL_ACK
#else
#define TSCH_SECURITY_KEY_SEC_LEVEL_ACK 5 /* Encryption + MIC-32, as per 6TiSCH minimal */
#endif

/* Key used for Other (Data, Cmd) */
#ifdef TSCH_SECURITY_CONF_KEY_INDEX_OTHER
#define TSCH_SECURITY_KEY_INDEX_OTHER TSCH_SECURITY_CONF_KEY_INDEX_OTHER
#else
#define TSCH_SECURITY_KEY_INDEX_OTHER 2 /* Use K2 as per 6TiSCH minimal */
#endif

/* Security level for Other (Data, Cmd) */
#ifdef TSCH_SECURITY_CONF_SEC_LEVEL_OTHER
#define TSCH_SECURITY_KEY_SEC_LEVEL_OTHER TSCH_SECURITY_CONF_SEC_LEVEL_OTHER
#else
#define TSCH_SECURITY_KEY_SEC_LEVEL_OTHER 5 /* Encryption + MIC-32, as per 6TiSCH minimal */
#endif

/********** Data types *********/

/* AES-128 key */
typedef uint8_t aes_key[16];

/********** Functions *********/
/**
 * \brief Set secret key of the given node address
 */
void tsch_security_set_sharedSecret(uint16_t addr, uint8_t* pSecret);

/**
 * \brief Return MIC length
 * \return The length of MIC (>= 0)
 */
unsigned int tsch_security_mic_len(frame802154_t *frame);

/**
 * \brief Protect a frame with encryption and/or MIC
 * \return The length of a generated MIC (>= 0)
 */
unsigned int tsch_security_secure_frame(uint8_t *hdr, uint8_t *outbuf,
                                        int hdrlen, int datalen,
                                        struct tsch_asn_t *asn);

/**
 * \brief Parse and check a frame protected with encryption and/or MIC
 * \retval 0 On error or security check failure (insecure frame)
 * \retval 1 On success or no need for security check (good frame)
 */
unsigned int tsch_security_parse_frame(const uint8_t *hdr, int hdrlen,
                                       int datalen, frame802154_t *frame,
                                       const linkaddr_t *sender,
                                       struct tsch_asn_t *asn);

/**
 * \brief Set packetbuf (or eackbuf) attributes depending on a given frame type
 * \param frame_type The frame type (FRAME802154_BEACONFRAME etc.)
 */
void tsch_security_set_packetbuf_attr(uint8_t frame_type);

#endif /* __TSCH_SECURITY_H__ */
/** @} */
