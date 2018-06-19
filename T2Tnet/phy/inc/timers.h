/*
 * timers.h
 *
 *  Created on: Sep 27, 2017
 *      Author: michel
 */


#ifndef PHY_TIMERS_H_
#define PHY_TIMERS_H_

void timers_init();
uint16_t read_TA1();
void slow_timer_delay(uint16_t ticks);
void start_preamble_timer(uint16_t cycles);
void fast_timer_delay(uint16_t ticks);
void startMacDownCounter(uint16_t cycles);


volatile bool macTimeout;

#endif /* PHY_TIMERS_H_ */
