	.file	"main.c"
.text
	.balign 2
	.global	main
	.type	main, @function
main:
; start of function
; framesize_regs:     0
; framesize_locals:   0
; framesize_outgoing: 0
; framesize:          0
; elim ap -> fp       2
; elim fp -> sp       0
; saved regs:(none)
	; start of prologue
	; end of prologue
	MOV.W	#23168, &WDTCTL
	MOV.B	#65, &P1DIR
	MOV.B	#65, &P1OUT
	MOV.W	#512, &TA0CTL
	BIS.W	#64, &TA0CTL
	MOV.W	#16, &TA0CCTL0
	BIS.W	#32, &TA0CTL
 ; 26 "main.c" 1
	bis.w #8, SR
 ; 0 "" 2
	; start of epilogue
	.refsym	__crt0_call_exit
	RET
	.size	main, .-main
	.balign 2
	.global	PORT1_ISR
	.section	__interrupt_vector_10,"ax",@progbits
	.word	PORT1_ISR
	.text
	.type	PORT1_ISR, @function
PORT1_ISR:
; start of function
; attributes: interrupt 
; framesize_regs:     2
; framesize_locals:   0
; framesize_outgoing: 0
; framesize:          2
; elim ap -> fp       4
; elim fp -> sp       0
; saved regs: R12
	; start of prologue
	PUSH	R12
	; end of prologue
	MOV.W	&timerCount, R12
	ADD.W	#1, R12
	AND.B	#7, R12
	MOV.W	R12, &timerCount
	CMP.W	#0, R12 { JNE	.L2
	XOR.B	#65, &P1OUT
.L2:
	; start of epilogue
	POP	R12
	RETI
	.size	PORT1_ISR, .-PORT1_ISR
	.global	timerCount
.data
	.balign 2
	.type	timerCount, @object
	.size	timerCount, 2
timerCount:
	.zero	2
	.ident	"GCC: (SOMNIUM Technologies Limited - msp430-gcc 6.2.1.16) 6.2.1 20161212"
