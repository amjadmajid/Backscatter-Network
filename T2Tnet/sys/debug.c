/*
 * debug.c
 *
 *  Created on: 22 Jun 2018
 *      Author: Amjad
 */

#include "debug.h"

/**
 * @description initialize all the leds
 ----------------------------------------------------------------------------*/
void leds_init()
{
    red_led_init();
    green_led_init();
}

/**
 * @description initialize the red led
 ----------------------------------------------------------------------------*/
void red_led_init()
{
    P4DIR |= BIT6;
}

/**
 * @description initialize the green led
 ----------------------------------------------------------------------------*/
void green_led_init()
{
    P1DIR |= BIT0;
}

/**
 * @description blink red led
 * @param   cycles: number of loops for the delay
 ----------------------------------------------------------------------------*/
void red_led_blink(uint32_t cycles)
{
    volatile uint32_t cycles_cntr = 0;
    P4OUT |= BIT6;    // set the LED pin
    while(cycles_cntr != cycles)
    {
        cycles_cntr++;
    }
    P4OUT &=~BIT6;    // CLear the LED pin
}

/**
 * @description blink green led
 * @param   cycles: number of loops for the delay
 ----------------------------------------------------------------------------*/
void green_led_blink(uint32_t cycles)
{
    volatile uint32_t cycles_cntr = 0;
    P1OUT |= BIT0;    // set the LED pin
    while(cycles_cntr != cycles)
    {
        cycles_cntr++;
    }
    P1OUT &=~BIT0;    // CLear the LED pin
}

