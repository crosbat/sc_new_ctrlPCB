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
 @brief      Implementation of the join frame type logic.
*/

/*==============================================================================
                            INCLUDES
==============================================================================*/
/* Standart libraries */
#include <stdint.h>
#include <stddef.h>

/* Module specific headers. */
#include "sf_frameType.h"

/*==============================================================================
                             MACROS
==============================================================================*/
/* The frame type field offset. */
#define SF_JOIN_FRAMER_TYPE_FIELD_OFFSET        (0U)

/*==============================================================================
                      API FUNCTION IMPLEMENTATION
==============================================================================*/
/*----------------------------------------------------------------------------*/
/*! sf_frameType_set */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_frameType_set(uint8_t* pBuf, E_FRAME_TYPE_t frameType)
{
  E_SF_RETURN_t typeSet = E_SF_SUCCESS;

  if(NULL == pBuf)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  pBuf[SF_JOIN_FRAMER_TYPE_FIELD_OFFSET] = frameType;

  return typeSet;
}/* sf_frameType_set() */

/*----------------------------------------------------------------------------*/
/*! sf_frameType_get */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_frameType_get(const uint8_t* pBuf, E_FRAME_TYPE_t *pFrameType)
{
  E_SF_RETURN_t typeGet = E_SF_SUCCESS;

  if(NULL == pBuf)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  *pFrameType = (E_FRAME_TYPE_t)pBuf[SF_JOIN_FRAMER_TYPE_FIELD_OFFSET];

  return typeGet;
}/* sf_frameType_get() */

#ifdef __cplusplus
}
#endif

