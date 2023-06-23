#ifdef __cplusplus
extern "C" {
#endif

#ifndef __SF_STATE_MANAGER_H__
#define __SF_STATE_MANAGER_H__

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
 @brief      This header contains information for using the state manager
             module.
*/

/*==============================================================================
                            INCLUDES
==============================================================================*/
/* Standart libraries */
#include <stdint.h>
#include <stdbool.h>

/* Satckforce types. */
#include "sf_types.h"

/*==============================================================================
                             FUNCTION TYPE
==============================================================================*/
/*! Function prototype of state functions. */
typedef void (*stateManager_stateFct_t)( void );

/*==============================================================================
                             STRUCTS
==============================================================================*/
/* Store the state parameters. */
typedef struct
{
  /* Store the state identifier. */
  uint8_t state;
  /* Point to the current state function. */
  stateManager_stateFct_t stateFct;
} stateManager_state_t;

/* Stores module settings. */
typedef struct
{
  /* Pointer to the states (state id, state function). */
  stateManager_state_t *pStates;
  /* Stores the number of the states. */
  uint8_t statesCount;
  /* Stores the current state. */
  uint8_t currentState;
}stateManager_ctx_t;

/*==============================================================================
                            FUNCTION PROTOTYPES
==============================================================================*/
/*============================================================================*/
/**
 * \brief Initialize the state manager by providing the states functions,
 *        states name and number of the total number of the states.
 *
 * \param pCtx                 Pointer to the context storage
 * \param pStates              Array of the states (@ref stateManager_state_t).
 * \param statesCount          The number of the states.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
E_SF_RETURN_t sf_stateManager_init(stateManager_ctx_t* pCtx,
                                   stateManager_state_t pStates[],
                                   uint8_t statesCount);

/*============================================================================*/
/**
 * \brief Set the current state.
 *
 * \param pCtx                 Pointer to the context storage
 * \param state                The state.
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
E_SF_RETURN_t sf_stateManager_setState(stateManager_ctx_t* pCtx, uint8_t state);

/*============================================================================*/
/**
 * \brief Get the current state.
 *
 * \param pCtx                 Pointer to the context storage
 *
 * \return the current state.
 */
/*============================================================================*/
uint8_t sf_stateManager_getState(stateManager_ctx_t* pCtx);

/*============================================================================*/
/**
 * \brief Call the current state function.
 *
 * \param pCtx                 Pointer to the context storage
 *
 * \return @ref E_SF_RETURN_t.
 */
/*============================================================================*/
E_SF_RETURN_t sf_stateManager_execState(stateManager_ctx_t* pCtx);

#endif /* __SF_STATE_MANAGER_H__ */

#ifdef __cplusplus
}
#endif
