/*
 * frameValidation.h
 *
 *  Created on: 30 May 2018
 *      Author: amjad
 */

#include "sys.h"
#include "phy.h"

#ifndef MAC_FRAMEVALIDATION_H_
#define MAC_FRAMEVALIDATION_H_


void frameValidation(func_ptr frameValidationState);
void *waitFrameState();
void *checkCRCState();
void *savePayloadState();

#endif /* MAC_FRAMEVALIDATION_H_ */
