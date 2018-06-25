#include <stdbool.h>
#include <msp430fr5969.h>

#ifndef RADIO_H_
#define RADIO_H_

/**
 *  Transceiver definitions
 */
#define RADIO_DIR   P4DIR
#define RADIO_OUT   P4OUT
#define ON          BIT3

#define RX_DIR      P1DIR
#define RX_OUT      P1OUT
#define RX_PIN      BIT6
#define RX_FUNC_SEL P1SEL0
/**
 *  RF switch definitions
 */
#define RFSW_DIR    P2DIR
#define RFSW_OUT    P2OUT
#define A0          BIT4
#define A1          BIT2


/**
 * Timer wait time length definitions
 */
#define BIT_LENGTH                  1600       // SMCLK (16 MHz) -> 10 kbps
#define INTERFRAME_TIME             4000       // Time between consecutive frames in SMCLK cycles
#define TIMER_JITTER_LOW            400         // SMCLK cycles
#define TIMER_JITTER_HIGH           600         // SMCLK cycles

bool channelBusy;

//#define LONG_PREAMBLE_FLAG          1
#define SLEEP_PERIOD_ACLK           10000
#define LONG_PREAMBLE_INTERVAL      2*SLEEP_PERIOD_ACLK


void transceiver_enable();
void transceiver_disable();
void radio_init();
void backscatter_state(bool phaseShift);
void receive_state();

#endif // end of the RADIO_H_
