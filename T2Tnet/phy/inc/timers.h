/*
 * timers.h
 *
 *  Created on: Sep 27, 2017
 *      Author: Amjad, Michel
 */
#include <stdint.h>
#include <stdbool.h>
#include <msp430fr5969.h>
#include "peripherals.h"

#ifndef PHY_TIMERS_H_
#define PHY_TIMERS_H_

volatile bool macTimeout;

void slow_timer_delay(uint16_t ticks);
void fast_timer_delay(uint16_t ticks);
void mac_down_cntr(uint16_t cycles);

#endif /* PHY_TIMERS_H_ */
