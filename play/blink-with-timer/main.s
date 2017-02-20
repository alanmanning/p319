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
	MOV.W	#-1, &TA0CCR0
	MOV.W	#576, &TA0CTL
	ADD.W	#16, &TA0CTL
	; start of epilogue
	.refsym	__crt0_call_exit
	RET
	.size	main, .-main
	.ident	"GCC: (SOMNIUM Technologies Limited - msp430-gcc 6.2.1.16) 6.2.1 20161212"
