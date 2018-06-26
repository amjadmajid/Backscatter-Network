#include <frame.h>

uint8_t frame_receiver_id = 0;
uint8_t frame_sender_id = 0;
uint8_t frame_ttl = 0;
uint8_t frame_type = 0;

/**
 * @description     set the frame type
 * @param           frame_type_val: frame type value
 ----------------------------------------------------------------------------*/
void set_frame_type(uint8_t frame_type_val)
{
	frame_type = frame_type_val;
}

/**
 * @description     set the frame receiver id
 * @param           id: frame receiver id
 ----------------------------------------------------------------------------*/
void set_frame_receiver_id(uint8_t id)
{
	frame_receiver_id = id;
}

/**
 * @description     set the frame sender id
 * @param           id: frame sender id
 ----------------------------------------------------------------------------*/
void set_frame_sender_id(uint8_t id)
{
	frame_sender_id = id;
}

/**
 * @description     set the frame TTL value
 * @param           ttl_val: the TTL value
 ----------------------------------------------------------------------------*/
void set_ttl(uint8_t ttl_val)
{
	frame_ttl = ttl_val;
}

/**
 * @description     update the crc value (called after changing anything in
 *                  a received frame)
 * @param           frame: a pointer to frame data structure
 ----------------------------------------------------------------------------*/
void update_frame_crc(uint8_t * frame)
{
    uint16_t checksum;
    uint8_t i = 0;

    /* Add CRC to frame */
    for(i = 0; i < (FRAME_LENGTH - CRC_LENGTH); i++){
        checksum += frame[i];
    }
    uint16_t resultCRC = calCRC(checksum);
    //  frame [byte 0| byte 1 ....| high CRC byte: low CRC byte  ]
    frame[FRAME_LENGTH - CRC_LENGTH] = resultCRC >> 8;
    frame[FRAME_LENGTH - CRC_LENGTH + 1] = resultCRC;
}

/**
 * @description this function creates a frame. and saves it into the frame
                global variable 
 * @param       payload_ptr: a pointer to the payload
 * @param       buf: a pointer to a buffer to write the created frame to
 ----------------------------------------------------------------------------*/
void create_frame(uint8_t *payload_ptr, rbuf_t * buf)
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

    update_frame_crc(frame);

    rbuf_write(buf, &frame[0], FRAME_LENGTH);
}
