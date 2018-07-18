#include <frame.h>

uint8_t frame_receiver_id = 0;
uint8_t frame_sender_id = 0;
uint8_t frame_ttl = 0;
uint8_t frame_type = DATA_FRAME;
uint8_t frameBx[FRAME_LENGTH] = {0};
/**
 * @description     set the frameBx type
 * @param           frame_type_val: frameBx type value
 ----------------------------------------------------------------------------*/
void set_frame_type(uint8_t frame_type_val)
{
	frame_type = frame_type_val;
}

/**
 * @description     set the frameBx receiver id
 * @param           id: frameBx receiver id
 ----------------------------------------------------------------------------*/
void set_frame_receiver_id(uint8_t id)
{
	frame_receiver_id = id;
}

/**
 * @description     set the frameBx sender id
 * @param           id: frameBx sender id
 ----------------------------------------------------------------------------*/
void set_frame_sender_id(uint8_t id)
{
	frame_sender_id = id;
}

/**
 * @description     set the frameBx TTL value
 * @param           ttl_val: the TTL value
 ----------------------------------------------------------------------------*/
void set_ttl(uint8_t ttl_val)
{
	frame_ttl = ttl_val;
}

/**
 * @description     update the crc value (called after changing anything in
 *                  a received frameBx)
 * @param           frameBx: a pointer to frameBx data structure
 ----------------------------------------------------------------------------*/
uint16_t checksum;
uint16_t resultCRC;
void update_frame_crc(uint8_t * frameBxIn)
{

    uint8_t i = 0;
    checksum=0;

    /* Add CRC to frameBx */
    for(i = 0; i < (FRAME_LENGTH - CRC_LENGTH); i++){
        checksum += (*(frameBxIn + i));
    }
    resultCRC = calCRC(checksum);
    //  frameBx [byte 0| byte 1 ....| high CRC byte: low CRC byte  ]
    frameBxIn[FRAME_LENGTH - CRC_LENGTH] = ((resultCRC >> 8) & 0xff);
    frameBxIn[FRAME_LENGTH - CRC_LENGTH + 1] = (resultCRC & 0xff);
}

/**
 * @description this function creates a frameBx. and saves it into the frameBx
                global variable 
 * @param       payload_ptr: a pointer to the payload
 * @param       buf: a pointer to a buffer to write the created frameBx to
 ----------------------------------------------------------------------------*/
void create_frame(uint8_t *payload_ptr, rbuf_t * buf)
{
    uint8_t i;
    uint16_t checksum = 0, resultCRC = 0;


    /* Prepare frameBx */
    frameBx[START_DELIMITER_IDX]  = START_DELIMITER_BYTE;
    frameBx[SENDER_ID_IDX]        = frame_sender_id;
    frameBx[RECEIVER_ID_IDX]      = frame_receiver_id;
    frameBx[FRAME_TYPE_IDX]       = frame_type;
    frameBx[TTL_IDX]              = frame_ttl;

    /* Copy payload into the frameBx */
    for(i = 0; i < PAYLOAD_LENGTH; i++){
        frameBx[PAYLOAD_IDX + i] = (*(payload_ptr + i));
    }

    update_frame_crc(frameBx);
    rbuf_write(buf, &frameBx[0], FRAME_LENGTH);
}


//void send_frameBx(uint8_t sender_id, uint8_t receiver_id,uint8_t message_type, uint8_t ttl, uint8_t *payload_ptr, rbuf_t * buf)
//{
//    set_frameBx_sender_id(sender_id);
//    set_frameBx_receiver_id(receiver_id);
//    set_frame_type(message_type);
//    set_ttl(ttl);
//
//    create_frameBx(payload_ptr, buf);
//    rbuf_write( &frameBx_id_buf ,get_frameBx_crc(frameBx), CRC_LENGTH )
//
//}
