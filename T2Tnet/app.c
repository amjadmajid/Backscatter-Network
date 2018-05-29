#include <msp430fr5969.h>
#include <t2t.h>

#define MAC 1

void init()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    gpio_init();
    clock_init();
    timers_init();
    radio_init();
    _BIS_SR(GIE);
}
int main(void) {

    init();
    mac_init();
    uint8_t testFrame[] = {0x01, 0x02, 0x03,0x04};

#if MAC

    mac_fsm(preamble_sampling);

#elif TX       // Transmitter
    while(1)
    {
        create_frame(0, 1, testFrame);
        backscatter_frame();
        __delay_cycles(160000);
    }
#else       // Receiver
    rx_init();
    recieve_state();
    start_capture();
#endif
    return 0;
}






