#include <phy/radio.h>

/** 
 * @description     Enable the transceiver
 ----------------------------------------------------------------------------*/
void radio_enable()
{
    RADIO_DIR |= ON;            // set the transceiver enable bit to be output
    RADIO_OUT |= ON;            // enable the transceiver
}

/** 
 * @description     disable the transceiver
 ----------------------------------------------------------------------------*/
void radio_disable()
{
    RADIO_OUT &= ~ON;            // enable the transceiver
}

/** 
 * @description     set the direction pins for the RF switch (A1, A0) 
 ----------------------------------------------------------------------------*/
void rf_sw_init()
{
    RFSW_OUT &= ~(A0 | A1);     // set the output to low
    RFSW_DIR |= (A0 | A1);      //set the two pins as output
}

void radio_init()
{
    radio_enable();
    rf_sw_init();
}

/**
 * @description     it causes the antenna matching circuit to be mismatched, to backscatter
 * @param           phaseShift indicates with a phase shift is required or not
 ----------------------------------------------------------------------------*/
void backscatter_state(bool phaseShift)
{
    if(phaseShift)
    {
        RFSW_OUT &= ~A0;
        RFSW_OUT |= A1;
    }else{
        RFSW_OUT |= A0;
        RFSW_OUT &= ~A1;
    }
}

/**
 * @description     it causes the antenna matching circuit to be matched, to receive the signal
 ----------------------------------------------------------------------------*/
void recieve_state()
{
    RFSW_DIR |= A0 | A1;
    RFSW_OUT |= A0 | A1;
}

