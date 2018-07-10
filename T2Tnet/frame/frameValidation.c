/*
 *      Authors: Amjad,  Michel
 */
#include "frameValidation.h"
// TODO this buffer size limit the network size to 50 nodes
// if I moved we to FRAM we can enlarge it
uint16_t crc_buf[20] = {0};
uint16_t crc_buf_idx = 0;

#if DEBUG
    uint16_t checksum_debug = 0;
#endif

/* frameValidation module private variables*/
static bool frame_validation_flag = true;
static uint8_t frameRx[FRAME_LENGTH];

/* frameValidation module private functions prototypes */
uint8_t get_frame_sender_id(uint8_t* frame);
uint8_t get_frame_receiver_id(uint8_t* frame);
uint8_t get_frame_type(uint8_t* frame);
uint16_t get_frame_crc(uint8_t* frame);
uint16_t get_buffered_crc(uint8_t *frame);
bool check_linear_buffered_crc(uint8_t *frame);
void update_linear_buffered_crc(uint8_t *frame);
void update_buffered_crc(uint8_t *frame);
bool update_ttl(uint8_t* frame);
bool check_ttl(uint8_t* frame);
void *check_crc_state();
void *save_payload_state();

/**
 * @description     return the sender id from a frame data structure
 * @param           frame: a pointer to a frame data structure
 ----------------------------------------------------------------------------*/
uint8_t get_frame_sender_id(uint8_t *frame)
{
    return frame[SENDER_ID_IDX];
}

/**
 * @description     return the receiver id from a frame data structure
 * @param           frame: a pointer to a frame data structure
 ----------------------------------------------------------------------------*/
uint8_t get_frame_receiver_id(uint8_t *frame)
{
    return frame[RECEIVER_ID_IDX];
}

/**
 * @description     return the frame type from a frame data structure
 * @param           frame: a pointer to a frame data structure
 ----------------------------------------------------------------------------*/
uint8_t get_frame_type(uint8_t *frame)
{
    return frame[FRAME_TYPE_IDX];
}

/**
 * @description     return the CRC of a frame data structure
 * @param           frame: a pointer to a frame data structure
 ----------------------------------------------------------------------------*/
uint16_t get_frame_crc(uint8_t *frame)
{
    return ( (uint16_t) frame[CRC_IDX] << 8 )  | frame[CRC_IDX + 1];
}

/**
 * @description     return the CRC of crc_buf data structure
 * @param           frame: a pointer to a frame data structure
 ----------------------------------------------------------------------------*/
uint16_t get_buffered_crc(uint8_t *frame)
{
    return crc_buf[get_frame_sender_id(frame)];
}


/**
 * @description     update a cell in  crc_buf data structure
 * @param           frame: a pointer to a frame data structure
 ----------------------------------------------------------------------------*/
void update_buffered_crc(uint8_t *frame)
{
    crc_buf[get_frame_sender_id(frame)] = get_frame_crc(frame);
}

#if DEBUG
    uint16_t crc;
#endif
/**
 * @description     returns true if the crc of the frame is found in the crc
 *                  buffer, otherwise returns false
 * @param           frame: a pointer to a frame data structure
 ----------------------------------------------------------------------------*/
bool check_linear_buffered_crc(uint8_t *frame)
{
#ifndef DEBUG
    uint16_t
#endif
    crc =  get_frame_crc (frame);

    uint16_t cntr = 0;
    while (cntr <= crc_buf_idx)
    {
        // If a match is found, then the frame is old
        if (crc == crc_buf[cntr])
        {
            return true;
        }
        cntr++;
    }
    // the frame is new
    return false;
}

/**
 * @description     update a cell in  crc_buf data structure
 * @param           frame: a pointer to a frame data structure
 ----------------------------------------------------------------------------*/
void update_linear_buffered_crc(uint8_t *frame)
{
    crc_buf[crc_buf_idx] = get_frame_crc(frame);
    crc_buf_idx++;
}

/**
 * @description     reduce the TTL value in a frame
 * @param           frame: a pointer to a frame data structure
 * @return          if TTL is bigger than 0, it returns true, otherwise false
 ----------------------------------------------------------------------------*/
bool update_ttl(uint8_t *frame)
{
    if( check_ttl(frame) )
    {
        frame[TTL_IDX]--;
        return true;
    }
    return false;
}

/**
 * @description     reduce the TTL value in a frame
 * @param           frame: a pointer to a frame data structure
 * @return          if TTL is 0, it returns true, otherwise false
 ----------------------------------------------------------------------------*/
bool check_ttl(uint8_t *frame)
{
    return frame[TTL_IDX] ==0;
}

/**
 * @description     Execute FSM to process frames
 * @param           validation_state: a pointer to a function that represents
 *                  a state of the frame processing FSM. The state machine will
 *                  break when the frame_validation_flag becomes false
 ----------------------------------------------------------------------------*/
void frame_validation(func_ptr validation_state)
{
    while( frame_validation_flag == true )
    {
        validation_state = (func_ptr)(*validation_state)();
    }
    frame_validation_flag = true;
}

/**
 * @description     read a frame from the rx_buf, and returns it the
 *                  check_crc_state
 ----------------------------------------------------------------------------*/
void *wait_frame_state(){
    /* If any frames are waiting, start processing them */
    error_t error = rbuf_read(&frameRx[0], &rx_buf, FRAME_LENGTH);
    if(error == E_SUCCESS){
        return check_crc_state;
    }
    frame_validation_flag = false;
    return wait_frame_state;
}

/**
 * @description     check the crc value of the received frame. If the received
 *                  crc is valid return the save_payload_state, Else returns
 *                  wait_frame_state and set the frame_validation_flag to false
 ----------------------------------------------------------------------------*/
void *check_crc_state(){
    /* Calculate CRC */
    int16_t checksum = 0;

    /* Calculate checksum of frame (excluding received CRC) */
    for(uint8_t i = 0; i < (FRAME_LENGTH - CRC_LENGTH); i++){
        checksum += frameRx[i];
    }
#if DEBUG
    checksum_debug = checksum;
#endif

    uint16_t CRC_Result = calCRC(checksum);    // Save results (per CRC-CCITT standard)

    /* If calculated CRC is equal to received CRC, the frame is correctly received*/
    if(CRC_Result == (frameRx[FRAME_LENGTH - CRC_LENGTH] << 8 | frameRx[FRAME_LENGTH - CRC_LENGTH + 1]) )
    {
        return save_payload_state;
    } else{
        frame_validation_flag = false;
#if DEBUG
        set_p1_2();
        clear_p1_2();
#endif
        return wait_frame_state;
    }
}

/**
 * @description     if the message is for this node then save the payload.
 *                  otherwise update the TTL, update the crc and write it to
 *                  the tx_buf.
 ----------------------------------------------------------------------------*/
void *save_payload_state(){

//    // check if the message is new (single depth history)
//    if ( get_frame_crc(frameRx) != get_buffered_crc(frameRx) )
//    {
//        // register the new frame
//        update_buffered_crc(frameRx);


//*// Using Linear buffer and linear check
    if(!check_linear_buffered_crc(frameRx))
    {
        update_linear_buffered_crc(frameRx);

        uint16_t r_id =  get_frame_receiver_id( (uint8_t*) frameRx);

        if(r_id == BROADCAST_ID_BYTE)              // broadcasting
        {
            // * saving
            rbuf_write( &rx_data_buf, &frameRx[PAYLOAD_IDX] , PAYLOAD_LENGTH);
            // * re-transmission
            rbuf_write( &tx_buf, &frameRx[0] , FRAME_LENGTH);
        }
        else if (r_id == get_node_id())         // point-to-point (intended receiver)
        {
            rbuf_write( &rx_data_buf, &frameRx[PAYLOAD_IDX] , PAYLOAD_LENGTH);
            // send acknowledgment
            // set frame type
            set_frame_type(ACK_FRAME);
            // set the receiver id
            set_frame_receiver_id(get_frame_receiver_id(frameRx));
            // set the sender id
            set_frame_sender_id(get_node_id());
            // set frame id (its crc) in the first two bytes of the payload
            uint16_t crc_tmp = get_frame_crc(frameRx);
            uint8_t payload[PAYLOAD_LENGTH] = { (uint8_t) (crc_tmp >> 8) & 0xff, (uint8_t) (crc_tmp) & 0xff, 0};
#if DEBUG
            __bic_SR_register(GIE);
            create_frame(payload, &tx_buf);
            __bis_SR_register(GIE);
#else
           create_frame(payload, &tx_buf);
#endif

#if DEBUG
            set_p3_0();
            clear_p3_0();
            green_led_blink( 16000 );
#endif
        }
        else  // point-to-point (relay node)
        {
            /*
             * For now ttl will not be used,
             ---------------------------------*/

    //        if( update_ttl( (uint8_t*) frameRx) )
    //        {
    //            update_frame_crc(frameRx);
    //            // prepare for retransmission
                rbuf_write( &tx_buf, &frameRx[0] , FRAME_LENGTH);
    //        }
#if DEBUG
        red_led_blink( 16000 );  // 16e5/16e6 = 0.1 sec blink duration
        set_p1_4();
        clear_p1_4();
#endif
        }
    }
#if DEBUG
    else
    {
        set_p1_3();
        clear_p1_3();
    }
#endif


    frame_validation_flag = false;
    return wait_frame_state;
}




