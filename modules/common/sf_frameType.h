#ifdef __cplusplus
extern "C" {
#endif

#ifndef __SF_FRAME_TYPE_H__
#define __SF_FRAME_TYPE_H__

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
 @brief      This header contains information for using the join frame type
             module.
*/

/*==============================================================================
                            INCLUDES
==============================================================================*/
/* Standart libraries */
#include <stdint.h>
#include <stdbool.h>

/* Stackforce types. */
#include "sf_types.h"

/*==============================================================================
                             MACROS
==============================================================================*/
#define SF_FRAME_TYPE_LEN       0x01

/*==============================================================================
                             ENUM
==============================================================================*/
/*! Defines the join frame types. */
typedef enum
{
  /* Join request frame type.
    This frame is issued by a sensor
    to join gateway network.*/
  E_FRAME_TYPE_REQUEST = 0,
  /* Join response frame type.
    This frame is issued by a gateway
    to confirm join request.*/
  E_FRAME_TYPE_RESPONSE = 1,
  /* Join successful frame type.
    This frame is issued by a sensor
    to end-up the joining process.*/
  E_FRAME_TYPE_SUCCESSFUL = 2,
  /* Measurement frame type.
     Used for transmitting
     measurement data. */
  E_FRAME_TYPE_MEASUREMENT = 3,
  /* Remote frame type.
     Used for transmitting config/cmd
     to the endpoints. */
  E_FRAME_TYPE_REMOTE = 4,
  /* Invalid frame type. */
  E_FRAME_TYPE_UNDEFINED
} E_FRAME_TYPE_t;

/*==============================================================================
                             STRUCTS
==============================================================================*/

/*==============================================================================
                            FUNCTION PROTOTYPES
==============================================================================*/
/*============================================================================*/
/**
 * \brief Label the frame by adding frame type identifier.
 *
 * \param pBuf          Pointer to the frame.
 * \param frameType     Frame type.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
E_SF_RETURN_t sf_frameType_set(uint8_t* pBuf, E_FRAME_TYPE_t frameType);

/*============================================================================*/
/**
 * \brief Parse the frame type identifier.
 *
 * \param pBuf           Pointer to the frame.
 * \param pFrameType     Pointer to the parsed frame type identifier.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
E_SF_RETURN_t sf_frameType_get(const uint8_t* pBuf, E_FRAME_TYPE_t *pFrameType);

#endif /* __SF_FRAME_TYPE_H__ */

#ifdef __cplusplus
}
#endif
