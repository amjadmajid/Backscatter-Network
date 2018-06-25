/*
 *      Authors: Amjad,  Michel
 */

#include "frameValidation.h"
// Validation module private variables
static uint8_t frame_validation;
static uint8_t frameRx[FRAME_LENGTH];

// Private functions prototype
uint8_t get_frame_sender_id(uint8_t* frame);
uint8_t get_frame_receiver_id(uint8_t* frame);
uint8_t get_frame_type(uint8_t* frame);
bool update_ttl(uint8_t* frame);
bool check_ttl(uint8_t* frame);
void *checkCRCState();
void *savePayloadState();

uint8_t get_frame_sender_id(uint8_t *frame)
{
    return frame[SENDER_ID_IDX];
}

uint8_t get_frame_receiver_id(uint8_t *frame)
{
    return frame[RECEIVER_ID_IDX];
}

uint8_t get_frame_type(uint8_t *frame)
{
    return frame[FRAME_TYPE_IDX];
}

bool update_ttl(uint8_t *frame)
{
    if( check_ttl(frame) )
    {
        frame[TTL_IDX]--;
        return true;
    }
    return false;
}

// return true if ttl == 0
bool check_ttl(uint8_t *frame)
{
    return frame[TTL_IDX] <=0;
}

/* Execute FSM for detection and processing of frames */
void frameValidation(func_ptr frameValidationState)
{
    while( frame_validation == 0 )
    {
        frameValidationState = (func_ptr)(*frameValidationState)();
    }
    frame_validation = 0;
}

/* Do nothing, until frames are available */
void *waitFrameState(){
    /* If any frames are waiting, start processing them */
    error_t error = rbuf_read(&frameRx[0], &rx_buf, FRAME_LENGTH);
    if(error == E_SUCCESS){
        return checkCRCState;
    }
    frame_validation = 1;
    return waitFrameState;
}

#if DEBUG
    uint16_t checksum_debug =0;
#endif

/* Validate frames, and save if valid */
void *checkCRCState(){
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
        return savePayloadState;
    } else{
        frame_validation = 1;
#if DEBUG
        received_frame_incorrect++;
        red_led_blink( 16000 );  // 16e5/16e6 = 0.1 sec blink duration
#endif
        return waitFrameState;
    }
}

void *savePayloadState(){

    if(get_node_id() == get_frame_receiver_id( (uint8_t*) frameRx) )
    {
        rbuf_write( &rx_data_buf, &frameRx[PAYLOAD_IDX] , PAYLOAD_LENGTH);
    }
    else
    {
        //TODO else reduce the time to live counter and retransmit
        if( update_ttl( (uint8_t*) frameRx) )
        {
            // prepare for retransmission
            rbuf_write( &tx_buf, &frameRx[0] , FRAME_LENGTH);
        }
    }

#if DEBUG
        received_frame_correct++;
        green_led_blink( 16000 );  // 16e5/16e6 = 0.1 sec blink duration
#endif

    frame_validation = 1;
    return waitFrameState;
}






