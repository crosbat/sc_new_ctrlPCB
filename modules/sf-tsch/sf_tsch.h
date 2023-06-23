#ifdef __cplusplus
extern "C" {
#endif

#ifndef __SF_TSCH_H__
#define __SF_TSCH_H__

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
 @brief      Abstraction layer of communication stack.
*/

/**
 *  @addtogroup SF_TSCH
 *
 *  @details
 *
 *  - <b>TSCH abstraction API</b>\n
 *    | API Function                              | Description                                     |
 *    |-------------------------------------------|-------------------------------------------------|
 *    | @ref sf_tsch_init()                       | @copybrief sf_tsch_init()                       |
 *    | @ref sf_tsch_start()                      | @copybrief sf_tsch_start()                      |
 *    | @ref sf_tsch_stop()                       | @copybrief sf_tsch_stop()                       |
 *    | @ref sf_tsch_setTxPowerMax()              | @copybrief sf_tsch_setTxPowerMax()              |
 *    | @ref sf_tsch_setDeviceAddress()           | @copybrief sf_tsch_setDeviceAddress()           |
 *    | @ref sf_tsch_addDataSlots()               | @copybrief sf_tsch_addDataSlots()               |
 *    | @ref sf_tsch_send()                       | @copybrief sf_tsch_send()                       |
 *  @{
 */

/*==============================================================================
                               ENUMS
==============================================================================*/
#include "sf_callbackHandler.h"
#include "sf_types.h"

/*==============================================================================
                            FUNCTION PROTOTYPES
==============================================================================*/
/*============================================================================*/
/**
 * \brief Initialize TSCH.
 */
/*============================================================================*/
void sf_tsch_init(void);

/*============================================================================*/
/**
 * \brief Start MAC as end point.
 */
/*============================================================================*/
void sf_tsch_start(void);

/*============================================================================*/
/**
 * \brief Stop MAC.
 */
/*============================================================================*/
void sf_tsch_stop(void);

/*============================================================================*/
/**
 * \brief Set Tx power to the maximum value.
 */
/*============================================================================*/
void sf_tsch_setTxPowerMax(void);

/*============================================================================*/
/**
 * \brief Set device link address (short address) into the stack.
 *
 * \return @ref E_SF_RETURN_t
 */
/*============================================================================*/
E_SF_RETURN_t sf_tsch_setDeviceAddress(linkaddr_t* pAddr);

/*============================================================================*/
/**
 * \brief Add data time-slots for the given sensor address to the schedule.
 *        This slots are used for regular communication.
 *
 * \param pAddr    Pointer to sensor link address. This should be the assigned
 *                 address by the gateway after successful manual join.
 *
 * \return @ref E_SF_RETURN_t
 */
/*============================================================================*/
E_SF_RETURN_t sf_tsch_addDataSlots(const linkaddr_t* pAddr);

/*============================================================================*/
/**
 * \brief Schedule frame transmission to the destination address.
 *        TSCH waits for the Tx slot to send the frame. It raises the callback,
 *        as the frame is processed, to indicate about the status of the
 *        transmission.
 *
 * \param pFrame               Pointer to the frame storage.
 * \param frameLen             Frame length
 * \param pDestAddr            Pointer to the destination link address.
 * \param pCallbackHandlerCtx  Pointer to the callback handler context
 *
 * \return @ref E_SF_RETURN_t
 */
/*============================================================================*/
E_SF_RETURN_t sf_tsch_send(uint8_t* pFrame, uint8_t frameLen,
                           linkaddr_t* pDestAddr,
                           sf_callbackHandlerCtxt_t* pCallbackHandlerCtx);

/*! @} */

#endif /* __SF_TSCH_H__ */

#ifdef __cplusplus
}
#endif
