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

void transceiver_enable();
void transceiver_disable();
void rf_sw_init();
void radio_init();
void backscatter_state(bool phaseShift);
void backscatter(bool phaseShift);
void receive_state();


#endif // end of the RADIO_H_
