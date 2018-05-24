#ifndef RADIO_H_
#define RADIO_H_

#include "globals.h"
#include "timers.h"
#include "rx.h"
#include "tx.h"
#include "utilities.h"

void radio_enable();
void radio_disable();
void rf_sw_init();
void radio_init();
void backscatter_state(bool phaseShift);
void backscatter(bool phaseShift);
void recieve_state();

#endif // end of the RADIO_H_
