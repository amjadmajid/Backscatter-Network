/*
 * phy.h
 *
 *  Created on: 30 May 2018
 *      Author: Amjad
 */

#include "radio.h"
#include "timers.h"
#include "packet.h"
#include "rx.h"
#include "tx.h"

#ifndef PHY_PHY_H_
#define PHY_PHY_H_


/* Definition of bytes */
#define NODE_ID                 4
#define PREAMBLE_BYTE           0xBB
#define START_DELIMITER_BYTE    0xAA
#define SENDER_ID_BYTE          NODE_ID
#define BROADCAST_ID_BYTE       255
#define RECEIVER_ID_BYTE        NODE_ID


/**
 * Timer wait time length definitions
 */
#define BIT_LENGTH           1600       // SMCLK (16 MHz) -> 10 kbps
#define INTERFRAME_TIME      4000       // Time between consecutive frames in SMCLK cycles
#define TIMER_JITTER_LOW     400         // SMCLK cycles
#define TIMER_JITTER_HIGH    600         // SMCLK cycles

bool channelBusy;

#define LONG_PREAMBLE 1
#define SLEEP_PERIOD_ACLK                10000
#define LONG_PREAMBLE_INTERVAL           2*SLEEP_PERIOD_ACLK

/* Buffers to store incoming and outgoing frames */
rbuf_t tx_buf;
rbuf_t rx_buf;
rbuf_t rx_data_buf;


#endif /* PHY_PHY_H_ */
