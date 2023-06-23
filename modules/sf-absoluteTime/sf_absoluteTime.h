#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief      API definition of the absolute time module.
 *
 * @details    The absolute time is synchronized between the BMSCC and
 *             the smart cells. The time base should be set by the user
 *             at the startup time of the BMSCC.
 *
 */

#ifndef SF_ABSOLUTE_TIME_H_
#define SF_ABSOLUTE_TIME_H_

/**
 *  @addtogroup SF_ABSOLUTE_TIME
 *
 *  @details
 *
 *  - <b>ABSOLUTE TIME API</b>\n
 *    | API Function                              | Description                                     |
 *    |-------------------------------------------|-------------------------------------------------|
 *    | @ref sf_absoluteTime_getSeconds()         | @copybrief sf_absoluteTime_getSeconds()         |
 *    | @ref sf_absoluteTime_getMinutes()         | @copybrief sf_absoluteTime_getMinutes()         |
 *    | @ref sf_absoluteTime_getHours()           | @copybrief sf_absoluteTime_getHours()           |
 *    | @ref sf_absoluteTime_getTime()            | @copybrief sf_absoluteTime_getTime()            |
 *    | @ref sf_absoluteTime_startTimer()         | @copybrief sf_absoluteTime_startTimer()         |
 *  @{
 */

/*==============================================================================
                            INCLUDES
==============================================================================*/
/* Standard include */
#include <stdint.h>
/* Application include */
#include "sf_types.h"

/*==============================================================================
                          API FUNCTIONS
==============================================================================*/
/*============================================================================*/
/**
 * \brief Returns seconds out of the given timestamp value.
 *
 * \param timestamp
 *
 * \return seconds
 */
/*============================================================================*/
uint8_t sf_absoluteTime_getSeconds(uint32_t timestamp);

/*============================================================================*/
/**
 * \brief Returns minutes out of the given timestamp value.
 *
 * \param timestamp
 *
 * \return minutes
 */
/*============================================================================*/
uint8_t sf_absoluteTime_getMinutes(uint32_t timestamp);

/*============================================================================*/
/**
 * \brief Returns hours out of the given timestamp value.
 *
 * \param timestamp
 *
 * \return hours
 */
/*============================================================================*/
uint8_t sf_absoluteTime_getHours(uint32_t timestamp);

/*============================================================================*/
/**
 * \brief Fetch the current absolute time.
 *
 * \return absolute time
 */
/*============================================================================*/
uint32_t sf_absoluteTime_getTime(void);

/*============================================================================*/
/**
 * \brief Starts absolute time incrementing.
 *
 * \return @ref E_SF_RETURN_t
 */
/*============================================================================*/
E_SF_RETURN_t sf_absoluteTime_startTimer(void);

/*! @} */

#endif /* SF_ABSOLUTE_TIME_H_ */

#ifdef __cplusplus
}
#endif
