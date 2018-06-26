#include <msp430fr5969.h>
#include <stdlib.h>
#include <time.h>
#include <net.h>
#include "debug.h"


#define TX_APP

uint16_t transmitted_frame = 100;
uint16_t dummy_debug;

void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    gpio_init();
    clock_init();
    timers_init();
    radio_init();
    _BIS_SR(GIE);

    srand((unsigned) 2 );

#if DEBUG
    leds_init();
#endif

}
int main(void) {
    init();
    mac_init();
    uint8_t testFrame[] = {0x11, 0x22, 0x33,0x44};

    while(1)
    {
        mac_fsm(preamble_sampling);
#ifdef TX_APP
        slow_timer_delay(1000);    // reduce the transmission rate (random guess)
        //TODO transmit with a probability (use rand)
        dummy_debug = rand();
        if( (dummy_debug % 4) == 0 )
        {
            set_node_id(1);
            set_frame_type(0);
            set_frame_receiver_id(1);
            set_frame_sender_id( get_node_id() );
            set_ttl(10);
            create_frame( &testFrame[0], &tx_buf);
        }
#endif
    }
    return 0;
}






