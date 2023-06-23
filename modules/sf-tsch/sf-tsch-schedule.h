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
 @brief      This header contains the schedule interfaces.
*/

#ifndef TSCH_SCHEDULE_H_
#define TSCH_SCHEDULE_H_

#include <stdint.h>
#include <stdlib.h>
#include "linkaddr.h"
#include "tsch.h"

/**
 * @brief   Initialize the schedule.
 *
 *          Initialized the default slotrame.
 *
 * @return  0 on success.
 */
int sf_tsch_schedule_init( void );


/**
 * @brief	Add the Beacon Slots.
 *
 *			Add the beacon slots to the schedule. This is typically done
 *			at initialization and remains till the end.
 *
 * @return	0 on success.
 */
int sf_tsch_schedule_add_beacon_slots( void );


/**
 * @brief	Remove the Beacon Slots.
 *
 *			Remove the beacon slots to the schedule. This is never
 *			done typically.
 *
 * @return	0 on success.
 */
int sf_tsch_schedule_delete_beacon_slots( void );


/**
 * @brief	Add the Join-Request Slots.
 *
 *			Add the Join Request Slots to the schedule. This function differs
 *			from gateway to device.
 *			The gateway keeps the slots open as long as it wants to accept
 *			new devices. Afterwards it removes the slots.
 *			The device opens the slots as long as it is not joined with a
 *			gateway. After it joined a gateway it should close the slots.
 *
 * @return	0 on success.
 */
int sf_tsch_schedule_add_jreq_slots( void );


/**
 * @brief	Remove the Join-Request Slots.
 *
 *			Remove the Join Request Slots from the schedule. This function differs
 *			from gateway to device.
 *			At the gateway keeps the slots are removed in case no new devices
 *			shall be accepted anymore.
 *			The device removes the slots once it is joined to a gateway.
 *
 * @return	0 on success.
 */
int sf_tsch_schedule_delete_jreq_slots( void );


/**
 * @brief	Add Join-Process Slots.
 *
 *			Add Join-Process Slots to the schedule. The process slots are
 *			device specific. While the gateway opens the slots for a specific
 *			device as a response to a join request, a device opens the slots
 *			to the gateway it wants to join.
 *
 * @param	addr 	Address to open the join process for.
 *
 * @return	0 on success.
 */
int sf_tsch_schedule_add_jproc_slots( const linkaddr_t* addr );


/**
 * @brief	Remove Join-Process Slots.
 *
 *			Remove Join-Process Slots from the schedule. Gateway and devices
 *          remove the slots after a successful or failed join.
 *
 * @return	0 on success.
 */
int sf_tsch_schedule_delete_jproc_slots( const linkaddr_t* addr );


/**
 * @brief	Add Data-Slots.
 *
 *			A Data Slots are used for regular communication between the
 *			the gateway and devices. Gatways keep the data slots open
 *			all the time for joined devices and add them after a successful join.
 *			Devices also add the data slots after a succesfull join.
 *
 * @param	addr 	Address to open the join process for.
 *
 * @return	0 on success.
 */
int sf_tsch_schedule_add_data_slots( const linkaddr_t* addr );


/**
 * @brief	Remove Data-Slots.
 *
 *			Remove Data Slots from the schedule. SInce data slots
 *			are devic especific the according address is required. Gateway
 *			and devices remove data slots whenever the connection was
 *			closed or broke.
 *
 * @return	0 on success.
 */
int sf_tsch_schedule_delete_data_slots( const linkaddr_t* addr );


#endif /* TSCH_SCHEDULE_H_ */

#ifdef __cplusplus
}
#endif
