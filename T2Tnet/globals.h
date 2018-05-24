/*
 * customTypes.h
 *
 *  Created on: 18 May 2018
 *      Author: amjad
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_
#include <stdint.h>
#include <stdbool.h>
#include <msp430fr5969.h>

typedef void* (*StateFunc)();

/* Define MessageType for incoming and outgoing frames */
typedef enum {
    M_DATA,
    M_ACKNOWLEDGMENT
} MessageType;

typedef enum {
    E_SUCCESS,
    E_FAILURE,
    E_CHANNEL_FREE,
    E_CHANNEL_BUSY,
    E_NO_PACKETS,
    E_WAITING_PACKETS,
    E_BUFFER_EMPTY,
    E_BUFFER_FULL
} error_t;

/**
 * Timer wait time length definitions
 */
#define BIT_LENGTH           1600       // SMCLK (16 MHz) -> 10 kbps
#define INTERFRAME_TIME      4000       // Time between consecutive frames in SMCLK cycles
#define TIMER_JITTER_LOW     400         // SMCLK cycles
#define TIMER_JITTER_HIGH    600         // SMCLK cycles


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

/*---------------------------------------------------
 * Frame structure (11b):
 * - Preamble               Time-based
 * - StartFrameDelimiter    1b
 * - SenderID               1b
 * - ReceiverID             1b
 * - MessageType            1b
 * - MessageID              1b
 * - Payload                4b
 * - CRC                    2b
 ---------------------------------------------------*/
/* Define positions of frame components */
#define START_DELIMITER         0
#define SENDER_ID               1
#define RECEIVER_ID             2
#define MESSAGE_TYPE            3
#define MESSAGE_ID              4
#define PAYLOAD                 5
#define CRC                     FRAME_LENGTH - CRC_LENGTH

/* Definition of bytes */
#define NODE_ID                 4
#define PREAMBLE_BYTE           0xBB
#define START_DELIMITER_BYTE    0xAA
#define SENDER_ID_BYTE          NODE_ID
#define BROADCAST_ID_BYTE       255
#define RECEIVER_ID_BYTE        NODE_ID

/* Length in bytes */
#define PAYLOAD_LENGTH          4
#define CRC_LENGTH              2
#define PREAMBLE_LENGTH         3 // Preamble length is only used in PER_TX_MODE or PER_RX_MODE
#define FRAME_LENGTH            11
extern uint8_t frame[];         // frame is defined in tx.c

#endif /* GLOBALS_H_ */
