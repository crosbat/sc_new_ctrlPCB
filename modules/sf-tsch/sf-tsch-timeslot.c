#include "contiki.h"
#include "net/mac/tsch/tsch.h"

/**
 * \brief TSCH timing attributes and description. All timings are in usec.
 *
 * CCAOffset   -> time between the beginning of timeslot and start of CCA
 * CCA         -> duration of CCA (CCA is NOT ENABLED by default)
 * TxOffset    -> time between beginning of the timeslot and start of frame TX (end of SFD)
 * RxOffset    -> beginning of the timeslot to when the receiver shall be listening
 * RxAckDelay  -> end of frame to when the transmitter shall listen for ACK
 * TxAckDelay  -> end of frame to the start of ACK tx
 * RxWait      -> time to wait for start of frame (Guard time)
 * AckWait     -> min time to wait for start of an ACK frame
 * RxTx        -> receive-to-transmit switch time (NOT USED)
 * MaxAck      -> TX time to send a max length ACK
 * MaxTx       -> TX time to send the max length frame
 *
 * The TSCH timeslot structure is described in the IEEE 802.15.4-2015 standard,
 * in particular in the Figure 6-30.
 *
 * The default timeslot timing in the standard is a guard time of
 * 2200 us, a Tx offset of 2120 us and a Rx offset of 1120 us.
 * As a result, the listening device has a guard time not centered
 * on the expected Tx time. This is to be fixed in the next iteration
 * of the standard. This can be enabled with:
 * TxOffset: 2120
 * RxOffset: 1120
 * RxWait:   2200
 *
 * Instead, we align the Rx guard time on expected Tx time. The Rx
 * guard time is user-configurable with TSCH_CONF_RX_WAIT.
 * (TxOffset - (RxWait / 2)) instead
 */
#define TSCH_DEFAULT_TS_CCA_OFFSET         1122
#define TSCH_DEFAULT_TS_CCA                128
#define TSCH_DEFAULT_TS_TX_OFFSET          1250
#define TSCH_DEFAULT_TS_RX_OFFSET          (TSCH_DEFAULT_TS_TX_OFFSET - (TSCH_CONF_RX_WAIT / 2))
#define TSCH_DEFAULT_TS_RX_ACK_DELAY       800
#define TSCH_DEFAULT_TS_TX_ACK_DELAY       1000
#define TSCH_DEFAULT_TS_RX_WAIT            TSCH_CONF_RX_WAIT
#define TSCH_DEFAULT_TS_ACK_WAIT           400
#define TSCH_DEFAULT_TS_RX_TX              192
#define TSCH_DEFAULT_TS_MAX_ACK            1000
#define TSCH_DEFAULT_TS_MAX_TX             2500
#define TSCH_DEFAULT_TS_TIMESLOT_LENGTH    7500

/* TSCH timeslot timing (microseconds) */
const tsch_timeslot_timing_usec tsch_timeslot_timing_us_7500 = {
  TSCH_DEFAULT_TS_CCA_OFFSET,
  TSCH_DEFAULT_TS_CCA,
  TSCH_DEFAULT_TS_TX_OFFSET,
  TSCH_DEFAULT_TS_RX_OFFSET,
  TSCH_DEFAULT_TS_RX_ACK_DELAY,
  TSCH_DEFAULT_TS_TX_ACK_DELAY,
  TSCH_DEFAULT_TS_RX_WAIT,
  TSCH_DEFAULT_TS_ACK_WAIT,
  TSCH_DEFAULT_TS_RX_TX,
  TSCH_DEFAULT_TS_MAX_ACK,
  TSCH_DEFAULT_TS_MAX_TX,
  TSCH_DEFAULT_TS_TIMESLOT_LENGTH,
};
