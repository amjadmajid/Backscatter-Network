#include "mac.h"
#include <stdbool.h>

/*
Mac steps:
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


void mac_init()
{

    // initialize the buffers
    rbuf_init(&tx_buf, TX_BUFFER_SIZE);
    rbuf_init(&rx_buf, RX_BUFFER_SIZE);

}


 void mac_fsm(func_ptr mac_func)
{
    while(1)
    {
        mac_func = (func_ptr) (*mac_func)();
         __delay_cycles(1000);
    }
}

void channel_assessment()
{
    receive_state();
    start_capture(); 
    slow_timer_delay( (uint16_t) MAC_PREAMBLE_SAMPLING_INTERVAL );
    stop_capture();
}

void* preamble_sampling()
{
	//uint16_t randomCsmaTime = rand() % MAC_EXTRA_CSMA_MAX_ACLK;
    // preamble sampling
    channel_assessment();

    //if the channel is busy 
    if(channelBusy == true)
    {
        channelBusy = false;
        return receive;
    }
    return transmit;
}


void* receive()
{
    startMacRxDownCounter((uint16_t) MAC_RX_TIMEOUT_ACLK);
    receive_state();
    start_capture(); 

    while( macRxTimeout != 0 );   //TODO Process the received packet instead of dummy delay

    return preamble_sampling;
}

void* transmit()
{
    //check if the transmit buffer is not empty
    if( !rbuf_empty(&tx_buf) )
    {
        //send_frame should return an error code
        backscatter_frame();
    }
    return preamble_sampling;
}
