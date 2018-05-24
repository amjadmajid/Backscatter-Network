#include "peripherals.h"

/****************************************************************************
  *                     GPIOs general initialization
******************************************************************************/

/** 
 * @description Configure GPIOs to a default state to ensure it's lowest power 
 *              state
 ----------------------------------------------------------------------------*/ 
void gpio_init() 
{
    P1OUT = 0;                                  // All P1.x reset 
    P1DIR = 0xFF;                               // All P1.x outputs 
    P2OUT = 0;                                  // All P2.x reset
    P2DIR = 0xFF;                               // All P2.x outputs
    P3OUT = 0;                                  // All P3.x reset
    P3DIR = 0xFF;                               // All P3.x outputs
    P4OUT = 0;                                  // All P4.x reset
    P4DIR = 0xFF;                               // All P4.x outputs
    PJOUT = 0;                                  // All PJ.x reset
    PJDIR = 0xFFFF;                             // All PJ.x outputs
    
}

/****************************************************************************
 *                    Clocks initialization
******************************************************************************/

/**
 * @description Set up all clocks: 
 *              MCLK and SMCLK set to 16 MHz
 *              ACLK set to 32 KHz and is fed by the external oscillator 
 *              FRAM is set to 8 MHz with wait state
 ----------------------------------------------------------------------------*/
void clock_init() {
    // Configure one FRAM waitstate as required by the device datasheet for MCLK
    // operation beyond 8MHz _before_ configuring the clock system.
    FRCTL0 = FRCTLPW | NWAITS_1;

    // Make sure IO ports for Low-Frequency Crystal Oscillator, are set correctly 
    // PJ.4/LFXIN,  PJ.5/LFXOUT
    PJSEL0 |= BIT4 | BIT5;    

    CSCTL0_H = CSKEY >> 8;                      // Unlock CS registers
    CSCTL1 = (DCORSEL | DCOFSEL_4);             // Set DCO to 16MHz
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | 
                             SELM__DCOCLK;      // Set ACLK = LFXTCLK (32.768 kHz), SMCLK = MCLK = DCO (16 MHz)
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;       // Set all dividers to 1
    CSCTL4 &= ~LFXTOFF;                         // Turn on low-frequency crystal and enable it in active mode (AM) through LPM4.
    CSCTL6 |= SMCLKREQEN;                       // If SMCLK is requested in low power modes, it should be enabled. (default setting)
    CSCTL0_H = 0;                               // Lock CS registers
}


/****************************************************************************
 *                    Timers initialization
******************************************************************************/

/**
 * @description initialize timerA1
 ----------------------------------------------------------------------------*/
void slow_timer_init() {
    TA1CTL = (TASSEL__ACLK |    // Use ACLK as source for timer
              MC__CONTINUOUS |  // Use continuous running mode
              TACLR |           // Clear timer
              TAIE);            // Enable Timer_A interrupts
}


/**
* @description  This interrupt handler handles interrupts from:
*               TA1CCR1
*               TA1CCR2
*               TA1R (TAIFG)
*
* Note: INT_Timer1_A0 handles interrupts from TA2CCR0, dedicated interrupt
----------------------------------------------------------------------------*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer1_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) Timer1_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TA1IV, TA1IV_TAIFG)) {
        case TA1IV_NONE:   break;               // No interrupt
        case TA1IV_TACCR1:                      // CCR1 routine
            TA1CCTL1 = 0x00;
//TODO uncomment the following line when the mac sublayer is implemented
//            preambleTimeout = true;
            TA1CTL &= ~TAIFG;
            break;
        case TA1IV_TACCR2:                      // CCR2 routine (slow_timer_delay hendler)
            TA1CCTL2 = 0x00;                    // Reset comparator settings
            TA1CTL &= ~TAIFG;                   // Clear Interrupt Flag
            __bic_SR_register_on_exit(LPM4_bits);
            break;
        case TA1IV_3:      break;               // reserved
        case TA1IV_4:      break;               // reserved
        case TA1IV_5:      break;               // reserved
        case TA1IV_6:      break;               // reserved
        case TA1IV_TAIFG:                       // overflow
            TA1CTL &= ~TAIFG;
            break;
        default: break;
    }
}

/** 
 * @description initialize timerA2
 ----------------------------------------------------------------------------*/
void fast_timer_init() {
    TA2CTL = (TASSEL__SMCLK |    // Use SMCLK as source for timer
              MC__CONTINUOUS |  // Use continuous running mode
              TACLR |           // Clear timer flag
              TAIE);            // Enable Timer_A interrupts
}

/**
* @description  This interrupt handler handles interrupts from:
*               TA2CCR1
*               TA2CCR2
*               TA2R (TAIFG)
*
* Note: INT_Timer2_A0 handles interrupts from TA2CCR0, dedicated interrupt
----------------------------------------------------------------------------*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER2_A1_VECTOR
__interrupt void Timer2_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER2_A1_VECTOR))) Timer2_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TA2IV, TA2IV_TAIFG)) {
        case TA2IV_NONE:   break;               // No interrupt
        case TA2IV_TACCR1:                      // CCR1 routine (fast_timer_delay handler)
            TA2CTL &= ~TAIFG;
            TA2CCTL1 = 0x00;                    // Reset comparator settings
            __bic_SR_register_on_exit(LPM0_bits);  // #define LPM0_bits (CPUOFF)
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


/**
 * @description initialize timerB0: Setup TimerB0 for PWM signal to be used to
 *              control the backscatter switch(transmitter) at SMCLK speed
 *              The ISR of timerB0 is moved to rx.c since it is used for bits
 *              deconding
 ----------------------------------------------------------------------------*/
void decode_timer_init() {
    TB0CTL = TBSSEL__SMCLK | MC__UP | TBCLR;    // SMCLK, up mode, clear TBR
}


