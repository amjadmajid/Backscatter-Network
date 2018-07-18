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
    debug_pins_init();
#endif

}
int main(void) {

    init();
    set_p1_7();
    mac_init();
    uint8_t testFrame[] = {0x11, 0x22, 0x33,0x44};
    set_node_id(1);

    while(1)
    {
        mac_fsm(preamble_sampling);
#ifdef TX_APP
        dummy_debug = rand();
        slow_timer_delay((SLEEP_PERIOD_ACLK/2) + dummy_debug % 1000);    // reduce the transmission rate (random guess)
        //TODO transmit with a probability (use rand)
        if( (dummy_debug % 2) == 0 )
        {
            set_frame_type(0);
            set_frame_receiver_id(1);
            set_frame_sender_id( get_node_id() );
            set_ttl(10);
            testFrame[0]++;
            create_frame( &testFrame[0], &tx_buf);
#if DEBUG
      set_p2_5();
      clear_p2_5();
#endif
        }
#endif
    }
    return 0;
}






