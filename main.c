#include <stdint.h>
#include <msp430.h>
#include <string.h>
#include "option.h"

#define WDTCONFIG (WDTPW+WDTCNTCL+WDTTMSEL) //configure wdt in timer mode and clear the count register

int main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL;
    IE1=WDTIFG;
    P1DIR=BIT0+BIT6;
    P1OUT=BIT0;
    P1IE=BIT3;
    
    BCSCTL2=DIVS_3;       // slow down the smclk by division to make the blinking slow.details in clock

    // module tutorial
    
    _EINT();
    while(1);
}

#pragma vector=WDT_VECTOR
__interrupt void wdttimer(void)
{
    P1OUT^=BIT0+BIT6;
    IFG1&=~WDTIFG;
}