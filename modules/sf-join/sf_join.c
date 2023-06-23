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
 @brief      Implementation of the join frames handler.
*/

/*==============================================================================
                            INCLUDES
==============================================================================*/
/* Standart libraries */
#include <stdint.h>
#include <stddef.h>
/* Module specific headers. */
#include "sf_join.h"
#include "sf_joinFramer.h"
#include "sf_callbackHandler.h"
/* Stack specific includes. */
#include "net/nullnet/nullnet.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "net/mac/framer/frame802154.h"
/* Project configuration. */
#include "project-conf.h"
/* Log configuration */
#include "sys/log.h"

/*==============================================================================
                             MACROS
==============================================================================*/
#define LOG_MODULE "Join"
#ifndef LOG_CONF_JOIN
  #define LOG_LEVEL     LOG_LEVEL_NONE
#else
  #define LOG_LEVEL     LOG_CONF_JOIN
#endif
/* Define frame maximum length. */
#define SF_JOIN_FRAME_LENGTH_MAX                           (40U)

/*==============================================================================
                             STRUCTS
==============================================================================*/

/*==============================================================================
                          GLOBAL PARAMS
==============================================================================*/
/* Stores callback handler context parameters. */
static sf_callbackHandlerCtxt_t gJoinCallbackHandlerCtxt = {sf_join_output_callback,
                                                            NULL};
/* Stores the created frame bytes to be forwarded to the lower layer */
static uint8_t gpFrameBuffer[SF_JOIN_FRAME_LENGTH_MAX] = {0};
/* The stored frame length */
static uint8_t gFrameLen = 0;

/*==============================================================================
                         LOCAL FUNCTION DEFINITION
==============================================================================*/
/*============================================================================*/
/**
 * \brief Hand the frame over to TSCH to be sent at the corresponding slot.
 *
 * \param pAddr            Destination address.
 */
/*============================================================================*/
static void loc_sendFrame(linkaddr_t *pAddr);

/*============================================================================*/
/**
 * \brief Handle request frame.
 *
 * \param pReqFrame           Pointer to the request frame.
 * \param length              Length of the request frame.
 * \param pSrcAddr            Pointer to the device link address.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
static E_SF_RETURN_t loc_handleRequest(uint8_t *pReqFrame, uint16_t length,
                                       linkaddr_t* pSrcAddr);

/*============================================================================*/
/**
 * \brief Handle response frame.
 *
 * \param pRespFrame          Pointer to the response frame.
 * \param length              Length of the request frame.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
static E_SF_RETURN_t loc_handleResponse(uint8_t *pRespFrame, uint16_t length);

/*============================================================================*/
/**
 * \brief Handle successful frame.
 *
 * \param pSuccFrame          Pointer to the successful frame.
 * \param length              Length of the successful frame.
 * \param srcAddr             The device link address.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
static E_SF_RETURN_t loc_handleSuccessful(uint8_t *pSuccFrame, uint16_t length,
                                          linkaddr_t* pSrcAddr);

/*==============================================================================
                         LOCAL FUNCTION IMPLEMENTATION
==============================================================================*/
/*----------------------------------------------------------------------------*/
/*! loc_sendFrame */
/*----------------------------------------------------------------------------*/
static void loc_sendFrame(linkaddr_t *pAddr)
{
  nullnet_buf = gpFrameBuffer;
  nullnet_len = gFrameLen;

  /* Hand the frame over to the lower layer */
  NETSTACK_NETWORK.output(pAddr, (void*) &gJoinCallbackHandlerCtxt,
                          FRAME802154_JOINFRAME);
}/* loc_sendFrame() */

/*----------------------------------------------------------------------------*/
/*! loc_handleRequest */
/*----------------------------------------------------------------------------*/
static E_SF_RETURN_t loc_handleRequest(uint8_t *pReqFrame, uint16_t length,
                                       linkaddr_t* pSrcAddr)
{
  /* Return value. */
  E_SF_RETURN_t handleStatus = E_SF_SUCCESS;

  if(!pReqFrame || !pSrcAddr || SF_JOINFRAMER_REQUEST_LENGTH != length)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  LOG_INFO("Received join request message from ");
  LOG_INFO_LLADDR(pSrcAddr);
  LOG_INFO_(" : ");
  LOG_INFO_BYTES(pReqFrame, length);
  LOG_INFO_("\n");

  handleStatus = sf_joinFramer_parse_request(pReqFrame, length);
  if(E_SF_SUCCESS == handleStatus)
  {
    sf_join_request_receive(pSrcAddr);
  }
  else
  {
    LOG_INFO("Failed to parse the received join request message from ");
    LOG_INFO_LLADDR(pSrcAddr);
    LOG_INFO_(" !\n");
  }

  return handleStatus;
}/* loc_handleRequest() */

/*----------------------------------------------------------------------------*/
/*! loc_handleResponse */
/*----------------------------------------------------------------------------*/
static E_SF_RETURN_t loc_handleResponse(uint8_t *pRespFrame, uint16_t length)

{
  /* Return value. */
  E_SF_RETURN_t handleStatus = E_SF_SUCCESS;

  if(!pRespFrame || SF_JOINFRAMER_RESPONSE_LENGTH != length)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  LOG_INFO("Rx join response frame\n");
  LOG_INFO_(" : ");
  LOG_INFO_BYTES(pRespFrame, length);
  LOG_INFO_(" \n");

  handleStatus = sf_joinFramer_parse_response(pRespFrame, length);
  if(E_SF_SUCCESS == handleStatus)
  {
    sf_join_response_receive();
  }
  else
  {
    LOG_ERR("!Fail to parse join response \n");
  }

  return handleStatus;
}/* loc_handleResponse() */

/*----------------------------------------------------------------------------*/
/*! loc_handleSuccessful */
/*----------------------------------------------------------------------------*/
static E_SF_RETURN_t loc_handleSuccessful(uint8_t *pSuccFrame, uint16_t length,
                                          linkaddr_t* pSrcAddr)
{
  if(!pSrcAddr || !pSuccFrame || SF_JOINFRAMER_SUCCESSFUL_LENGTH != length)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  LOG_INFO("Received JoinSuccessful message from ");
  LOG_INFO_LLADDR(pSrcAddr);
  LOG_INFO_(" : ");
  LOG_INFO_BYTES(pSuccFrame, length);
  LOG_INFO_(" !\n");

  sf_join_successful_receive(pSrcAddr);

  return E_SF_SUCCESS;
}/* loc_handleSuccessful() */

/*==============================================================================
                      API FUNCTION IMPLEMENTATION
==============================================================================*/
/*----------------------------------------------------------------------------*/
/*! sf_join_request_send */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_join_request_send(linkaddr_t *pDestinationAddr)
{
  /* Return value. */
  E_SF_RETURN_t requestSent = E_SF_ERROR;

  if(NULL == pDestinationAddr)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  requestSent = sf_joinFramer_create_request(gpFrameBuffer);
  gFrameLen = SF_JOINFRAMER_REQUEST_LENGTH;

  /* Send join request. */
  if(E_SF_SUCCESS == requestSent)
  {
    LOG_INFO("Tx JoinReq to ");
    LOG_INFO_LLADDR(pDestinationAddr);
    LOG_INFO_(" : ");
    LOG_INFO_BYTES(gpFrameBuffer, gFrameLen);
    LOG_INFO_("\n");

    /* Set the max number of transmissions, 8 = 1 transmission and 7 retry. */
    packetbuf_set_attr(PACKETBUF_ATTR_MAX_MAC_TRANSMISSIONS, 8U);
    loc_sendFrame(pDestinationAddr);
    /* Set the max number of transmissions, 1 = 1 transmission and 0 retry. */
    packetbuf_set_attr(PACKETBUF_ATTR_MAX_MAC_TRANSMISSIONS, 1U);
  }

  return requestSent;
}/* sf_join_request_send() */

/*----------------------------------------------------------------------------*/
/*! sf_join_response_send */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_join_response_send(linkaddr_t *pDestinationAddr,
                                    linkaddr_t *pNewDeviceAddress)
{
  /* Return value. */
  E_SF_RETURN_t responseSent = E_SF_SUCCESS;

  if(NULL == pDestinationAddr || NULL == pNewDeviceAddress)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  responseSent = sf_joinFramer_create_response(gpFrameBuffer, pNewDeviceAddress);
  gFrameLen = SF_JOINFRAMER_RESPONSE_LENGTH;

  if(E_SF_SUCCESS == responseSent)
  {
    LOG_INFO("Send JoinResponse message to ");
    LOG_INFO_LLADDR(pDestinationAddr);
    LOG_INFO_(" : ");
    LOG_INFO_BYTES(gpFrameBuffer, SF_JOINFRAMER_RESPONSE_LENGTH);
    LOG_INFO_("\n");

    loc_sendFrame(pDestinationAddr);
  }

  return responseSent;
}/* sf_join_response_send() */

/*----------------------------------------------------------------------------*/
/*! sf_join_successful_send */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_join_successful_send(linkaddr_t *pDestinationAddr)
{
  /* Return value. */
  E_SF_RETURN_t successfulSent = E_SF_ERROR;

  if(NULL == pDestinationAddr)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  successfulSent = sf_joinFramer_create_successful(gpFrameBuffer);
  gFrameLen = SF_JOINFRAMER_SUCCESSFUL_LENGTH;

  if(E_SF_SUCCESS == successfulSent)
  {
    LOG_INFO("Tx JoinSuc to ");
    LOG_INFO_LLADDR(pDestinationAddr);
    LOG_INFO_(" : ");
    LOG_INFO_BYTES(gpFrameBuffer, gFrameLen);
    LOG_INFO_("\n");

    /* Set the max number of transmissions, 8 = 1 transmission and 7 retry. */
    packetbuf_set_attr(PACKETBUF_ATTR_MAX_MAC_TRANSMISSIONS, 8U);
    loc_sendFrame(pDestinationAddr);
    /* Set the max number of transmissions, 1 = 1 transmission and 0 retry. */
    packetbuf_set_attr(PACKETBUF_ATTR_MAX_MAC_TRANSMISSIONS, 1U);
  }

  return E_SF_SUCCESS;
}/* sf_join_successful_send() */

/*----------------------------------------------------------------------------*/
/*! sf_join_handleFrame */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_join_handleFrame(uint8_t* pInBuf, uint16_t length,
                                  linkaddr_t* pSrcAddr)
{
  /* Return value. */
  E_SF_RETURN_t handleStatus = E_SF_ERROR;
  /* Frame type */
  E_FRAME_TYPE_t frameType = E_FRAME_TYPE_UNDEFINED;

  if(NULL == pInBuf || SF_JOIN_FRAME_LENGTH_MAX < length)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  handleStatus = sf_frameType_get(pInBuf, &frameType);
  if(E_SF_SUCCESS == handleStatus)
  {
    switch(frameType)
    {
      case E_FRAME_TYPE_REQUEST:
        LOG_INFO("Received join request frame;\n ");
        handleStatus = loc_handleRequest(pInBuf, length, pSrcAddr);
        break;

      case E_FRAME_TYPE_RESPONSE:
        LOG_INFO("Received join response frame;\n ");
        handleStatus = loc_handleResponse(pInBuf, length);
        break;

      case E_FRAME_TYPE_SUCCESSFUL:
        LOG_INFO("Received join successful frame;\n ");
        handleStatus = loc_handleSuccessful(pInBuf, length, pSrcAddr);
        break;

      default:
        LOG_ERR("Received invalid join frame;\n");
        break;
      }
  }

  return handleStatus;
}/* sf_join_handleFrame() */

/*==============================================================================
                  INDICATION DUMMY IMPLEMENTATION
==============================================================================*/
/*------------------------------------------------------------------------------
  sf_join_request_receive()
------------------------------------------------------------------------------*/
__attribute__((weak))  void sf_join_request_receive(linkaddr_t *src)
{

} /* sf_join_request_receive()*/

/*------------------------------------------------------------------------------
  sf_join_response_receive()
------------------------------------------------------------------------------*/
__attribute__((weak))  void sf_join_response_receive(void)
{

} /* sf_join_response_receive()*/

/*------------------------------------------------------------------------------
  sf_join_successful_receive()
------------------------------------------------------------------------------*/
__attribute__((weak))  void sf_join_successful_receive(linkaddr_t *src)
{

} /* sf_join_successful_receive()*/

#ifdef __cplusplus
}
#endif
