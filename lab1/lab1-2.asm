#include "msp430g2553.inc"
org 0x0C000
RESET:
    mov.w #0x400, sp
    mov.w #WDTPW|WDTHOLD,&WDTCTL
    mov.b #11110111b, &P1DIR
    mov.b #01001001b, &P1OUT
    mov.b #00001000b, &P1REN
    mov.b #00001000b, &P1IE
    mov.w #8, R7 ;want to start with both lights off
    mov.b R7, &P1OUT
    mov.w #0, R6   ;use this register as the 2-bit counter
EINT
    bis.w #CPUOFF,SR
PUSH:
    inc R6 
    mov.w R6, R7 ;set R7 bit 0
    rla.b R6 ;bit1->bit2
    rla.b R6 ;bit2->bit3
    rla.b R6 ;bit3->bit4
    rla.b R6 ;bit4->bit5
    rla.b R6 ;bit5->bit6
    xor.w R6, R7 ;set R7 bit 6
    rra.w R6 ;bit6->bit5
    rra.w R6 ;bit5->bit4
    rra.w R6 ;bit4->bit3
    rra.w R6 ;bit3->bit2
    rra.w R6 ;bit2->bit1
    rra.w R6 ;bit1->bit0
    bic.w #1111111111111100b, R6
    bis.w #0000000000001000b, R7 ;make sure p1.3 is high
    bic.w #0000000001001001b, R7 ;set everything else to 0
    mov.b R7,&P1OUT
    bic.b #00001000b, &P1IFG ;clear interrupt flag
    reti

org 0xffe4
    dw PUSH
org 0xfffe
    dw RESET
