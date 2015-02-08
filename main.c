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

unsigned int prog[16] = {1,1,2,3,5,9,14,23,36,52,69,86,100,110,117,122};
//unsigned int prog[16] = {1,9,69,122,115,104,86,63,40,23,12,6,3,1,1,0};

void set_state(enum state_enum requested_state) {
    state = requested_state;
    if (requested_state == DAY) {
        CCR1 = 0;
        WDTCTL = CLOCK_DAY;
        _BIS_SR(LPM3_bits + GIE); // Enter LPM3 w/interrupt
    } else if (requested_state == NIGHT) {
        WDTCTL = CLOCK_NIGHT;
        _BIS_SR(LPM0_bits + GIE); // Enter LPM3 w/interrupt
    }
}

int main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    // TODO: Set the time from Flash
    unix_time = 0;

    // Setup the timer
    CCR0 = 128-1;             // PWM Period
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

    set_state(NIGHT);
    
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

    if (state_should_be_in != state) set_state(state_should_be_in);

    if (state == NIGHT) {

        if (frac_second == 16-1) {
            direction = -1;
        } else if (frac_second == 0) {
            direction = 1;
        }

        frac_second += direction;

        CCR1 = prog[frac_second];

        if (frac_second == 0) {
            unix_time++;
        }

    } else if (state == DAY) {
        unix_time++;
    }
    IFG1&=~WDTIFG;
}