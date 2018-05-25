#include  "msp430.h"

#define     LED1                  BIT0
#define     LED2                  BIT6

#define     TXD                   BIT1                      // TXD on P1.1

//   Conditions for 9600 Baud SW UART, SMCLK = 1MHz
#define     Bitime                13
// duration of 1 bit, measured in timer A clock cycles.
// the main clock runs at 1MHz. TimerA runs at 1MHz/8. 
//For 2400 bits per second, we want 52 timer ticks.
// For 9600 baud, 13 timer ticks
volatile unsigned char BitCnt;

/* An int is 2 bytes long. Even though we are only going to transmit
 * 8 bits of data, need extra bits for start/stop and (optional) parity.
 * If it signed, then the TXByte = TXByte >> 1; is done as RRA
   but if it is unsigned, then it becomes a library call ! */
volatile int TXByte; //The thing we want to transmit
  
void ConfigureClocks(void);
void ResetTimerUart(void);
void Transmit(void);
void main(void)
{ 

  TXByte = 8; //The thing to transmit

  WDTCTL = WDTPW + WDTHOLD;   //Configure watchdog (stop it)
  ConfigureClocks();

  //Configure ports
  P1OUT |= TXD;
  
  __enable_interrupt(); //Enable interrupts
  
  /* Main Application Loop */
  while(1)
  {    
    // set up timer and port for uart.
    ResetTimerUart();
    Transmit(); 

   // // go to sleep, wait till timer expires to do another measurement.
   // __bis_SR_register(LPM3_bits + GIE);  // LPM0 with interrupts enabled  turns cpu off.

   // // could have just done this - but low power mode is nicer.
    __delay_cycles(500000);  
  }  
}

void ConfigureClocks(){
  /*BCSCTL1 = Basic Clock System CONTROL register 1
   * There are a number of settings in this register, the
   * one we are setting below is the range. There are 16 different
   * ranges, and for a target clock frequency (here 1 MHz), the
   * correct range has to be set. These ranges are calibrated in the
   * factory and the calibration values stored in memory. The calibration
   * value for 1 MHz at room temp is CALBC1_1MHz */
  BCSCTL1 = CALBC1_1MHZ;

  /* DCOCTL = Digitally Controlled Oscillator CONTROL.
   * We are setting this to 1 MHz. CALDCO_1MHZ is read
   * in from device memory and is factory-set (since the clocks
   * will vary slightly from device to device). */
  DCOCTL = CALDCO_1MHZ;

  /*BCSCTL1 = Basic Clock System CONTROL register 1
   * We are going to be using the Sub-Main Clock (SMCLK) to
   * control the timers. SMCLK can be used to control the peripherals, 
   * whereas Main Clock (MCLK) is used to control the CPU.
   * We'd like SMCLK rate = MCLK rate, so we should set the divisor
   * to 1. Do this by clearing the bits for this part of the register.*/
  BCSCTL2 &= ~(DIVS_3);  //Clear these bits, change to 00 -> divide by 1 -> SMCLK rate = MCLK rate = 1 MHz
}

void ResetTimerUart(){
  /*TACCTL0 = Timer A0 Capture/Compare CONTROL register
   * On power up, most of the register is set to default
   * values that work for our purposes. These are:
   * -Compare mode (no capture)
   * -Output mode: output the OUT value
   * -Compare/compare interrupt disabled
   * The only thing we need to change is the OUT value to high
   * (from default of low */
  TACCTL0 = OUT;
  TACTL = TASSEL_2 + MC_2 + ID_3;// set SMCLK as source, divide by 8, continuous mode

  /*P1SEL = Port 1 Select register. This is one of the registers
   * That are used to set what modules a pin is connected to. In this
   * case, we want to connect the pins to the timer module. Just flipping
   * the bit for P1.1 in the P1SEL and P1DIR registers does it. To figure out
   * what to do to connect a specific module to a pin, take a look in the
   * device-specific datasheet. There is a table for each pin that shows what
   * modules are available and how to connect them.*/
  P1SEL |= TXD;
  P1DIR |= TXD;
  /*P1.1 is now connected to TA0.0, or Timer_A Capture/Compare unit 0 output*/
}

/* UART data framing:
 * Resting state is high
 * Start bit is low
 * Usually 8 bits are transmitted, but can be configured for others
 * An optional parity bit is transmitted (error correction) NOT USED
 * Stop bit is high (also called the mark) 
 */
void Transmit()
{ 
  BitCnt = 0xA;           // Load Bit counter, 8 data + Start bit + Stop bit = 0xA bits in total
  TXByte |= 0x100;        // Add stop bit to TXByte (start bit is high)
  TXByte = TXByte << 1;   // Add start bit (start bit is low)

  /* TACCR0 is the value that is going to trigger the interrupt.
   * This is for Capture/Compare register 0
   * We want to take the current value
   * of the timer, and then add the amount of time we want to bit to transmit for. Note that if
   * this overflows, that's OK, because the timer register TAR will overflow as well. */
  TACCR0 = TAR + Bitime;   //TAR is the timer value, Bittime is configured for the transmission rate

  /*OUTMOD0 just ignores the time and sets the pin to the value of OUT. And since above
   * we've set OUT to be high, this just has the effect of setting the TXD pin to high.
   * Later on we will change the output mode. */
  TACCTL0 = OUTMOD0; 
  TACCTL0 |=  CCIE; //enable timer interrupts
  while ( TACCTL0 & CCIE );                   // Wait for TX completion
}


/*TODO: Questions
 * What is EQU in the datasheets? 
 * -> It's an internal signal.
 *
 * We set an interrupt using TACCR0.. but how doesn't the chip know to ignor the other interrupts?
 * -> We only enable interrupts for Timer A Capture/compare Register 0, not for the other ones.
 */


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
    P1SEL &= ~TXD;
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
