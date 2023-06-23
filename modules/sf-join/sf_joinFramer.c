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
 @brief      Implementation of the join framer interface.
*/

/*==============================================================================
                            INCLUDES
==============================================================================*/
/* Standart libraries */
#include <stdint.h>
#include <stddef.h>
#include <string.h>
/* Stack specific includes */
#include "sys/log.h"
#if CONTIKI_TARGET_COOJA
#include <dev/moteid.h>
#endif
/* Module specific headers. */
#include "sf_joinFramer.h"
#include "sf_frameType.h"
#include "sf_configMgmt.h"



/*==============================================================================
                             MACROS
==============================================================================*/
#define LOG_MODULE "Join"
#ifndef LOG_CONF_JOIN
  #define LOG_LEVEL     LOG_LEVEL_NONE
#else
  #define LOG_LEVEL     LOG_CONF_JOIN
#endif
/* Converts an uint8_t array to an uint16_t. */
#define UINT8_TO_UINT16(x)                (uint16_t)(((uint16_t)((x)[0])<<8) + \
                                          (uint16_t)((x)[1]))

/*==============================================================================
                          GLOBAL PARAMS
==============================================================================*/

/* Join request frame parameters. */
static joinRequest_t gJoinReqParams = {0};
/* Join response frame parameters. */
static joinResponse_t gJoinRespParams = {0};

/*==============================================================================
                      API FUNCTION IMPLEMENTATION
==============================================================================*/
/*----------------------------------------------------------------------------*/
/*! sf_joinFramer_create_request */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_joinFramer_create_request(uint8_t* pOutBuf)
{
  E_SF_RETURN_t requestCreated = E_SF_ERROR;

  if(NULL == pOutBuf)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  /* Create join request. */
#if CONTIKI_TARGET_COOJA
  gJoinReqParams.serialNumber = simMoteID;
  requestCreated = E_SF_SUCCESS;
#else
  requestCreated = sf_configMgmt_getParam(&gJoinReqParams.serialNumber,
                                          sizeof(gJoinReqParams.serialNumber),
                                          E_CONFIGMGMT_PARAM_DEVICE_SERIAL);
#endif

  if(E_SF_SUCCESS == requestCreated)
  {
    requestCreated = sf_frameType_set(pOutBuf, E_FRAME_TYPE_REQUEST);
    memcpy(pOutBuf + SF_FRAME_TYPE_LEN, &gJoinReqParams,
           sizeof(gJoinReqParams));
  }

  return requestCreated;
}/* sf_joinFramer_create_request() */

/*----------------------------------------------------------------------------*/
/*! sf_joinFramer_create_response */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_joinFramer_create_response(uint8_t* pOutBuf,
                                            linkaddr_t *pNewDeviceAddress)
{
  /* Return value. */
  E_SF_RETURN_t responseCreated = E_SF_SUCCESS;

  if(NULL == pOutBuf || NULL == pNewDeviceAddress)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  gJoinRespParams.deviceId = pNewDeviceAddress->u16;

  responseCreated = sf_frameType_set(pOutBuf, E_FRAME_TYPE_RESPONSE);

  memcpy(pOutBuf + SF_FRAME_TYPE_LEN, (uint8_t*)&gJoinRespParams,
        sizeof(gJoinRespParams));

  return responseCreated;
}/* sf_joinFramer_create_response() */

/*----------------------------------------------------------------------------*/
/*! sf_joinFramer_create_successful */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_joinFramer_create_successful(uint8_t* pOutBuf)
{
  E_SF_RETURN_t successfulCreated = E_SF_ERROR;

  if(NULL == pOutBuf)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  successfulCreated = sf_frameType_set(pOutBuf, E_FRAME_TYPE_SUCCESSFUL);

  return successfulCreated;
}/* sf_joinFramer_create_successful() */

/*----------------------------------------------------------------------------*/
/*! sf_joinFramer_parse_request */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_joinFramer_parse_request(uint8_t *pInBuf, uint16_t length)
{
  if((NULL == pInBuf) || (SF_JOINFRAMER_REQUEST_LENGTH != length))
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  pInBuf += SF_FRAME_TYPE_LEN;

  memcpy((uint8_t*)&gJoinReqParams, pInBuf, length);

  return E_SF_SUCCESS;
}/* sf_joinFramer_parse_request() */

/*----------------------------------------------------------------------------*/
/*! sf_joinFramer_parse_response */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_joinFramer_parse_response(uint8_t *pInBuf, uint16_t length)
{
  E_SF_RETURN_t responseParsed = E_SF_SUCCESS;

  if((NULL == pInBuf) || (SF_JOINFRAMER_RESPONSE_LENGTH != length))
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  pInBuf += SF_FRAME_TYPE_LEN;

  gJoinRespParams.deviceId = UINT8_TO_UINT16(pInBuf);

  return responseParsed;
}/* sf_joinFramer_parse_response() */

/*----------------------------------------------------------------------------*/
/*! sf_joinFramer_getRequestParams */
/*----------------------------------------------------------------------------*/
joinRequest_t sf_joinFramer_getRequestParams(void)
{
  return gJoinReqParams;
}/* sf_joinFramer_getRequestParams() */

/*----------------------------------------------------------------------------*/
/*! sf_joinFramer_getResponseParams */
/*----------------------------------------------------------------------------*/
joinResponse_t sf_joinFramer_getResponseParams(void)
{
  return gJoinRespParams;
}/* sf_joinFramer_getResponseParams() */

#ifdef __cplusplus
}
#endif

