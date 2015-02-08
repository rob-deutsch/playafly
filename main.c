#include <stdint.h>
#include <msp430.h>
#include <string.h>

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
    WDTCTL = CLOCK_NIGHT;

    // TODO: Set the time from Flash
    unix_time = 0;

    // Setup the timer
    CCR0 = 16-1;             // PWM Period
    CCTL1 = OUTMOD_7;          // CCR1 reset/set
    CCR1 = 0;                // CCR1 PWM duty cycle
    TACTL = TASSEL_2 + MC_1;   // SMCLK, up mode

    // Enable the interrupt and set the ports
    IE1=WDTIFG;
    P1DIR=BIT6;
    P1SEL=BIT6;
    
    BCSCTL1 &= !XTS; // LFXT1 low frequency mode
    BCSCTL3 |= LFXT1S_2; // LFXT1 = VLO

    //state = DAY;

    _BIS_SR(LPM0_bits + GIE); // Enter LPM3 w/interrupt
    
}

#pragma vector=WDT_VECTOR
__interrupt void wdttimer(void)
{
    static int frac_second = 0;
    static int direction = 1;

    enum state_enum state_should_be_in;
    if ((unix_time / 10) % 2 == 0) {
        state_should_be_in = NIGHT;
    } else {
        state_should_be_in = DAY;
    }

    if (state_should_be_in == NIGHT) {
        WDTCTL = CLOCK_NIGHT;
        if (frac_second == 16-1) {
            direction = -1;
        } else if (frac_second == 0) {
            direction = 1;
        }

        frac_second += direction;

        CCR1 = frac_second;

        if (frac_second == 0 || frac_second == 15) {
            unix_time++;
        }

    } else {
        WDTCTL = CLOCK_DAY;
        CCR1 = 0;
        unix_time++;
    }
    IFG1&=~WDTIFG;
}