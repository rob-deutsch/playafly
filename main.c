#include <stdint.h>
#include <msp430.h>
#include <string.h>
#include "option.h"

int main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL + WDTSSEL + WDTIS0;

    // Enable the interrupt and set the ports
    IE1=WDTIFG;
    P1DIR=BIT0+BIT6;
    P1OUT=BIT0;
    P1IE=BIT3;
    
    //BCSCTL2=DIVS_3;       // slow down the smclk by division to make the blinking slow.details in clock
    
    BCSCTL1 &= !XTS; // LFXT1 low frequency mode
    BCSCTL3 |= LFXT1S_2; // LFXT1 = VLO

    // module tutorial
    
    _EINT();
    
    
    // Need at least one of the following:
    
    _BIS_SR(LPM3_bits + GIE); // Enter LPM3 w/interrupt

    //while(1);
}

#pragma vector=WDT_VECTOR
__interrupt void wdttimer(void)
{
    P1OUT^=BIT0+BIT6;
    IFG1&=~WDTIFG;
}