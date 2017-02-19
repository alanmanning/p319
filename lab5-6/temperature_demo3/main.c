/*
 * main.c
 *
 * MSP-EXP430G2-LaunchPad User Experience Application
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *

 Heavily modified: Nov, 2013 Carl Michal

*/


/******************************************************************************
 *                  MSP-EXP430G2-LaunchPad User Experience Application
 *
 * 1. Device starts up in LPM3 + blinking LED to indicate device is alive    
 *    + Upon first button press, device transitions to application mode
 * 2. Application Mode
 *    + Continuously sample ADC Temp Sensor channel
 *       
 *    + Transmit temperature value via TimerA UART to PC  
 * 
 *
 * Texas Instruments, Inc.
 ******************************************************************************/
  
#include  "msp430.h"

#define     LED1                  BIT0
#define     LED2                  BIT6

#define     BUTTON                BIT3

#define     TXD                   BIT1                      // TXD on P1.1
#define     RXD                   BIT2                      // RXD on P1.2

#define     PreAppMode            0
#define     RunningMode           1
//   Conditions for 9600 Baud SW UART, SMCLK = 1MHz

#define     Bitime                13
// duration of 1 bit, measured in timer A clock cycles.
// the main clock runs at 1MHz. TimerA runs at 1MHz/8. 
//For 2400 bits per second, we want 52 timer ticks.
// For 9600 baud, 13 timer ticks
volatile unsigned char BitCnt;
volatile int TXByte;  // don't make this unsigned:
/* If it signed, then the TXByte = TXByte >> 1; is done as RRA
   but if it is unsigned, then it becomes a library call ! */
volatile unsigned int Mode;   
  
void ConfigureTimerUart(void);
void InitializeButton(void);
void PreApplicationMode(void); 
void Transmit(void);

void main(void)
{
  long tempMeasured;

  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  /* next three lines to use internal calibrated 1MHz clock: */
  BCSCTL1 = CALBC1_1MHZ;                    // Set range
  DCOCTL = CALDCO_1MHZ;
  BCSCTL2 &= ~(DIVS_3);                     // SMCLK = DCO = 1MHz  

  InitializeButton();

  // setup port for leds:
  P1DIR |= LED1 + LED2;                          
  P1OUT &= ~(LED1 + LED2);  

  P1DIR |= TXD;
  P1OUT |= TXD;
  
  Mode = PreAppMode;
  PreApplicationMode();          // Blinks LEDs, waits for button press
  
  /* Configure ADC Temp Sensor Channel */
  ADC10CTL1 = INCH_10 + ADC10DIV_3;         // Temp Sensor ADC10CLK/4
  ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE;

  __delay_cycles(1000);                     // Wait for ADC Ref to settle  
  __enable_interrupt();                     // Enable interrupts.
  
  /* Main Application Loop */
  while(1)
  {    
    ADC10CTL0 |= ENC + ADC10SC;        // Sampling and conversion start
    __bis_SR_register(CPUOFF + GIE);   // LPM0 with interrupts enabled  turns cpu off.
    // an interrupt is triggered when the ADC result is ready. 
    // The interrupt handler restarts the cpu.
    
    // store result 
    tempMeasured = ADC10MEM;   

    // set up timer and port for uart.
    ConfigureTimerUart();

    // convert to farenheit and send to host computer
    TXByte = (unsigned char)( ((tempMeasured - 630) * 761) / 1024 );  
    Transmit(); 

    P1OUT ^= LED1;  // toggle the light every time we make a measurement.
        
    // set up timer to wake us in a while:
    TACCR0 = 2400;                             //  period
    TACTL = TASSEL_1 | MC_1;                  // TACLK = ACLK, Up mode.  
    TACCR1 = 2400; // interrupt at end
    TACCTL1 = CCIE;                // TACCTL0 

    // go to sleep, wait till timer expires to do another measurement.
    __bis_SR_register(LPM3_bits + GIE);  // LPM0 with interrupts enabled  turns cpu off.

    // could have just done this - but low power mode is nicer.
    //  __delay_cycles(500000);  

  }  
}

void PreApplicationMode(void)
{    
  P1DIR |= LED1 + LED2;
  P1OUT |= LED1;                 // To enable the LED toggling effect
  P1OUT &= ~LED2;
    
  /* these next two lines configure the ACLK signal to come from 
     a secondary oscillator source, called VLO */

  BCSCTL1 |= DIVA_1;             // ACLK is half the speed of the source (VLO)
  BCSCTL3 |= LFXT1S_2;           // ACLK = VLO
  
  /* here we're setting up a timer to fire an interrupt periodically. 
     When the timer 1 hits its limit, the interrupt will toggle the lights 

     We're using ACLK as the timer source, since it lets us go into LPM3
     (where SMCLK and MCLK are turned off). */

  TACCR0 = 1200;                 //  period
  TACTL = TASSEL_1 | MC_1;       // TACLK = ACLK, Up mode.  
  TACCTL1 = CCIE + OUTMOD_3;     // TACCTL1 Capture Compare
  TACCR1 = 600;                  // duty cycle
  __bis_SR_register(LPM3_bits + GIE);   // LPM3 with interrupts enabled
  // in LPM3, MCLCK and SMCLK are off, but ACLK is on.
}

void ConfigureTimerUart(){
  TACCTL0 = OUT;                 // TXD Idle as Mark
  TACTL = TASSEL_2 + MC_2 + ID_3;// set SMCLK as source, divide by 8, continuous mode
  P1SEL |= (TXD+RXD);
  P1DIR |= TXD;                               
}

/* using the serial port requires Transmit(), 
   the TIMERA0_VECTOR, ConfigureTimerUart() 
   and variables Bitcnt, TXbyte, Bitime */

// Function Transmits Character from TXByte 
void Transmit()
{ 

  BitCnt = 0xA;           // Load Bit counter, 8 data + Start/Stop bit
  TXByte |= 0x100;        // Add mark stop bit to TXByte
  TXByte = TXByte << 1;   // Add space start bit

  /*
  // The TI folks originally had the four lines of code below, but why?
  // replace with the single line after the comment ends

  //  Simulate a timer capture event to obtain the value of TAR into 
  //   the TACCR0 register.

  TACCTL0 = CM_1 + CCIS_2  + SCS + CAP + OUTMOD0;  //capture on rising edge, 
  // initially set to GND as input 
  TACCTL0 |= CCIS_3;	//change input to Vcc, rising the edge,
  // triggering the capture action
  while (!(TACCTL0 & CCIFG));	//wait till the interrupt happens.
  TACCR0 += Bitime;                        // Time till first bit
  */

  TACCR0 = TAR+ Bitime;
  TACCTL0 =  CCIS0 + OUTMOD0 + CCIE;   // TXD = mark = idle, enable interrupts
  // OUTMOD0 sets output mode 1: SET which will
  // have the CCR bit (our TX bit) to go high when the timer expires
  while ( TACCTL0 & CCIE );                   // Wait for TX completion
}



// this gets used in pre-application mode only to toggle the lights:
#if defined(__TI_COMPILER_VERSION__)
#pragma vector=TIMER0_A1_VECTOR
__interrupt void ta1_isr (void)
#else
  void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) ta1_isr (void)
#endif
{
  TACCTL1 &= ~CCIFG; // reset the interrupt flag
  if (Mode == PreAppMode){
    P1OUT ^= (LED1 + LED2); // toggle the two lights.
  }
  else{
    TACCTL1 = 0;                // no more interrupts.
    __bic_SR_register_on_exit(LPM3_bits);        // Restart the cpu
  }
    
}

void InitializeButton(void)                 // Configure Push Button 
{
  P1DIR &= ~BUTTON;
  P1OUT |= BUTTON;
  P1REN |= BUTTON;
  P1IES |= BUTTON;
  P1IFG &= ~BUTTON;
  P1IE |= BUTTON;
}

/* *************************************************************
 * Port Interrupt for Button Press 
 * 1. During standby mode: to enter application mode
 *
 * *********************************************************** */


#if defined(__TI_COMPILER_VERSION__)
#pragma vector=PORT1_VECTOR
__interrupt void port1_isr(void)
#else
  void __attribute__ ((interrupt(PORT1_VECTOR))) port1_isr (void)
#endif

{   
  
  /* this disables port1 interrupts for a little while so that
  we don't try to respond to two consecutive button pushes right together.
  The watchdog timer interrupt will re-enable port1 interrupts 

  This whole watchdog thing is completely unnecessary here, but its useful 
  to see how it is done.
*/
  P1IFG = 0;  // clear out interrupt flag
  P1IE &= ~BUTTON;         // Disable port 1 interrupts 
  WDTCTL = WDT_ADLY_250;   // set up watchdog timer duration 
  IFG1 &= ~WDTIFG;         // clear interrupt flag 
  IE1 |= WDTIE;            // enable watchdog interrupts
    
  TACCTL1 = 0;             // turn off timer 1 interrupts
  P1OUT &= ~(LED1+LED2);   // turn off the leds
  Mode = RunningMode;
  __bic_SR_register_on_exit(LPM3_bits); // take us out of low power mode
  
}

// WDT Interrupt Service Routine used to de-bounce button press

#if defined(__TI_COMPILER_VERSION__)
#pragma vector=WDT_VECTOR
__interrupt void wdt_isr(void)
#else
  void __attribute__ ((interrupt(WDT_VECTOR))) wdt_isr (void)
#endif

{
    IE1 &= ~WDTIE;                   /* disable watchdog interrupt */
    IFG1 &= ~WDTIFG;                 /* clear interrupt flag */
    WDTCTL = WDTPW + WDTHOLD;        /* put WDT back in hold state */
    P1IE |= BUTTON;             /* Debouncing complete - reenable port 1 interrupts*/
}

// ADC10 interrupt service routine
#if defined(__TI_COMPILER_VERSION__)
#pragma vector=ADC10_VECTOR
__interrupt void adc10_isr(void)
#else
  void __attribute__ ((interrupt(ADC10_VECTOR))) adc10_isr (void)
#endif
{
  __bic_SR_register_on_exit(CPUOFF);        // Restart the cpu
}


// Timer A0 interrupt service routine - 
#if defined(__TI_COMPILER_VERSION__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#else
  void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#endif
{
  
  TACCR0 += Bitime;                         // Add Offset to TACCR0
  
  if ( BitCnt == 0){
    P1SEL &= ~(TXD+RXD);
    TACCTL0 &= ~ CCIE ;                   // All bits TXed, disable interrupt
  }  
  else{
    // in here we set up what the next bit will be: when the timer expires
    // next time.
    // In TimerConfigUart, we set OUTMOD0 for output mode 1 (set).
    // Adding OUTMOD2 gives output mode 5 (reset).

    // The advantage to doing this is that the bits get set in hardware 
    // when the timer expires so the timing is as accurate as possible.
    
    TACCTL0 |=  OUTMOD2;                  // puts output unit in 'set' mode
    if (TXByte & 0x01)
      TACCTL0 &= ~ OUTMOD2;               // puts output unit in reset mode
    TXByte = TXByte >> 1; // shift down so the next bit is in place.
    BitCnt --; 
  }
  
}
