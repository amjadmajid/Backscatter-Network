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
 *          CCR2: Used by ACLK_delay()
 * TA2: SMCLK - Continuous
 *          CCR0: (Available)
 *          CCR1: Used by SMCLK_delay()
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
inline uint16_t __read_TA1()
{
    uint16_t t1,t2;
    do{
        t1 = TA1R;
        t2 = TA1R;
    }while(t1 != t2);
    return t1;
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
    TA1CCR1 = __read_TA1() + cycles;
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
    TA1CCR2 = __read_TA1() + ticks;    // Set compare value
    TA1CTL &= ~TAIFG;               // Clear interrupt flag
    TA1CCTL2 = CCIE;                // Enable Register comparator interrupt
    /* Put MCU to low-power operation mode 3 and wait for timer interrupt. */
    __bis_SR_register(LPM0_bits | GIE);
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

