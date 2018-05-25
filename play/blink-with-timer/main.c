/*
 * Blink the lights using the built-in timers instead of a loop.
 * I wrote this program to give some practice using the timers.
 */

//LEDs are on P1.6 and P1.0

#include <msp430.h>

unsigned int timerCount = 0;
void main(void) {
    unsigned int count_until = 65535; //maximum is 16 bits, or 65535

	WDTCTL = WDTPW + WDTHOLD;  //turn off the watchdog timer
    P1DIR = BIT0 + BIT6; //Set P1.0 and P1.6 to outputs
    P1OUT = BIT0 + BIT6;
   
//    TACCR0 = count_until;

    TACTL = TASSEL_2; //SMCLK
    TACTL |= ID_1; //Don't divide the clock input by an amount

    TACCTL0 = CCIE; //Enable interrupt on timer completion

    TACTL |= MC_2; //Start the timer
    _BIS_SR(GIE);
//    _BIS_SR (LPM4_bits + GIE);	//Turn on interrupts and go into the lowest
}

// Timer interrupt service routine
void __attribute__ ((interrupt(TIMER0_A0_VECTOR)))  PORT1_ISR(void)
{ 					//code goes here 
    timerCount = (timerCount + 1) % 8;
    if(timerCount == 0){
        P1OUT ^= (BIT0 + BIT6); //toggle the LEDs
    }
}
