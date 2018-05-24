/*
 * receiver.h
 *
 *  Created on: 28 Feb 2018
 *      Author: Amjad, Michel
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_

#include <msp430fr5969.h>
#include <stdbool.h>
#include <stdint.h>

//#define NEW_HW

typedef void* (*StateFunc)();


/* RX port for received bitstream */
#ifdef NEW_HW
#define RX_PIN BIT2
#else
#define RX_PIN BIT6
#endif

/**
 *  RF switch definitions
 */
#define RFSW_DIR    P2DIR
#define RFSW_OUT    P2OUT
#define A0          BIT4
#define A1          BIT2

#define RX_OUT P1OUT
#define RX_REN P1REN
#define RX_IN  P1IN
#define RX_DIR P1DIR
#define RX_IES P1IES
#define RX_IFG P1IFG
#define RX_IE  P1IE

#define RX_VECTOR PORT1_VECTOR


#define PREAMBLE_BYTE           0xBB
#define START_DELIMITER_BYTE    0xAA
#define FRAME_LENGTH            11

#define TIMER_JITTER_LOW_SMCLK      400         // SMCLK cycles
#define TIMER_JITTER_HIGH_SMCLK     600         // SMCLK cycles

#define BIT_LENGTH_SMCLK 1600 // SMCLK (16 MHz)


/* RADIO_EN to enable VDD of transceiver */
#ifndef NEW_HW
#define RADIO_EN_PIN BIT3
#define RADIO_EN_OUT P4OUT
#define RADIO_EN_DIR P4DIR
#endif

#endif /* RECEIVER_H_ */
