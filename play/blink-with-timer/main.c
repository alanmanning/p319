/*
 * Servo program: used to control the servo by a potentiometer.
 * I wrote this program to re-teach myself the controls of the timer
 * and of the ADC.
 */

#include <msp430.h>

void main(void) {
	volatile unsigned int count;	//You must declare your variables in C
  // notice the label volatile. What happens if you remove this label?
	WDTCTL = WDTPW + WDTHOLD;     //Stop WDT
	P1DIR = 0x41;			//Set P1 output direction
	P1OUT = 0x01;			//Set the output
	
	while (1){				//Loop forever
		count = 60000;
		while(count != 0) {
			count--;		//decrement
		}
		P1OUT = P1OUT ^ 0x41;	//bitwise xor the output with 0x41
	}
}
