
#include "transmitter.h"
#define DEBUG 1

uint16_t frame_cntr = 0;
/*
 * Setup timerA2 for continuous mode operation (SMCLK = DCO speed)
 */
void timerA2_init() {
    TA2CTL = (TASSEL__SMCLK |    // Use SMCLK as source for timer
              MC__CONTINUOUS |  // Use continuous running mode
              TACLR |           // Clear timer flag
              TAIE);            // Enable Timer_A interrupts
}

void SMCLK_delay(uint16_t ticksSMCLK) {
    TA2CCR1 = TA2R + ticksSMCLK;    // Set compare value
    TA2CTL &= ~TAIFG;               // Clear interrupt flag
    TA2CCTL1 = CCIE;                // Enable Register comparator interrupt
    // /* Put MCU to low-power operation mode 3 and wait for timer interrupt. */
    __bis_SR_register(LPM0_bits | GIE);
}

#pragma vector=TIMER2_A1_VECTOR
__interrupt void INT_Timer2_A1(void) {
    switch(__even_in_range(TA2IV, TA2IV_TAIFG)) {
        case TA2IV_NONE:   break;               // No interrupt
        case TA2IV_TACCR1:                      // CCR1 routine (SMCLK delay)
            TA2CTL &= ~TAIFG;
            TA2CCTL1 = 0x00;                    // Reset comparator settings
            __bic_SR_register_on_exit(LPM4_bits);  // #define LPM4_bits      (SCG1+SCG0+OSCOFF+CPUOFF)
            break;
        case TA2IV_3:      break;               // reserved
        case TA2IV_4:      break;               // reserved
        case TA2IV_5:      break;               // reserved
        case TA2IV_6:      break;               // reserved
        case TA2IV_TAIFG:                       // overflow
            TA2CTL &= ~TAIFG;
            break;
        default: break;
    }
}

/*---------------------------------------------------
 * 1-0 Modulation utilities
 ---------------------------------------------------*/

void tx_on() {
    /* TX on -> means TX1
     * TX1 = (A1,A0) = (0,1) for non-shifted */
        A1_OUT &= ~A1_PIN;
        A0_OUT &= ~A0_PIN;
}
void tx_toggle() {
    /* TX toggle -> means switch between 1) RX (Matched) Mode and 2) TX1 (no phase-shift)
     * RX  = (A1,A0) = (1,1) for matched
     * TX1 = (A1,A0) = (0,1) for non-shifted */
        A1_OUT ^= A1_PIN;
//        A0_OUT |= A0_PIN;
}

void tx_init(){
    /* Configure control ports as output */
#ifdef NEW_HW

    A1_DIR &= ~A1_PIN;
    A0_DIR &= ~A0_PIN;
    A1_DIR |= A1_PIN;
    A0_DIR |= A0_PIN;
#else
    A1_DIR |= A1_PIN;
    A0_DIR |= A0_PIN;
    //    /* Configure old backscatter switch to be off */
    TX_DIR |= TX_PIN;
    TX_OUT &= ~TX_PIN;
#endif

}

/*---------------------------------------------------
 *  Transmission of a bit using FM0 / Miller0
 ---------------------------------------------------*/
void send_1() {
    tx_toggle();
    SMCLK_delay( (uint16_t) BIT_LENGTH_SMCLK);
}

void send_0() {
    tx_toggle();
    SMCLK_delay( (uint16_t) BIT_LENGTH_SMCLK>>1);
    tx_toggle();
    SMCLK_delay( (uint16_t) BIT_LENGTH_SMCLK>>1);
}

/*---------------------------------------------------
 * Transmission of a Byte using FM0
 ---------------------------------------------------*/
void send_byte(uint8_t byte) {
    int8_t i;
    for(i = 7; i >= 0; i--){
        if(byte & (0x01<<i)){
            send_1();
        }
        else {
            send_0();
        }
    }
}

uint16_t calculateCRC(uint16_t checksum){
    CRCINIRES = CRC_Init;                     // Init CRC16 HW module
    CRCDIRB = checksum;                 // Input data in CRC
    __no_operation();
    uint16_t resultCRC = CRCINIRES;
    return resultCRC;
}

/*
 * Send single frame
 */
void sendFrame(){

        int i ;

        tx_on();
        for(i = 0; i < PREAMBLE_LENGTH; i++){

            send_byte(PREAMBLE_BYTE);
            //phaseShift = !phaseShift;
        }

        /* Start transmission of non-phase shifted frame */
        for(i = 0; i < FRAME_LENGTH; i++){
            send_byte( frame[i] );
        }

        tx_toggle();
        SMCLK_delay( (uint16_t) INTERFRAME_TIME_SMCLK);

}

#if DEBUG
    uint16_t checksum_debug;
#endif

void createFrame(uint8_t receiverId, MessageType messageType, uint8_t *payloadPtr){
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

#if DEBUG
    checksum_debug = checksum;
#endif

    resultCRC = calculateCRC(checksum);
    //  frame [byte 0| byte 1 ....| high CRC byte: low CRC byte  ]
    frame[FRAME_LENGTH - CRC_LENGTH] = resultCRC >> 8;
    frame[FRAME_LENGTH - CRC_LENGTH + 1] = resultCRC;
}



/*******************************************************************************
 * Clock System functions
 ******************************************************************************/

/* Set up all clocks (main clock @ 16 MHz, FRAM @ 8 MHz (with wait state) */
void clock_init() {

    /*
     * Configure one FRAM waitstate as required by the device datasheet for MCLK
     * operation beyond 8MHz _before_ configuring the clock system.
     */
    FRCTL0 = FRCTLPW | NWAITS_1;

    /* Make sure IO ports for crystal are set correctly */
    PJSEL0 |= BIT4 | BIT5;

    CSCTL0_H = CSKEY >> 8;                  // Unlock CS registers
    CSCTL1 = (DCORSEL | DCOFSEL_4);         // Set DCO to 16MHz
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;    // Set ACLK = LFXTCLK (32.768 kHz), SMCLK = MCLK = DCO (16 MHz)
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;      // Set all dividers to 1
    CSCTL4 &= ~LFXTOFF;                     // Turn on low-frequency crystal and enable it in active mode (AM) through LPM4.
    CSCTL6 |= SMCLKREQEN;                    // If SMCLK is requested in low power modes, it should be enabled.
    CSCTL0_H = 0;                           // Lock CS registers
}


/*******************************************************************************
 * GPIO functions
 ******************************************************************************/

void gpio_init() {
    // Configure GPIOs to a default state to ensure it's lowest power state
    P1OUT = 0;                                // All P1.x reset
    P1DIR = 0xFF;                             // All P1.x outputs
    P2OUT = 0;                                // All P2.x reset
    P2DIR = 0xFF;                             // All P2.x outputs
    P3OUT = 0;                                // All P3.x reset
    P3DIR = 0xFF;                             // All P3.x outputs
    P4OUT = 0;                                // All P4.x reset
    P4DIR = 0xFF;                             // All P4.x outputs
    PJOUT = 0;                                // All PJ.x reset
    PJDIR = 0xFFFF;                           // All PJ.x outputs

    // Set up XT1(low frequency crystal)
    PJSEL0 = BIT4 | BIT5;                     // For XT1

}


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    uint8_t testFrame[] = {0x11,0x22, 0x33, 0x44};

    gpio_init();
    tx_init();
    clock_init();
    timerA2_init();

    while(frame_cntr < 100)
    {
//        send_byte(0xaa);
        createFrame(0, 1, &testFrame);
        sendFrame();
//        tx_off();
             __delay_cycles(8000000);
//        tx_on();
//        __delay_cycles(1600000);
             frame_cntr++;
    }

    while(1);
	
	return 0;
}


















