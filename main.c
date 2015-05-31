#include <stdint.h>
#include <msp430.h>
#include <string.h>

// TODO: Set the time in Makefile
#ifndef UNIX_TIME
#define UNIX_TIME 0
#endif

volatile long unix_time = UNIX_TIME;
volatile unsigned int unix_time_frac = 0;

enum state_enum {
    DAY,
    NIGHT
};

enum state_enum state;

unsigned int prog[16] = {1,1,2,3,5,9,14,23,36,52,69,86,100,110,117,127};
//unsigned int prog[16] = {1,9,69,122,115,104,86,63,40,23,12,6,3,1,1,0};

int main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    // Setup the timer
    CCR0 = 128-1;             // PWM Period
    CCTL1 = OUTMOD_7;          // CCR1 reset/set
    CCR1 = 0;                // CCR1 PWM duty cycle
    TACTL = TASSEL_2 + MC_1 + ID_0 + ID_1;   // SMCLK, up mode

    // Enable the interrupt and set the ports
    IE1=WDTIFG;
    P1DIR=BIT6;
    P1SEL=BIT6;
    
    BCSCTL1 &= !XTS; // LFXT1 low frequency mode
    //BCSCTL3 |= LFXT1S_2; // LFXT1 = VLO
    BCSCTL3 |= XCAP_3;            // xtal has 12.5 pF caps

    // Set the state. Set up WTD. Enter LPM0
    state = NIGHT;

    WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL + WDTSSEL + WDTIS1 + WDTNMI + WDTNMIES;
    IFG1 &= ~(WDTIFG | NMIIFG);
    IE1 |= WDTIE | NMIIE;
    _BIS_SR(LPM0_bits + GIE); // Enter LPM0 w/interrupt
    
}



#pragma vector=WDT_VECTOR
__interrupt void wdttimer(void)
{
    //P1OUT ^= BIT0;

    unix_time_frac++;
    unix_time_frac = unix_time_frac % 64; // Modulo 64
    if (!unix_time_frac) unix_time++;   // If unix_time_frac returned to 0
                                        // then increment unix time

    enum state_enum state_should_be_in;
    if ((unix_time / 30) % 2 == 0) {
        state_should_be_in = NIGHT;
    } else {
        state_should_be_in = DAY;
    }

    if (state_should_be_in != state) {
        state = state_should_be_in;
        if (state_should_be_in == DAY) {
            CCR1 = 0;
            _BIC_SR_IRQ(LPM4_bits);
            _BIS_SR_IRQ(LPM3_bits + GIE); // Enter LPM3 w/interrupt
        } else if (state_should_be_in == NIGHT) {
            _BIC_SR_IRQ(LPM4_bits);
            _BIS_SR_IRQ(LPM0_bits + GIE); // Enter LPM0 w/interrupt
        }
    }

    int pos;
    if (unix_time_frac / 32 == 0) {
        pos = unix_time_frac / 4;
    } else {
        pos = 15 - unix_time_frac / 4;
    }
    
    if (state == NIGHT) {
        TAR=0;
        CCR1 = prog[pos];
    }

    IFG1&=~WDTIFG;
}

#pragma vector=NMI_VECTOR
__interrupt void resetnmi(void)
{

    unix_time_frac = 0;
 
    int rem = (unix_time % 60); // 0 if unix_time is even, 2 if it is odd
//    unix_time = (unix_time / 2) * 2; // Round down to even number
    unix_time -= rem; // Round back up if required

    WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL + WDTSSEL + WDTIS1 + WDTNMI + WDTNMIES;    IFG1 &= ~( NMIIFG);
    IE1 |= NMIIE;

}