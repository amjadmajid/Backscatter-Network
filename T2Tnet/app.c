#include <msp430fr5969.h>
#include <net.h>

//#define TX_APP

uint16_t received_frame_correct = 0;
uint16_t received_frame_incorrect = 0;
int16_t transmitted_frame = 100;


void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    gpio_init();
    clock_init();
    timers_init();
    radio_init();
    _BIS_SR(GIE);

#if DEBUG
     P1DIR |= BIT0;
#endif
}
int main(void) {
    init();
    mac_init();
    uint8_t testFrame[] = {0x11, 0x22, 0x33,0x44};

#ifdef TX_APP
    while(trasmitted_frame--)
    {
        mac_fsm(preamble_sampling);

        slow_timer_delay(2500);   // reduce the transmission rate (random guess)
        //TODO transmit with a probability (use rand)
        uint8_t testFrame[] = {0x11, 0x22, 0x33,0x44};
        create_frame(0, 1, testFrame);
    }

    while(1);
#else
    while(1)
    {
        mac_fsm(preamble_sampling);
    }
#endif

    return 0;
}






