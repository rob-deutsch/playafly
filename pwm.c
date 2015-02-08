#include <stdint.h>
#include <msp430.h>
#include <string.h>
#include "option.h"

unsigned long unix_time;

enum state_enum {
    DAY,
    NIGHT
};

enum state_enum state;

#define CLOCK_DAY WDTPW + WDTTMSEL + WDTCNTCL + WDTSSEL + WDTIS0
#define CLOCK_NIGHT WDTPW + WDTTMSEL + WDTCNTCL + WDTSSEL + WDTIS1

int main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = CLOCK_DAY;

    // TODO: Set the time from Flash
    unix_time = 0;

    // Enable the interrupt and set the ports
    IE1=WDTIFG;
    P1DIR=BIT0;
    P1OUT=BIT0;
    P1IE=BIT3;
    
    BCSCTL1 &= !XTS; // LFXT1 low frequency mode
    BCSCTL3 |= LFXT1S_2; // LFXT1 = VLO

    state = DAY;

    _BIS_SR(LPM3_bits + GIE); // Enter LPM3 w/interrupt
    
}

#pragma vector=WDT_VECTOR
__interrupt void wdttimer(void)
{
    static int frac_second = 0;

    enum state_enum state_should_be_in;
    if ((unix_time / 10) % 2 == 0) {
        state_should_be_in = NIGHT;
    } else {
        state_should_be_in = DAY;
    }

    if (state_should_be_in == NIGHT) {
        WDTCTL = CLOCK_NIGHT;
        frac_second++;
        frac_second = frac_second % 16;
        if (frac_second == 0) {
            unix_time++;
            P1OUT^=BIT0;
        }

    } else {
        WDTCTL = CLOCK_DAY;
        P1OUT = 0;
        unix_time++;
    }
    IFG1&=~WDTIFG;
}