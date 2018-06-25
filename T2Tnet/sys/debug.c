/*
 * debug.c
 *
 *  Created on: 22 Jun 2018
 *      Author: Amjad
 */

#include "debug.h"

void leds_init()
{
    red_led_init();
    green_led_init();
}

void red_led_init()
{
    P4DIR |= BIT6;
}

void green_led_init()
{
    P1DIR |= BIT0;
}

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

