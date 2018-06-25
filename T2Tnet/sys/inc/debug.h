/*
 * debug.h
 *
 *  Created on: 14 Jun 2018
 *      Author: Amjad
 */
#include <stdint.h>
#include <msp430fr5969.h>

#define DEBUG 1

#ifndef SYS_INC_DEBUG_H_
#define SYS_INC_DEBUG_H_

void leds_init();
void red_led_init();
void green_led_init();
void red_led_blink(uint32_t cycles);
void green_led_blink(uint32_t cycles);

uint16_t received_frame_correct;
uint16_t received_frame_incorrect;



#endif /* SYS_INC_DEBUG_H_ */
