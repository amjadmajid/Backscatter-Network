/*
 *      Authors: Michel, Amjad
 */

#include "sys.h"
#include "phy.h"
#include "mac.h"


/* State function pointer for frame processing */
func_ptr stateFuncFrames;
/* Variables for frame processing*/
volatile uint8_t frameRx[FRAME_LENGTH];

/* Execute FSM for detection and processing of frames */
void frameValidation(func_ptr frameValidationState)
{
    while( frame_validation == 0 )
    {
        frameValidationState = (func_ptr)(*frameValidationState)();
    }
}

/* Do nothing, until frames are available */
void *waitFrameState(){
    /* If any frames are waiting, start processing them */
    error_t error = rbuf_read(&frameRx, &rx_buf, FRAME_LENGTH);
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
        P4OUT |= BIT6;    // set the LED pin
        __delay_cycles(160000);
        P4OUT &=~BIT6;    // CLear the LED pin
#endif
        return waitFrameState;
    }
}

void *savePayloadState(){

//#if defined PER_RX_MODE
//    // PER measurements to keep track of number successful received frames:
//    measurementPER[ (uint8_t) frameRx[PAYLOAD] ][ (uint8_t) frameRx[PAYLOAD + 1] ]++;
//#endif
//    error_t error;

//  // DEBUG CODE: NODE 4 (RX) is not checking for messageIds to capture all packets (=more data during testing)
//  if(NODE_ID == 4){
//      RingBuffer_Write( &dataBufferRx , (void*) &frameRx[PAYLOAD] );
//      return waitFrameState;
//  }
//  if(NODE_ID == 1){
//      RingBuffer_Write( &dataBufferRx , (void*) &frameRx[PAYLOAD] );
//      return waitFrameState;
//  }
//  // END DEBUG CODE

    /* Check if message is new or old with messageId */
//    error = checkMessageId( frameRx[SENDER_ID], frameRx[MESSAGE_ID] );
//    if(error == E_SUCCESS){
//        updateMessageId( frameRx[SENDER_ID], frameRx[MESSAGE_ID] );
        /* If information is intended for this node, copy payload of received frame to buffer
         * If not, rebroadcast frame  */
//        if (frameRx[RECEIVER_ID] == BROADCAST_ID_BYTE || frameRx[RECEIVER_ID] == RECEIVER_ID_BYTE ){
            // DEBUG CODE: commented out to prevent double writing to dataBufferRx (see above with NODE_ID == 4)
            rbuf_write( &rx_data_buf, &frameRx[PAYLOAD] , PAYLOAD_LENGTH);
#if DEBUG
            received_frame_correct++;
            P1OUT |= BIT0;    // set the LED pin
            __delay_cycles(160000);
            P1OUT &=~BIT0;    // CLear the LED pin
#endif

//        } else{
//            // DEBUG CODE: save payload of relay nodes to RX buffer
//            rbuf_write( &rx_buf, FRAME_LENGTH, (void*) &frameRx[PAYLOAD] );
//            // END DEBUG CODE
//            //TODO forward the received message
////            rebroadcastFrame( frameRx[SENDER_ID], frameRx[RECEIVER_ID], (MessageType) frameRx[MESSAGE_TYPE], (uint8_t) frameRx[MESSAGE_ID], &frameRx[PAYLOAD]);
//        }
//    }
    frame_validation = 1;
    return waitFrameState;
}






