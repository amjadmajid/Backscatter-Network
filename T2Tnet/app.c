#include <msp430fr5969.h>
#include <stdlib.h>
#include <time.h>
#include <net.h>

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
     P1DIR |= BIT0;
#endif
}
int main(void) {
    init();
    mac_init();
    const uint8_t testFrame[] = {0x11, 0x22, 0x33,0x44};

    while(1)
    {
        mac_fsm(preamble_sampling);
#ifdef TX_APP
        slow_timer_delay(10000);   // reduce the transmission rate (random guess)
        //TODO transmit with a probability (use rand)
        dummy_debug = rand();
        if( dummy_debug % 2 )
        {
            create_frame(0, 1, testFrame);
        }
#endif
    }
    return 0;
}






