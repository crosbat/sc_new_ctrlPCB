#ifdef __cplusplus
extern "C" {
#endif

#ifndef __SF_JOIN_FRAMER_H__
#define __SF_JOIN_FRAMER_H__

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
 @brief      This header contains information for using the join framer
             module.
  This module creates all the frames needed during a joining process.
    + Join request frame
    + Join response frame
    + Join successful frame
 In addition to that, this module can parse all kind of frames mentioned above.
 The parsed data will be formatted into struct data type.
    + @ref joinRequest_t
    + @ref joinResponse_t
    + @ref joinSuccessful_t
*/

/*==============================================================================
                            INCLUDES
==============================================================================*/
/* Standart libraries */
#include <stdint.h>
#include <stdbool.h>
/* Stack include */
#include "net/linkaddr.h"
/* Application specific includes */
#include "sf_frameType.h"
#include "sf_types.h"

/*==============================================================================
                             MACROS
==============================================================================*/
/* Defines serial number parameter length. */
#define SF_JOINFRAMER_SERIALNUMBER_LENGTH        sizeof(uint32_t)
/* Link address length */
#define SF_JOINFRAMER_SHORTADDR_LENGTH           (2U)
/* Define the maximum join request frame length.
    req len = serial number + frame type field
            = 4 + 1 = 5 */
#define SF_JOINFRAMER_REQUEST_LENGTH             SF_JOINFRAMER_SERIALNUMBER_LENGTH + \
                                                 SF_FRAME_TYPE_LEN
/* Length of join response frame.
    resp len = assigned device address + frame type field.
             = 2 + 1 = 3 */
#define SF_JOINFRAMER_RESPONSE_LENGTH            SF_JOINFRAMER_SHORTADDR_LENGTH + \
                                                 SF_FRAME_TYPE_LEN
/* Length of join successful frame.
    succ len = frame type field.
             = 1 */
#define SF_JOINFRAMER_SUCCESSFUL_LENGTH          SF_FRAME_TYPE_LEN

/*==============================================================================
                             ENUM
==============================================================================*/

/*==============================================================================
                             STRUCTS
==============================================================================*/
/*! Store the join request frame parameters. */
typedef struct
{
  /*! Device serial number */
  uint32_t serialNumber;
} joinRequest_t;

/*! Store the join response frame parameters. */
typedef struct
{
  /*! Store the assigned id to the joined device. */
  uint16_t deviceId;
} joinResponse_t;

/*==============================================================================
                            FUNCTION PROTOTYPES
==============================================================================*/
/*============================================================================*/
/**
 * \brief Create a join request frame.
 *
 * \param pOutBuf             Pointer to the join request frame.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
E_SF_RETURN_t sf_joinFramer_create_request(uint8_t* pOutBuf);

/*============================================================================*/
/**
 * \brief Create a join response frame.
 *
 * \param pOutBuf              Pointer to the join response frame.
 * \param pNewDeviceAddress    Pointer to the assigned address.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
E_SF_RETURN_t sf_joinFramer_create_response(uint8_t* pOutBuf,
                                            linkaddr_t *pNewDeviceAddress);

/*============================================================================*/
/**
 * \brief Create a join successful frame.
 *
 * \param pOutBuf             Pointer to the frame buffer.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
E_SF_RETURN_t sf_joinFramer_create_successful(uint8_t* pOutBuf);

/*============================================================================*/
/**
 * \brief Parse the join request frame and format it into a structure.
 *
 * \param pInBuf              Pointer to the join request frame.
 * \param length              Join request frame length.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
E_SF_RETURN_t sf_joinFramer_parse_request(uint8_t *pInBuf, uint16_t length);

/*============================================================================*/
/**
 * \brief Parse the join response frame and format it into a structure.
 *
 * \param pInBuf                Pointer to the join response frame.
 * \param length                Join response frame length.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
E_SF_RETURN_t sf_joinFramer_parse_response(uint8_t *pInBuf, uint16_t length);

/*============================================================================*/
/**
 * \brief Return join request frame paramters
 *
 * \return @ref joinRequest_t
 */
/*============================================================================*/
joinRequest_t sf_joinFramer_getRequestParams(void);


/*============================================================================*/
/**
 * \brief Return join response frame parameters
 *
 * \return @ref joinResponse_t
 */
/*============================================================================*/
joinResponse_t sf_joinFramer_getResponseParams(void);

#endif /* __SF_JOIN_FRAMER_H__ */
#ifdef __cplusplus
}
#endif
