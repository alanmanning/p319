;;;
;;; LAB3-4 PROG1 ASM WITHOUT VOLATILE ON "count" VARIABLE
;;;
;;;
	
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
	MOV.B	#1, &P1OUT
.L3:
	MOV.W	#-5536, R12
.L2:
	ADD.W	#-1, R12
	CMP.W	#0, R12 { JNE	.L2
	XOR.B	#65, &P1OUT
	BR	#.L3
	.size	main, .-main
	.ident	"GCC: (SOMNIUM Technologies Limited - msp430-gcc 5.3.0.224) 5.3.0"
