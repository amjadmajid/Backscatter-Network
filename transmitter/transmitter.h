/*
 * transmitter.h
 *
 *  Created on: 28 Feb 2018
 *      Author:  Amjad, Michel
 */

#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_

#include <msp430fr5969.h>
#include <stdbool.h>
#include <stdint.h>

//#define NEW_HW

///* Port definitions for A1 & A0 of input switch */
#ifdef NEW_HW
#define A1_PIN BIT2
#define A1_OUT P3OUT
#define A1_DIR P3DIR
#define A0_PIN BIT1
#define A0_OUT P3OUT
#define A0_DIR P3DIR
#else
#define A1_PIN BIT2
#define A1_OUT P2OUT
#define A1_DIR P2DIR
#define A0_PIN BIT4
#define A0_OUT P2OUT
#define A0_DIR P2DIR

#define TX_PIN BIT5
#define TX_OUT P1OUT
#define TX_DIR P1DIR

#endif

#define BIT_LENGTH_SMCLK 1600 // SMCLK (16 MHz) -> 10 kbps

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
#define FRAME_LENGTH            11
#define PAYLOAD_LENGTH          4
#define CRC_LENGTH              2
#define PREAMBLE_LENGTH         3 // Preamble length is only used in PER_TX_MODE or PER_RX_MODE

const unsigned int CRC_Init = 0xFFFF;


#define INTERFRAME_TIME_SMCLK       4000    // Time between consecutive frames in SMCLK cycles

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


uint8_t frame[FRAME_LENGTH] = {0};

#endif /* TRANSMITTER_H_ */
