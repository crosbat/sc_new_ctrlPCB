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
 @brief      Implementation of the contains the schedule interfaces.
*/

#include "sf-tsch-schedule.h"
#include "net/mac/tsch/tsch-schedule.h"
#include "watchdog.h"

/* Log configuration */
#include "sys/log.h"

#define LOG_MODULE "TSCHSH"
#ifndef LOG_CONF_APP
  #define LOG_LEVEL     LOG_LEVEL_NONE
#else
  #define LOG_LEVEL     LOG_CONF_APP
#endif

/* check whether the module has already been initialized */
static int initialized = 0;

/*---------------------------------------------------------------------------*/
int sf_tsch_schedule_init( void )
{
    struct tsch_slotframe *sf_common;

    /* remove all slotframes */
    tsch_schedule_remove_all_slotframes();

    /* create a new slotframe */
    sf_common = tsch_schedule_add_slotframe(APP_SLOTFRAME_HANDLE, APP_SLOTFRAME_SIZE);

    if( sf_common == NULL )
        return -1;
    else
    {
        initialized = 1;
        return 0;
    }
}


/*---------------------------------------------------------------------------*/
int sf_tsch_schedule_add_beacon_slots( void )
{
    struct tsch_link* link;
    uint16_t slot_offset;
    uint16_t channel_offset;
    struct tsch_slotframe *sf_common;

    sf_common = tsch_schedule_get_slotframe_by_handle( APP_SLOTFRAME_HANDLE );
    if( (sf_common == NULL) || (!initialized))
        return -1;


    LOG_INFO("Add b slots\n");

    /* schedule the beacon slots */
    for( int i = 0; i < APP_SLOTFRAME_SECTION_NUM; i += APP_SLOTFRAME_BEACON_DIV )
    {
      slot_offset = i * APP_SLOTFRAME_SECTION_SIZE;
      channel_offset = 0;

      watchdog_periodic();
      link = tsch_schedule_add_link(sf_common,
                LINK_OPTION_RX | LINK_OPTION_SHARED,
                LINK_TYPE_ADVERTISING_ONLY, &tsch_broadcast_address,
                slot_offset, channel_offset, true);

      if( link == NULL )
          /* an error occurred that should not. */
          return -1;
    }

    return 0;
}


/*---------------------------------------------------------------------------*/
int sf_tsch_schedule_delete_beacon_slots( void )
{
    uint16_t slot_offset;
    uint16_t channel_offset;
    struct tsch_slotframe *sf_common;

    sf_common = tsch_schedule_get_slotframe_by_handle( APP_SLOTFRAME_HANDLE );
    if( (sf_common == NULL) || (!initialized))
        return -1;

    LOG_INFO("Delete b slots\n");

    /* delete the beacon slots */
    for( int i = 0; i < APP_SLOTFRAME_SECTION_NUM; i++ )
    {
      slot_offset = i * APP_SLOTFRAME_SECTION_SIZE;
      channel_offset = 0;

      watchdog_periodic();
      if( tsch_schedule_remove_link_by_timeslot(sf_common, slot_offset, channel_offset) == 0)
          /* an error occurred that should not. */
          return -1;
    }

    return 0;
}


/*---------------------------------------------------------------------------*/
int sf_tsch_schedule_add_jreq_slots( void )
{
    struct tsch_link* link;
    uint16_t slot_offset;
    uint16_t channel_offset;
    struct tsch_slotframe *sf_common;

    sf_common = tsch_schedule_get_slotframe_by_handle( APP_SLOTFRAME_HANDLE );
    if( (sf_common == NULL) || (!initialized))
        return -1;

    LOG_INFO("Add join req slots\n");

    /* schedule the join request slots */
    for( int i = 0; i < APP_SLOTFRAME_SECTION_NUM; i++ )
    {
      slot_offset = (i * APP_SLOTFRAME_SECTION_SIZE) + APP_SLOTFRAME_SECTION_BEACON_SLOTS;
      channel_offset = 0;

      for( int j = 0; j < APP_SLOTFRAME_SECTION_JOIN_REQUEST_SLOTS; j++ )
      {
          watchdog_periodic();
          link = tsch_schedule_add_link(sf_common,
                (LINK_OPTION_TX | LINK_OPTION_SHARED) ,
                LINK_TYPE_NORMAL, &tsch_broadcast_address,
                slot_offset+j, channel_offset, true);

          if( link == NULL )
              /* an error occurred that should not. */
              return -1;
      }
    }

    return 0;
}


/*---------------------------------------------------------------------------*/
int sf_tsch_schedule_delete_jreq_slots( void )
{
    uint16_t slot_offset;
    uint16_t channel_offset;
    struct tsch_slotframe *sf_common;

    sf_common = tsch_schedule_get_slotframe_by_handle( APP_SLOTFRAME_HANDLE );
    if( (sf_common == NULL) || (!initialized))
        return -1;

    LOG_INFO("Delete join req slots\n");

    /* delete the join request slots */
    for( int i = 0; i < APP_SLOTFRAME_SECTION_NUM; i++ )
    {
        slot_offset = (i * APP_SLOTFRAME_SECTION_SIZE) + APP_SLOTFRAME_SECTION_BEACON_SLOTS;
        channel_offset = 0;

        for( int j = 0; j < APP_SLOTFRAME_SECTION_JOIN_REQUEST_SLOTS; j++ )
        {
            watchdog_periodic();
            if( tsch_schedule_remove_link_by_timeslot(sf_common, slot_offset+j, channel_offset) == 0)
                /* an error occurred that should not. */
                return -1;
        }
    }

    return 0;
}


/*---------------------------------------------------------------------------*/
int sf_tsch_schedule_add_jproc_slots( const linkaddr_t* addr )
{
    struct tsch_link* link;
    uint16_t slot_offset;
    uint16_t channel_offset;
    struct tsch_slotframe *sf_common;

    sf_common = tsch_schedule_get_slotframe_by_handle( APP_SLOTFRAME_HANDLE );
    if( (sf_common == NULL) || (!initialized))
        return -1;

    LOG_INFO("Add join pro slots ");
    LOG_INFO_LLADDR(addr);
    LOG_INFO_("\n");

    /* schedule the join request slots */
    for( int i = 0; i < APP_SLOTFRAME_SECTION_NUM; i++ )
    {
      slot_offset = (i * APP_SLOTFRAME_SECTION_SIZE) + APP_SLOTFRAME_SECTION_BEACON_SLOTS +
              APP_SLOTFRAME_SECTION_JOIN_REQUEST_SLOTS;
      channel_offset = 0;

      for( int j = 0; j < APP_SLOTFRAME_SECTION_JOIN_PROCESS_SLOTS; j++ )
      {
          watchdog_periodic();
          link = tsch_schedule_add_link(sf_common,
                LINK_OPTION_RX | LINK_OPTION_TX,
                LINK_TYPE_NORMAL, addr,
                slot_offset+j, channel_offset, true);

          if( link == NULL )
              /* an error occurred that should not. */
              return -1;
      }
    }

    return 0;
}


/*---------------------------------------------------------------------------*/
int sf_tsch_schedule_delete_jproc_slots( const linkaddr_t* addr )
{
    uint16_t slot_offset;
    uint16_t channel_offset;
    struct tsch_slotframe *sf_common;

    sf_common = tsch_schedule_get_slotframe_by_handle( APP_SLOTFRAME_HANDLE );
    if( (sf_common == NULL) || (!initialized))
        return -1;

    LOG_INFO("Delete join pro slots ");
    LOG_INFO_LLADDR(addr);
    LOG_INFO_("\n");

    /* delete the join process slots */
    for( int i = 0; i < APP_SLOTFRAME_SECTION_NUM; i++ )
    {
        slot_offset = (i * APP_SLOTFRAME_SECTION_SIZE) + APP_SLOTFRAME_SECTION_BEACON_SLOTS +
                APP_SLOTFRAME_SECTION_JOIN_REQUEST_SLOTS;
        channel_offset = 0;

        for( int j = 0; j < APP_SLOTFRAME_SECTION_JOIN_PROCESS_SLOTS; j++ )
        {
            watchdog_periodic();
            if( tsch_schedule_remove_link_by_timeslot(sf_common, slot_offset+j, channel_offset) == 0)
                /* an error occurred that should not. */
                return -1;
        }
    }

    return 0;
}


/*---------------------------------------------------------------------------*/
int sf_tsch_schedule_add_data_slots( const linkaddr_t* addr )
{
    struct tsch_link* link;
    uint16_t slot_offset;
    uint16_t channel_offset;
    struct tsch_slotframe *sf_common;
    uint8_t section = 0;
    uint16_t slot = 0;
#if LINKADDR_SIZE == 2
    uint16_t devid = addr->u16 - 1;
#else
    uint16_t devid = addr->u16[0] - 1;
#endif /* #if LINKADDR_SIZE == 2 */

    sf_common = tsch_schedule_get_slotframe_by_handle( APP_SLOTFRAME_HANDLE );
    if( (sf_common == NULL) || (!initialized))
        return -1;

    if( (int16_t)devid < 0 )
      return -1;


    LOG_INFO("Add data slots ");
    LOG_INFO_LLADDR(addr);
    LOG_INFO_("\n");

#if SCHEDULE_A

    /* schedule the TX slot */
    section = devid / APP_SLOTFRAME_SECTION_DEVICE_TX_SLOTS;
    slot = ((devid - 0) * 2) % APP_SLOTFRAME_SECTION_DEVICE_TX_SLOTS;
    slot_offset = (section * APP_SLOTFRAME_SECTION_SIZE) +
            APP_SLOTFRAME_SECTION_BEACON_SLOTS + APP_SLOTFRAME_SECTION_JOIN_SLOTS +
            slot;
    channel_offset = 0;
    link = tsch_schedule_add_link(sf_common,
          LINK_OPTION_TX,
          LINK_TYPE_NORMAL, &tsch_broadcast_address,
          slot_offset, channel_offset, true);

    if( link == NULL )
        /* an error occurred that should not. */
        return -1;

    /* Add slot only for retransmissions */
    slot_offset = slot_offset + 1;
    link = tsch_schedule_add_link(sf_common,
          LINK_OPTION_TX,
          LINK_TYPE_NORMAL_RTX, &tsch_broadcast_address,
          slot_offset, channel_offset, true);

    if( link == NULL )
        /* an error occurred that should not. */
        return -1;



    /* schedule the RX slot */
    section = devid / APP_SLOTFRAME_SECTION_DEVICE_RX_SLOTS;
    slot = ((devid - 0) * 2) % APP_SLOTFRAME_SECTION_DEVICE_RX_SLOTS;
    slot_offset = (section * APP_SLOTFRAME_SECTION_SIZE) +
            APP_SLOTFRAME_SECTION_BEACON_SLOTS + APP_SLOTFRAME_SECTION_JOIN_SLOTS + (APP_SLOTFRAME_SECTION_DEVICE_TX_SLOTS * 2) +
            slot;
    channel_offset = 0;
    link = tsch_schedule_add_link(sf_common,
          LINK_OPTION_RX,
          LINK_TYPE_NORMAL, &tsch_broadcast_address,
          slot_offset, channel_offset, true);

    if( link == NULL )
        /* an error occurred that should not. */
        return -1;
#endif /* #endif #if SCHEDULE_A */

    return 0;
}


/*---------------------------------------------------------------------------*/
int sf_tsch_schedule_delete_data_slots( const linkaddr_t* addr )
{
    uint16_t slot_offset;
    uint16_t channel_offset;
    struct tsch_slotframe *sf_common;
    uint8_t section = 0;
    uint16_t slot = 0;
#if LINKADDR_SIZE == 2
    uint16_t devid = addr->u16 - 1;
#else
    uint16_t devid = addr->u16[0] - 1;
#endif /* #if LINKADDR_SIZE == 2 */

    sf_common = tsch_schedule_get_slotframe_by_handle( APP_SLOTFRAME_HANDLE );
    if( (sf_common == NULL) || (!initialized))
        return -1;

    if( (int16_t)devid < 0 )
      return -1;


    LOG_INFO("Delete data slots ");
    LOG_INFO_LLADDR(addr);
    LOG_INFO_("\n");

#if SCHEDULE_A
    /* remove the TX slot */
    section = devid / APP_SLOTFRAME_SECTION_DEVICE_TX_SLOTS;
    slot = ((devid - 0) * 2) % APP_SLOTFRAME_SECTION_DEVICE_TX_SLOTS;
    slot_offset = (section * APP_SLOTFRAME_SECTION_SIZE) +
            APP_SLOTFRAME_SECTION_BEACON_SLOTS + APP_SLOTFRAME_SECTION_JOIN_SLOTS +
            slot;
    channel_offset = 0;
    if( tsch_schedule_remove_link_by_timeslot(sf_common, slot_offset, channel_offset) == 0)
       /* an error occurred that should not. */
       return -1;

    /* Remove slot only for retransmissions */
    slot_offset = slot_offset + 1;
    if( tsch_schedule_remove_link_by_timeslot(sf_common, slot_offset, channel_offset) == 0)
       /* an error occurred that should not. */
       return -1;

        /* remove the RX slot */
    section = devid / APP_SLOTFRAME_SECTION_DEVICE_RX_SLOTS;
    slot = ((devid - 0) * 2) % APP_SLOTFRAME_SECTION_DEVICE_RX_SLOTS;
    slot_offset = (section * APP_SLOTFRAME_SECTION_SIZE) +
            APP_SLOTFRAME_SECTION_BEACON_SLOTS + APP_SLOTFRAME_SECTION_JOIN_SLOTS + (APP_SLOTFRAME_SECTION_DEVICE_TX_SLOTS * 2) +
            slot;
    channel_offset = 0;
    if( tsch_schedule_remove_link_by_timeslot(sf_common, slot_offset, channel_offset) == 0)
       /* an error occurred that should not. */
       return -1;
#endif /* #if SCHEDULE_A */

    return 0;

}



#ifdef __cplusplus
}
#endif
