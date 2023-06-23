#ifdef __cplusplus
extern "C" {
#endif

#ifndef __SF_JOIN_H__
#define __SF_JOIN_H__

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
 @brief      This header contains information for using the join module.
*/

/*==============================================================================
                            INCLUDES
==============================================================================*/
/* Standart libraries */
#include <stdint.h>
#include <stdbool.h>
/* Stack includes */
#include "net/linkaddr.h"
#include "net/nullnet/nullnet.h"
/* Module specific headers. */
#include "sf_frameType.h"
#include "sf_types.h"
#include "sf_joinFramer.h"

/*==============================================================================
                            FUNCTION PROTOTYPES
==============================================================================*/
/*============================================================================*/
/**
 * \brief Send-out a join request frame.
 *
 * \param pDestinationAddr       Pointer to the destination address.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
E_SF_RETURN_t sf_join_request_send(linkaddr_t *pDestinationAddr);

/*============================================================================*/
/**
 * \brief Send-out a join response frame to the giver address.
 *
 * \param pDestinationAddr       Pointer to the destination address.
 * \param pNewDeviceAddress      Pointer to the assigned address.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
E_SF_RETURN_t sf_join_response_send(linkaddr_t *pDestinationAddr,
                                    linkaddr_t *pNewDeviceAddress);

/*============================================================================*/
/**
 * \brief Send-out a join successful frame.
 *
 * \param pDestinationAddr       Pointer to the destination address.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
E_SF_RETURN_t sf_join_successful_send(linkaddr_t *pDestinationAddr);

/*============================================================================*/
/**
 * \brief Handle join frame. The frame can be one of the following:
 *          - join request
 *          - join response
 *          - join successful
 *
 * \param pInBuf        Pointer to the frame.
 * \param length        Frame length.
 * \param pSrcAddr      Pointer to the device link address.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
E_SF_RETURN_t sf_join_handleFrame(uint8_t* pInBuf, uint16_t length,
                                  linkaddr_t* pSrcAddr);

/*==============================================================================
                            INDICATIONS
==============================================================================*/
/*============================================================================*/
/**
 * @brief Informs the upper layer about new join request reception.
 *        This function is implemented by the application.
 *
 * @param	pSrcAddr Pointer to received frame source address.
 */
/*============================================================================*/
void sf_join_request_receive(linkaddr_t *pSrcAddr);

/*============================================================================*/
/**
 * \brief Informs the upper layer that join response is received from the
 *        beacon source. This function is implemented by the application.
 */
/*============================================================================*/
void sf_join_response_receive(void);

/*============================================================================*/
/**
 * @brief Informs the upper layer about new join request reception.
 *        This function is implemented by the application.
 *
 * @param	pSrcAddr Pointer to received frame source address.
 */
/*============================================================================*/
void sf_join_successful_receive(linkaddr_t *pSrcAddr);

/*==============================================================================
                             CALLBACKS
==============================================================================*/
/*============================================================================*/
/**
 * \brief This function is called by TSCH to forward the join packet Tx status.
 *
 * \param ptr         Pointer to join data.
 * \param status      Status of nullnet Tx.
 */
/*============================================================================*/
void sf_join_output_callback(void *ptr, nullnet_tx_status_t status);

#endif /* __SF_JOIN_H__ */

#ifdef __cplusplus
}
#endif
