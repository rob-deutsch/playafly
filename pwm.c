#include <stdint.h>
#include <msp430.h>
#include <string.h>

int main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    // Set up ALCK
    BCSCTL1 &= !XTS; // LFXT1 low frequency mode
    BCSCTL3 |= LFXT1S_2; // LFXT1 = VLO

    // Set up the port
    P1DIR=BIT0;
    P1SEL=BIT0;

    CCR0 = 1000-1;
    CCTL1 = OUTMOD_7;
    CCR1 = 1;
    TACTL = TASSEL_2 + MC_1;
    

    _BIS_SR(LPM0_bits); // Enter LPM0
    
}