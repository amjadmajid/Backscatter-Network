#include "buffers.h"
#include "frameValidation.h"
#include "tx.h"
#include "rx.h"

#ifndef MAC_H_
#define MAC_H_


/**
 *  MAC related timing
 */

/*
Mac steps: TODO TO BE UPDATED
1. trigger mac_fsm()
    1.1 preamble_sampling
        1.1.1 receiving_state
        1.1.2 sleep
    1.2 if the channel is busy
        1.2.1 receive
        1.2.2 return mac_fsm
    1.3 if the channel is free and the tx buffer is not empty
        1.3.1 transmit
        1.3.2 return mac_fsm
    1.4 else
        1.4.1 turn off radio (or only disable interrupts, if the radio requires long time to reach stable state)
        1.4.2 switch to computation
*/

/*
Switching between computation and mac
1. timer based
    1.1 the computation is interrupted using timer and all the mac is handled within
        the interrupt, nested interrupt handling is required.
    1.2 task granularity based switching mechanism: after each x weighted computation tasks, execute the mac
*/

/*
Stating point: mac without computation
*/


#define MAC_PREAMBLE_SAMPLING_INTERVAL  PREAMBLE_SAMPLING_INTERVAL       // sample for 20 bits to detect a preabmle "byte"
#define MAC_RX_TIMEOUT_ACLK				500	                            // Ticks with 32.768 kHz CLK
#define MAC_CSMA_PERIOD_ACLK			    3000						        // Radio Settling time = 7.5 ms, so minimum ACLK cycles (32kHz) = ~250 cycles => 500 cycles = ~15 ms, 650 cycles = ~20 ms
#define MAC_EXTRA_CSMA_MAX_ACLK			500						        // Maximum extra ticks to do CSMA with 32.768 kHz CLK (i.e. max 10 bytes CSMA)

void mac_init();
void* preamble_sampling();
func_ptr mac_func;
void mac_fsm(func_ptr mac_func);

#endif  // end of MAC_H_
