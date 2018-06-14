 /*
 * packet.h
 *
 *  Created on: 30 May 2018
 *      Author: amjad
 */

#ifndef PHY_PACKET_H_
#define PHY_PACKET_H_

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


/* Length in bytes */
#define PAYLOAD_LENGTH          4
#define CRC_LENGTH              2
#define PREAMBLE_LENGTH         3 // Preamble length is only used in PER_TX_MODE or PER_RX_MODE
#define FRAME_LENGTH            11
extern uint8_t frame[];         // frame is defined in tx.c




#endif /* PHY_PACKET_H_ */
