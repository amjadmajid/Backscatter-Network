/*
 * rx.h
 *
 *  Created on: Sep 27, 2017
 *      Author: michel
 */

#ifndef PHY_RX_H_
#define PHY_RX_H_

#include "custom_data_type.h"

/* Initialize receiver */
void rx_init();

/* Enable or disable interrupt capability of receiver */
void start_capture();
void stop_capture();
void *detectFrameState();
void *catchFrameState();


bool channelBusy;

#endif /* PHY_RX_H_ */
