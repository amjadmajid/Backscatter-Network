#include <frame.h>

uint8_t frame_receiver_id = 0;
uint8_t frame_sender_id = 0;
uint8_t frame_ttl = 0;
uint8_t frame_type = 0;

void set_frame_type(uint8_t frame_type_val)
{
	frame_type = frame_type_val;
}

void set_frame_receiver_id(uint8_t id)
{
	frame_receiver_id = id;
}

void set_frame_sender_id(uint8_t id)
{
	frame_sender_id = id;
}

void set_ttl(uint8_t ttl_val)
{
	frame_ttl = ttl_val;
}


/**
 * @description this function creates a frame. and saves it into the frame
                global variable 
 * @param       receiverId represents the id of the receiver node
 * @param       messageType specify the type of the message
 * @param       payloadPtr a pointer to the payload
 ----------------------------------------------------------------------------*/
void create_frame(uint8_t *payloadPtr, rbuf_t * buf)
{
    uint8_t frame[FRAME_LENGTH] = {0};
    uint8_t i;
    uint16_t checksum = 0, resultCRC = 0;


    /* Prepare Frame */
    frame[START_DELIMITER_IDX]  = START_DELIMITER_BYTE;
    frame[SENDER_ID_IDX]        = frame_sender_id;
    frame[RECEIVER_ID_IDX]      = frame_receiver_id;
    frame[FRAME_TYPE_IDX]       = frame_type;
    frame[TTL_IDX]              = frame_ttl;

    /* Copy payload into the frame */
    for(i = 0; i < PAYLOAD_LENGTH; i++){
        frame[PAYLOAD_IDX + i] = (*(payloadPtr + i));
    }

    /* Add CRC to frame */
    for(i = 0; i < (FRAME_LENGTH - CRC_LENGTH); i++){
        checksum += frame[i];
    }
    resultCRC = calCRC(checksum);
    //  frame [byte 0| byte 1 ....| high CRC byte: low CRC byte  ]
    frame[FRAME_LENGTH - CRC_LENGTH] = resultCRC >> 8;
    frame[FRAME_LENGTH - CRC_LENGTH + 1] = resultCRC;

    rbuf_write(buf, &frame[0], FRAME_LENGTH);
}
