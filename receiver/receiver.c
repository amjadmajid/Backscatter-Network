#include <receiver.h>

void recieve_state();
void *detectFrameState();
void *catchFrameState();

/* Variables used for bit decoding in ISR */
StateFunc frameDetectionStateFunc;
volatile uint8_t bitCounter, byteCounter, decodedBits;
volatile uint16_t timeCapture;
volatile bool potentialZero, recordFrame;
volatile uint8_t frame[FRAME_LENGTH];

/* Enable supply of transceiver */

#ifdef NEW_HW
// TODO Timer_A requires an ISR !!
void timer_init() {
    TA0CTL = TASSEL__SMCLK | MC__UP | TACLR;    // SMCLK, up mode, clear TBR
}

void timer_startCapture(){
    TA0CCTL2 =  CM_3    // Capture both rising and falling edge,
              | CCIS_0  // Use CCI3B = P1.6,
              | SCS     // Synchronous capture,
              | CAP     // Enable capture mode,
              | CCIE;   // Enable capture interrupt
    TA0CCR2 = 0xFFFF;
}

#else

/**
 * @description     it causes the antenna matching circuit to be matched, to receive the signal
 ----------------------------------------------------------------------------*/
void recieve_state()
{
    RFSW_DIR |= A0 | A1;
    RFSW_OUT |= A0 | A1;
}


void radio_on(){
    recieve_state();
    RADIO_EN_DIR |= RADIO_EN_PIN;
    RADIO_EN_OUT |= RADIO_EN_PIN;
}

void timer_init() {
    TB0CTL = TBSSEL__SMCLK | MC__UP | TBCLR;    // SMCLK, up mode, clear TBR
}

void timer_startCapture(){
    TB0CCTL3 =  CM_3    // Capture both rising and falling edge,
              | CCIS_1  // Use CCI3B = P1.6,
              | SCS     // Synchronous capture,
              | CAP     // Enable capture mode,
              | CCIE;   // Enable capture interrupt
    TBCCR0 = 0xFFFF;
}
#endif

void rx_init(){
    /* Configure RX port to use as input for TB0 Timer CCR3 */
    RX_DIR &= ~RX_PIN;                          // configure RX port as an input port
#ifdef NEW_HW

#else
    P1SEL0 |= BIT6;                             // P1.6 options select (primary module function= TB0 CCR3)
#endif
    /* Initialize state pointer for frame detection and recording */
    frameDetectionStateFunc = (StateFunc) detectFrameState;

    /* Initialize variables for RX decoding */
    decodedBits = 0;
    bitCounter = 0;
    byteCounter = 0;
    potentialZero = false;
}

void *detectFrameState(){
    if(decodedBits == START_DELIMITER_BYTE){
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
        byteCounter = 0;
        P1OUT |= BIT0;    // set the LED pin
        __delay_cycles(160000);
        P1OUT &=~BIT0;    // CLear the LED pin
        return detectFrameState;
    }
    return catchFrameState;
}

#pragma vector=TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void) {
    switch(__even_in_range(TB0IV, TB0IV_TBIFG)) {
        case TB0IV_NONE:   break;               // No interrupt
        case TB0IV_TBCCR1: break;               // CCR1 not used
        case TB0IV_TBCCR2: break;               // CCR2 not used
        case TB0IV_TBCCR3: /* Decode bits algorithm */
            /* Save captured timer value and reset timer */
            timeCapture = TB0CCR3;
            TB0CTL |= TBCLR;

            /* Check if time corresponds to a valid zero (= BIT_LENGTH/2) */
            if( timeCapture >= (BIT_LENGTH_SMCLK >> 1) - TIMER_JITTER_LOW_SMCLK && timeCapture < (BIT_LENGTH_SMCLK - (BIT_LENGTH_SMCLK >> 2)) ){
                if(potentialZero == 0){
                    potentialZero = true;
                } else {
                    potentialZero = false;
                    decodedBits = (decodedBits <<1);
                    bitCounter++;
                }
            /* Check if time corresponds to a valid one (= BIT_LENGTH) */
            } else if( timeCapture >= (BIT_LENGTH_SMCLK - (BIT_LENGTH_SMCLK >> 2)) && timeCapture <= BIT_LENGTH_SMCLK + TIMER_JITTER_HIGH_SMCLK ){
                potentialZero = false;
                decodedBits = (decodedBits << 1) +1;
                bitCounter++;
            } else {
                potentialZero = false;
                decodedBits = 0;
                bitCounter = 0;
            }

            /* Detection and recording of frames */
            frameDetectionStateFunc = (StateFunc)(*frameDetectionStateFunc)();
            break;
        case TB0IV_TBCCR4: break;               // CCR4 not used
        case TB0IV_TBCCR5: break;               // CCR5 not used
        case TB0IV_TBCCR6: break;               // CCR6 not used
        case TB0IV_TBIFG:                       // overflow
          break;
        default: break;
    }
}

/*******************************************************************************
 * GPIO functions
 ******************************************************************************/

void gpio_init() {
    // Configure GPIOs to a default state to ensure it's lowest power state
    P1OUT = 0x00;                                // All P1.x reset
    P1DIR = 0xFF;                                // All P1.x outputs
    P2OUT = 0x00;                                // All P2.x reset
    P2DIR = 0xFF;                                // All P2.x outputs
    P3OUT = 0x00;                                // All P3.x reset
    P3DIR = 0xFF;                                // All P3.x outputs
    P4OUT = 0x00;                                // All P4.x reset
    P4DIR = 0xFF;                                // All P4.x outputs
    PJOUT = 0x00;                                // All PJ.x reset
    PJDIR = 0xFFFF;                              // All PJ.x outputs
    PJSEL0 |= BIT4 | BIT5;
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
    PJSEL0 |= BIT4 | BIT5;

    CSCTL0_H = CSKEY >> 8;                  // Unlock CS registers
    CSCTL1 = (DCORSEL | DCOFSEL_4);         // Set DCO to 16MHz
    CSCTL2 =  SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;    // Set ACLK = LFXTCLK (32.768 kHz), SMCLK = MCLK = DCO (16 MHz)
    CSCTL3 =  DIVA__1 | DIVS__1 | DIVM__1;      // Set all dividers to 1
    CSCTL4 &= ~LFXTOFF;
    CSCTL6 |= SMCLKREQEN;                    // If SMCLK is requested in low power modes, it should be enabled.
    CSCTL0_H = 0;                           // Lock CS registers
}

void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;
    gpio_init();
    clock_init();
#ifndef NEW_HW
    radio_on();
#endif
    rx_init();
    timer_init();
    _BIS_SR(GIE);
}

int main(void) {
    init();
    timer_startCapture(); // Activate timer B0 to capture timing
    while(1);
	
	return 0;
}
