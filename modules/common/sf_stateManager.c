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
 @brief      Implementation of the state manager logic.
*/

/*==============================================================================
                            INCLUDES
==============================================================================*/
/* Standart libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Module specific include. */
#include "sf_stateManager.h"

/*==============================================================================
                             MACROS
==============================================================================*/
/* Define invalid state. */
#define SF_STATE_MANAGER_INVALID_STATE     (0xFF)

/*==============================================================================
                      API FUNCTION IMPLEMENTATION
==============================================================================*/
/*----------------------------------------------------------------------------*/
/*! sf_stateManager_init */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_stateManager_init(stateManager_ctx_t* pCtx,
                                   stateManager_state_t pStates[],
                                   uint8_t statesCount)
{
  /* Return value. */
  E_SF_RETURN_t initStatus = E_SF_SUCCESS;

  if((NULL == pCtx) || (NULL == pStates))
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  pCtx->pStates = pStates;
  pCtx->statesCount = statesCount;
  pCtx->currentState = SF_STATE_MANAGER_INVALID_STATE;

  return initStatus;
}/* sf_stateManager_init() */

/*----------------------------------------------------------------------------*/
/*! sf_stateManager_setState */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_stateManager_setState(stateManager_ctx_t* pCtx, uint8_t state)
{
  /* Return value. */
  E_SF_RETURN_t setStatus = E_SF_SUCCESS;

  if((NULL == pCtx) || (state >= pCtx->statesCount))
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  pCtx->currentState = state;

  return setStatus;
}/* sf_stateManager_setState() */

/*----------------------------------------------------------------------------*/
/*! sf_stateManager_getState */
/*----------------------------------------------------------------------------*/
uint8_t sf_stateManager_getState(stateManager_ctx_t* pCtx)
{
  if(NULL == pCtx)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

 return pCtx->currentState;
}/* sf_stateManager_getState() */

/*----------------------------------------------------------------------------*/
/*! sf_stateManager_execState */
/*----------------------------------------------------------------------------*/
E_SF_RETURN_t sf_stateManager_execState(stateManager_ctx_t* pCtx)
{
  /* Return value. */
  E_SF_RETURN_t execStatus = E_SF_SUCCESS;
  /* Current state function. */
  stateManager_stateFct_t stateFct = NULL;

  if(NULL == pCtx)
  {
    return E_SF_ERROR_INVALID_PARAM;
  }

  stateFct = pCtx->pStates[pCtx->currentState].stateFct;

  if(NULL != stateFct)
  {
    stateFct();
  }
  else
  {
    execStatus = E_SF_ERROR_NPE;
  }

  return execStatus;
}/* sf_stateManager_execState() */

#ifdef __cplusplus
}
#endif
