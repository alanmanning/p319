	.file	"main.c"
.text
	.balign 2
	.global	main
	.type	main, @function
main:
; start of function
; framesize_regs:     0
; framesize_locals:   2
; framesize_outgoing: 0
; framesize:          2
; elim ap -> fp       2
; elim fp -> sp       2
; saved regs:(none)
	; start of prologue
	SUB.W	#2, R1
	; end of prologue
	MOV.W	#23168, &WDTCTL
	MOV.B	#65, &P1DIR
	MOV.B	#1, &P1OUT
.L4:
	MOV.W	#-5536, @R1
.L2:
	MOV.W	@R1, R12
	CMP.W	#0, R12 { JEQ	.L6
	ADD.W	#-1, @R1
	BR	#.L2
.L6:
	XOR.B	#65, &P1OUT
	BR	#.L4
	.size	main, .-main
	.ident	"GCC: (SOMNIUM Technologies Limited - msp430-gcc 5.3.0.224) 5.3.0"
