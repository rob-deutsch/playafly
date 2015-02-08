#include <stdint.h>
#include <msp430.h>
#include <string.h>

void main(void)
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL + WDTSSEL + WDTIS1; 

     // Set up ALCK
    BCSCTL1 &= !XTS; // LFXT1 low frequency mode
    BCSCTL3 |= LFXT1S_2; // LFXT1 = VLO

    // Set up green LED port
    P1DIR |= BIT6;             // P1.2 to output
    P1SEL |= BIT6;             // P1.2 to TA0.1

    // Enable the interrupt
    IE1=WDTIFG;

    CCR0 = 18;             // PWM Period
    CCTL1 = OUTMOD_7;          // CCR1 reset/set
    CCR1 = 0;                // CCR1 PWM duty cycle
    TACTL = TASSEL_2 + MC_1;   // SMCLK, up mode

    _BIS_SR(LPM0_bits + GIE);        // Enter LPM0
}


#pragma vector=WDT_VECTOR
__interrupt void wdttimer(void)
{
    static int frac_second = 0;
    static int direction = 1;

    if (frac_second == 17) {
        direction = -1;
    } else if (frac_second == 1) {
        direction = 1;
    }

    frac_second += direction;

    CCR1 = frac_second;

    IFG1&=~WDTIFG;
}