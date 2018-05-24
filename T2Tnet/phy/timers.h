/*
 * timers.h
 *
 *  Created on: Sep 27, 2017
 *      Author: michel
 */

#ifndef PHY_TIMERS_H_
#define PHY_TIMERS_H_

#include "radio.h"
void timers_init();
inline uint16_t __read_TA1();
void slow_timer_delay(uint16_t ticks);
void start_preamble_timer(uint16_t cycles);
void fast_timer_delay(uint16_t ticks);

#endif /* PHY_TIMERS_H_ */
