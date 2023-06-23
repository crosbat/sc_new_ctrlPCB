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
 @brief      This header contains the join mode interface.
*/

#ifndef __SF_JOINREQUESTER_H__
#define __SF_JOINREQUESTER_H__

/**
 *  @addtogroup SF_JOIN_REQUESTER
 *
 *  @details
 *
 *  - <b>SF JOIN REQUESTER API</b>\n
 *    | API Function                              | Description                                     |
 *    |-------------------------------------------|-------------------------------------------------|
 *    | @ref sf_joinRequester_join()              | @copybrief sf_joinRequester_join()             |
 *    | @ref sf_joinRequester_rejoin()            | @copybrief sf_joinRequester_rejoin()           |
 *  @{
 */

/*=============================================================================
                                API FUNCTIONS
=============================================================================*/
/*============================================================================*/
/**
 * \brief Join to a network that offers the best RSSI. It will search for
 *        a gateway with join window open then perform the handshake procedure
 *        to get identified within the network. If it fails, it will go into
 *        shutdown mode.
 */
/*============================================================================*/
void sf_joinRequester_join(void);

/*============================================================================*/
/**
 * \brief Rejoin to the same last network it was connected to. It will try
 *        to connect to the same gateway. If it fails, it will go into backoff
 *        time then retry again.
 */
/*============================================================================*/
void sf_joinRequester_rejoin(void);

/*! @} */

#endif /* __SF_JOINREQUESTER_H__ */

#ifdef __cplusplus
}
#endif
