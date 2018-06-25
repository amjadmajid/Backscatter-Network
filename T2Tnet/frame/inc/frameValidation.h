/*
 * frameValidation.h
 *
 *  Created on: 30 May 2018
 *      Author: Amjad
 */

#include <stdint.h>

#include "debug.h"
#include "buffers.h"
#include "custom_data_type.h"
#include "frame.h"
#include "node.h"
#include "rx.h"
#include "tx.h"

#ifndef MAC_FRAMEVALIDATION_H_
#define MAC_FRAMEVALIDATION_H_

void frameValidation(func_ptr frameValidationState);
void *waitFrameState();

rbuf_t rx_data_buf;

#endif /* MAC_FRAMEVALIDATION_H_ */
