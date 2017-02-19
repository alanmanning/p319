# 1 "main.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "main.c"
# 16 "main.c"
# 1 "/home/alan/ti/msp430_gcc/include/msp430.h" 1
# 1423 "/home/alan/ti/msp430_gcc/include/msp430.h"
# 1 "/home/alan/ti/msp430_gcc/include/msp430g2553.h" 1
# 65 "/home/alan/ti/msp430_gcc/include/msp430g2553.h"
# 1 "/home/alan/ti/msp430_gcc/include/iomacros.h" 1
# 66 "/home/alan/ti/msp430_gcc/include/msp430g2553.h" 2
# 132 "/home/alan/ti/msp430_gcc/include/msp430g2553.h"
# 1 "/home/alan/ti/msp430_gcc/include/in430.h" 1
# 46 "/home/alan/ti/msp430_gcc/include/in430.h"
typedef unsigned int __istate_t;
# 133 "/home/alan/ti/msp430_gcc/include/msp430g2553.h" 2
# 142 "/home/alan/ti/msp430_gcc/include/msp430g2553.h"
extern volatile unsigned char IE1;





extern volatile unsigned char IFG1;






extern volatile unsigned char IE2;






extern volatile unsigned char IFG2;
# 174 "/home/alan/ti/msp430_gcc/include/msp430g2553.h"
extern volatile unsigned char ADC10DTC0;
extern volatile unsigned char ADC10DTC1;
extern volatile unsigned char ADC10AE0;

extern volatile unsigned int ADC10CTL0;
extern volatile unsigned int ADC10CTL1;
extern volatile unsigned int ADC10MEM;
extern volatile unsigned int ADC10SA;
# 285 "/home/alan/ti/msp430_gcc/include/msp430g2553.h"
extern volatile unsigned char DCOCTL;
extern volatile unsigned char BCSCTL1;
extern volatile unsigned char BCSCTL2;
extern volatile unsigned char BCSCTL3;
# 365 "/home/alan/ti/msp430_gcc/include/msp430g2553.h"
extern volatile unsigned char CACTL1;
extern volatile unsigned char CACTL2;
extern volatile unsigned char CAPD;
# 406 "/home/alan/ti/msp430_gcc/include/msp430g2553.h"
extern volatile unsigned int FCTL1;
extern volatile unsigned int FCTL2;
extern volatile unsigned int FCTL3;
# 460 "/home/alan/ti/msp430_gcc/include/msp430g2553.h"
extern volatile unsigned char P1IN;
extern volatile unsigned char P1OUT;
extern volatile unsigned char P1DIR;
extern volatile unsigned char P1IFG;
extern volatile unsigned char P1IES;
extern volatile unsigned char P1IE;
extern volatile unsigned char P1SEL;
extern volatile unsigned char P1SEL2;
extern volatile unsigned char P1REN;

extern volatile unsigned char P2IN;
extern volatile unsigned char P2OUT;
extern volatile unsigned char P2DIR;
extern volatile unsigned char P2IFG;
extern volatile unsigned char P2IES;
extern volatile unsigned char P2IE;
extern volatile unsigned char P2SEL;
extern volatile unsigned char P2SEL2;
extern volatile unsigned char P2REN;
# 488 "/home/alan/ti/msp430_gcc/include/msp430g2553.h"
extern volatile unsigned char P3IN;
extern volatile unsigned char P3OUT;
extern volatile unsigned char P3DIR;
extern volatile unsigned char P3SEL;
extern volatile unsigned char P3SEL2;
extern volatile unsigned char P3REN;






extern volatile unsigned int TA0IV;
extern volatile unsigned int TA0CTL;
extern volatile unsigned int TA0CCTL0;
extern volatile unsigned int TA0CCTL1;
extern volatile unsigned int TA0CCTL2;
extern volatile unsigned int TA0R;
extern volatile unsigned int TA0CCR0;
extern volatile unsigned int TA0CCR1;
extern volatile unsigned int TA0CCR2;
# 613 "/home/alan/ti/msp430_gcc/include/msp430g2553.h"
extern volatile unsigned int TA1IV;
extern volatile unsigned int TA1CTL;
extern volatile unsigned int TA1CCTL0;
extern volatile unsigned int TA1CCTL1;
extern volatile unsigned int TA1CCTL2;
extern volatile unsigned int TA1R;
extern volatile unsigned int TA1CCR0;
extern volatile unsigned int TA1CCR1;
extern volatile unsigned int TA1CCR2;
# 638 "/home/alan/ti/msp430_gcc/include/msp430g2553.h"
extern volatile unsigned char UCA0CTL0;
extern volatile unsigned char UCA0CTL1;
extern volatile unsigned char UCA0BR0;
extern volatile unsigned char UCA0BR1;
extern volatile unsigned char UCA0MCTL;
extern volatile unsigned char UCA0STAT;
extern volatile unsigned char UCA0RXBUF;
extern volatile unsigned char UCA0TXBUF;
extern volatile unsigned char UCA0ABCTL;
extern volatile unsigned char UCA0IRTCTL;
extern volatile unsigned char UCA0IRRCTL;



extern volatile unsigned char UCB0CTL0;
extern volatile unsigned char UCB0CTL1;
extern volatile unsigned char UCB0BR0;
extern volatile unsigned char UCB0BR1;
extern volatile unsigned char UCB0I2CIE;
extern volatile unsigned char UCB0STAT;
extern volatile unsigned char UCB0RXBUF;
extern volatile unsigned char UCB0TXBUF;
extern volatile unsigned int UCB0I2COA;
extern volatile unsigned int UCB0I2CSA;
# 833 "/home/alan/ti/msp430_gcc/include/msp430g2553.h"
extern volatile unsigned int WDTCTL;
# 880 "/home/alan/ti/msp430_gcc/include/msp430g2553.h"
extern volatile unsigned char CALDCO_16MHZ;
extern volatile unsigned char CALBC1_16MHZ;
extern volatile unsigned char CALDCO_12MHZ;
extern volatile unsigned char CALBC1_12MHZ;
extern volatile unsigned char CALDCO_8MHZ;
extern volatile unsigned char CALBC1_8MHZ;
extern volatile unsigned char CALDCO_1MHZ;
extern volatile unsigned char CALBC1_1MHZ;
# 901 "/home/alan/ti/msp430_gcc/include/msp430g2553.h"
extern volatile unsigned int TLV_CHECKSUM;
extern volatile unsigned char TLV_DCO_30_TAG;
extern volatile unsigned char TLV_DCO_30_LEN;
extern volatile unsigned char TLV_ADC10_1_TAG;
extern volatile unsigned char TLV_ADC10_1_LEN;
# 1424 "/home/alan/ti/msp430_gcc/include/msp430.h" 2
# 17 "main.c" 2

void main(void) {
 volatile unsigned int count;

 WDTCTL = (0x5A00) + (0x0080);
 P1DIR = 0x41;
 P1OUT = 0x01;

 while (1){
  count = 60000;
  while(count != 0) {
   count--;
  }
  P1OUT = P1OUT ^ 0x41;
 }
}
