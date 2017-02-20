/*
 * Blink the lights using the built-in timers instead of a loop.
 * I wrote this program to give some practice using the timers.
 */

//LEDs are on P1.6 and P1.0

#include <msp430.h>

void main(void) {
    unsigned int count_until = 65535; //maximum is 16 bits, or 65535

	WDTCTL = WDTPW + WDTHOLD;  //turn off the watchdog timer
    P1DIR = BIT0 + BIT6; //Set P1.0 and P1.6 to outputs
    P1OUT = BIT0 + BIT6;
   
    TACCR0 = count_until;

    TACTL = TASSEL_2 + ID_1; //SMCLK
    TACTL += MC_1; //Start the timer
}

// Timer interrupt service routine
void __attribute__ ((interrupt(PORT1_VECTOR)))  PORT1_ISR(void)
{ 					//code goes here 
  P1OUT ^= 0x41;           // toggle the LEDS
  P1IFG &= ~0x08;          // Clear P1.3 IFG. If you don't, it just happens again.
}
