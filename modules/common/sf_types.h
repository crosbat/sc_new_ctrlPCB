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
 * @brief      Type definitions for STACKFORCE implementations.
 *
 */

#ifndef __SF_TYPES_H__
#define __SF_TYPES_H__

/**
 *  @addtogroup SF_TYPES
 *
 *  @details
 *  @{
 */

/*==============================================================================
                            TYPEDEF ENUMS
==============================================================================*/
/*! Definition of generic return codes. */
typedef enum
{
  /*!
    * 0x00 : Return code success
    */
  E_SF_SUCCESS = 0x00,
  /*!
    * 0x01 : Return code general error
    */
  E_SF_ERROR = 0x01,
  /*!
    * 0x02 : Return code for null pointer exception (NPE) error
    */
  E_SF_ERROR_NPE = 0x02,
  /*!
    * 0x03 : Return code invalid parameter
    */
  E_SF_ERROR_INVALID_PARAM = 0x03,
  /*!
    * 0x04 : Return code busy error
    */
  E_SF_ERROR_BUSY = 0x04,
}E_SF_RETURN_t;

/*! Definition join beacon flag. */
typedef enum
{
  /*!
    * 0x00 : Manual join disabled.
    */
  E_SF_JOIN_BEACON_DISABLED = 0x00,
  /*!
    * 0x01 : Manual join enabled.
    */
  E_SF_JOIN_BEACON_ENABLED = 0x01,
}E_SF_JOIN_BEACON_t;

/*! @} */

#endif /* __SF_TYPES_H__ */
#ifdef __cplusplus
}
#endif
