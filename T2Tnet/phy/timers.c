#include "radio.h"

void timers_init()
{
    slow_timer_init();
    fast_timer_init();
    decode_timer_init();
}

/*******************************************************************************
 * Timer functions
 ******************************************************************************/
//TODO correct the description
/* Timers overview:
 * TA0: (Available)
 * TA1: ACLK - Continuous
 *          CCR0: MAC: For RX timeout
 *          CCR1: PHY: Preamble duration during TX
 *          CCR2: Used by slow_timer_delay()
 * TA2: SMCLK - Continuous
 *          CCR0: (Available)
 *          CCR1: Used by fast_timer_delay()
 * TA3: (Available)
 * TB0: SMCLK - Up
 *          CCR0: (Available)
 *          CCR1: (Available)
 *          CCR2: (Available)
 *          CCR3: PHY: Used for Capturing RX events
 *          CCR4: (Available)
 *          CCR5: (Available)
 *          CCR6: (Available)
 */

/**
 * @description read timerA1, without stopping it, using majority vote technique
 *----------------------------------------------------------------------------*/
uint16_t read_TA1()
{
    uint16_t t1,t2;
    do{
        t1 = TA1R;
        t2 = TA1R;
    }while(t1 != t2);
    return t1;
}

void startMacRxDownCounter(uint16_t cycles)
{
    macRxTimeout = 0;
    TA1CCR0 = read_TA1() + cycles;
    TA1CCTL0 = CCIE;     // Enable Interrupts on Comparator register
}

// Timer1 A0 interrupt service routine
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) Timer1_A0_ISR (void)
{
    macRxTimeout = 1;
    TA1CCTL0 = 0x00;
    stop_capture();
}

/**
 * @description This function provide a delay service using TimerA1 and
                 capture/compare channel 1.
 * @param       It expects the length of the delay in ACLK cycles
-----------------------------------------------------------------------------*/
void start_preamble_timer(uint16_t cycles)
{
    //TODO uncomment the following line when mac sublayer implementation is done
//    preambleTimeout = false;
    TA1CTL |= TACLR;
    TA1CCR1 = read_TA1() + cycles;
    TA1CCTL1 = CCIE;     // Enable Interrupts on Comparator register
}

/**
 * @description This function provide a delay service using TimerA1 and
                 capture/compare channel 2. It also puts the MCU into Low power
                 mode 0
 * @param       It expects the length of the delay in ACLK cycles
-----------------------------------------------------------------------------*/
void slow_timer_delay(uint16_t ticks)
{
    TA1CCR2 = read_TA1() + ticks;    // Set compare value
    TA1CTL &= ~TAIFG;               // Clear interrupt flag
    TA1CCTL2 = CCIE;                // Enable Register comparator interrupt
    /* Put MCU to low-power operation mode 3 and wait for timer interrupt. */
    __bis_SR_register(LPM0_bits | GIE);
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
 * @description This function provide a delay service using TimerA2 and
                 capture/compare channel 1. It also puts the MCU into Low power
                 mode 0
 * @param       It expects the length of the delay in SMCLK cycles
-----------------------------------------------------------------------------*/
void fast_timer_delay(uint16_t ticks)
{
    TA2CTL &=~MC__CONTINUOUS;           // stop the TimerA2 (to read its value)
    TA2CCR1 = TA2R + ticks;             // Set compare value
    TA2CCTL1 = CCIE;                    // Enable Register comparator interrupt
    TA2CTL &= ~TAIFG;                   // Clear interrupt flag
    TA2CTL |= MC__CONTINUOUS;           // resume the timer
    // /* Put MCU to low-power operation mode 3 and wait for timer interrupt. */
    __bis_SR_register(LPM0_bits | GIE);
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

