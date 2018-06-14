#include "sys.h"
#include "phy.h"

bool phaseShift = false;
uint8_t frame[FRAME_LENGTH] = {0};

/**
 * @description this function creates a frame. and saves it into the frame
                global variable 
 * @param       receiverId represents the id of the receiver node
 * @param       messageType specify the type of the message
 * @param       payloadPtr a pointer to the payload
 ----------------------------------------------------------------------------*/
void create_frame(uint8_t receiverId, MessageType messageType, uint8_t *payloadPtr)
{
    uint8_t i;
    uint16_t checksum = 0, resultCRC = 0;


    /* Prepare Frame */
    frame[START_DELIMITER]  = START_DELIMITER_BYTE;
    frame[SENDER_ID]        = SENDER_ID_BYTE;
    frame[RECEIVER_ID]      = receiverId;
    frame[MESSAGE_TYPE]     = messageType;
    frame[MESSAGE_ID]       = 0;

    /* Copy payload into the frame */
    for(i = 0; i < PAYLOAD_LENGTH; i++){
        frame[PAYLOAD + i] = (*(payloadPtr + i));
    }

    /* Add CRC to frame */
    for(i = 0; i < (FRAME_LENGTH - CRC_LENGTH); i++){
        checksum += frame[i];
    }
    resultCRC = calCRC(checksum);
    //  frame [byte 0| byte 1 ....| high CRC byte: low CRC byte  ]
    frame[FRAME_LENGTH - CRC_LENGTH] = resultCRC >> 8;
    frame[FRAME_LENGTH - CRC_LENGTH + 1] = resultCRC;

    rbuf_write(&tx_buf, &frame[0], FRAME_LENGTH);
}


/**
 * @description     it toggles the RF switch to backscatter
 * @param           phaseShift indicate with a phase shift is required or not
 ----------------------------------------------------------------------------*/
void backscatter(bool phaseShift)
{
//    P2DIR |= BIT2;
//    P2OUT ^= BIT2;

    if(phaseShift)
    {
        RFSW_OUT ^= A0;
        RFSW_OUT |= A1;
    }else{
        RFSW_OUT |= A0;
        RFSW_OUT ^= A1;
    }
}

/**
 * @description this function backscatters a data-1 symbol
 ----------------------------------------------------------------------------*/
void __backscatter_1()
{
    backscatter(phaseShift);
    fast_timer_delay( (uint16_t) BIT_LENGTH);
}

/**
 * @description this function backscatters a data-0 symbol
 ----------------------------------------------------------------------------*/
void __backscatter_0()
{
    backscatter(phaseShift);
    fast_timer_delay( (uint16_t) BIT_LENGTH >> 1);
    backscatter(phaseShift);
    fast_timer_delay( (uint16_t) BIT_LENGTH >> 1);
}

/**
 * @description this function backscatters a given byte
 * @param       byte a byte of data to be backscattered
 ----------------------------------------------------------------------------*/
void __backscatter_byte(uint8_t byte)
{
	int8_t i;
    for(i = 7; i >= 0; i--){
        if(byte & (0x01<<i)){
            __backscatter_1();
        }
        else {
            __backscatter_0();
        }
    }
}

#ifdef DEBUG
uint8_t frameTx[FRAME_LENGTH];
#endif

/**
 * @description this function backscatters a frame
 ----------------------------------------------------------------------------*/
void backscatter_frame()
{
#ifndef DEBUG
    uint8_t frameTx[FRAME_LENGTH];
#endif
    rbuf_read(frameTx, &tx_buf, FRAME_LENGTH);

    phaseShift=false;
    // transmission of non-phase shifted frame 
    backscatter_state(phaseShift);
    uint8_t i ;
    // Transmit preamble
    for(i = 0; i < PREAMBLE_LENGTH; i++){
        __backscatter_byte(PREAMBLE_BYTE);
        __no_operation();
     }
     // Transmit payload
     for(i = 0; i < FRAME_LENGTH; i++){
        __backscatter_byte( frameTx[i] );
     }

     fast_timer_delay( (uint16_t) INTERFRAME_TIME);

     phaseShift = true;
    // transmission of phase shifted frame 
    for(i = 0; i < PREAMBLE_LENGTH; i++){
        __backscatter_byte(PREAMBLE_BYTE);
     }
    for(i = 0; i < FRAME_LENGTH; i++){
        __backscatter_byte( frameTx[i] );
     }

    backscatter_state(phaseShift);
    phaseShift = false;
    fast_timer_delay( (uint16_t) INTERFRAME_TIME);
}


