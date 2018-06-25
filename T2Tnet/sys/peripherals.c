#include "peripherals.h"

/**
 * @description This function calculate the CRC.
 * @param       checksum a value to calculate the CRC for it.
 * @return      It returns the CRC value
-----------------------------------------------------------------------------*/
uint16_t calCRC(uint16_t checksum)
{
    CRCINIRES = 0xffff;                     // Init CRC16 HW module
    CRCDIRB = checksum;                 // Input data in CRC
    __no_operation();
    uint16_t resultCRC = CRCINIRES;
    return resultCRC;
}

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
 * @description initialize timerA2
 ----------------------------------------------------------------------------*/
void fast_timer_init() {
    TA2CTL = (TASSEL__SMCLK |    // Use SMCLK as source for timer
              MC__CONTINUOUS |  // Use continuous running mode
              TACLR |           // Clear timer flag
              TAIE);            // Enable Timer_A interrupts
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

void timers_init()
{
    slow_timer_init();
    fast_timer_init();
    decode_timer_init();
}


