/*
 *      Authors: Amjad,  Michel
 */

#include "frameValidation.h"

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
        received_frame_incorrect++;
        red_led_blink( 16000 );  // 16e5/16e6 = 0.1 sec blink duration
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

    if(get_node_id() == get_frame_receiver_id( (uint8_t*) frameRx) )
    {
        rbuf_write( &rx_data_buf, &frameRx[PAYLOAD_IDX] , PAYLOAD_LENGTH);
    }
    else
    {
        if( update_ttl( (uint8_t*) frameRx) )
        {
            update_frame_crc(frameRx);
            // prepare for retransmission
            rbuf_write( &tx_buf, &frameRx[0] , FRAME_LENGTH);
        }
    }

#if DEBUG
        received_frame_correct++;
        green_led_blink( 16000 );  // 16e5/16e6 = 0.1 sec blink duration
#endif

    frame_validation_flag = false;
    return wait_frame_state;
}






