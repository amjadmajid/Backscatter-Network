/*
 * rx.c
 *
 *  Created on: Sep 27, 2017
 *      Author: michel
 */

#include "rx.h"

// Private functions prototype
void *detectFrameState();
void *catchFrameState();

/* Variables used for bit decoding in ISR */
func_ptr frameDetect_fsm;
volatile uint8_t bitCounter, byteCounter, decodedBits;
volatile bool potentialZero;
volatile uint16_t timeCapture;
volatile uint8_t frame[FRAME_LENGTH] = {0};

void rx_line_init(){
	/* Configure RX port to use as input for TB0 Timer CCR3 */
    RX_DIR &= ~RX_PIN;                          // configure RX port as an input port
    RX_FUNC_SEL |= RX_PIN;                      // P1.6 options select (primary module function= TB0 CCR3)

	/* Initialize state pointer for frame detection and recording */
    frameDetect_fsm = (func_ptr) detectFrameState;

    /* Initialize variables for RX decoding */
	decodedBits = 0;
	bitCounter = 0;
	byteCounter = 0;
	potentialZero = false;
}

/* Enable interrupts of receiver */
void start_capture(){
    TB0CCTL3 =  CM_3    // Capture both rising and falling edge,
              | CCIS_1  // Use CCI3B = P1.6,
              | SCS     // Synchronous capture,
              | CAP     // Enable capture mode,
              | CCIE;   // Enable capture interrupt
    TBCCR0 = 0xFFFF;
}

/* Disable interrupts of receiver */
void stop_capture(){
    TB0CCTL3 &= ~CCIE; // Stop capture interrupts
}

void *detectFrameState(){
	if(decodedBits == PREAMBLE_BYTE){
		channelBusy = true;
	}

	if(decodedBits == START_DELIMITER_BYTE){
		channelBusy = true;
		bitCounter = 0;
		frame[byteCounter] = decodedBits;
		byteCounter++;
		decodedBits = 0;
		return catchFrameState;
	}
	return detectFrameState;
}

void *catchFrameState(){
	if(bitCounter == 8){
		bitCounter = 0;
		frame[byteCounter] = decodedBits;
		byteCounter++;
		decodedBits = 0;
	}
	if(byteCounter == FRAME_LENGTH){
        //disable reception
        stop_capture();
		byteCounter = 0;
		rbuf_write(&rx_buf, &frame, FRAME_LENGTH);
		start_capture();
		return detectFrameState;
	}
	return catchFrameState;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_B1_VECTOR
__interrupt void Timer0_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B1_VECTOR))) Timer0_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TB0IV, TB0IV_TBIFG)) {
        case TB0IV_NONE:   break;               // No interrupt
        case TB0IV_TBCCR1: break;               // CCR1 not used
        case TB0IV_TBCCR2: break;               // CCR2 not used
        case TB0IV_TBCCR3: /* Decode bits algorithm */
        	/* Save captured timer value and reset timer */
            //TODO use majority voting to have a more accurate timer readings

            timeCapture = TB0CCR3;
			TB0CTL |= TBCLR;

			/* Check if time corresponds to a valid zero (= BIT_LENGTH/2) */
			if( timeCapture >= (BIT_LENGTH >> 1) - TIMER_JITTER_LOW && timeCapture < (BIT_LENGTH - (BIT_LENGTH >> 2)) ){
				if(potentialZero == 0){
					potentialZero = true;
				} else {
					potentialZero = false;
					decodedBits = (decodedBits <<1);
					bitCounter++;
				}
			/* Check if time corresponds to a valid one (= BIT_LENGTH) */
			} else if( timeCapture >= (BIT_LENGTH - (BIT_LENGTH >> 2)) && timeCapture <= BIT_LENGTH + TIMER_JITTER_HIGH ){
				potentialZero = false;
				decodedBits = (decodedBits << 1) +1;
				bitCounter++;
			} else {
				potentialZero = false;
				decodedBits = 0;
				bitCounter = 0;
			}

			/* Detection and recording of frames */
			frameDetect_fsm = (func_ptr)(*frameDetect_fsm)();
            break;
        case TB0IV_TBCCR4: break;               // CCR4 not used
        case TB0IV_TBCCR5: break;               // CCR5 not used
        case TB0IV_TBCCR6: break;               // CCR6 not used
        case TB0IV_TBIFG:                       // overflow
          break;
        default: break;
    }
}
