; •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e
; $Id$

STD_PROLOG	macro	_numlocal, savexmm
numlocal = &_numlocal
argsoffset = 8 * 8 + 8 + 8
	sub			rsp, 8
	mov			qword ptr [rsp + 16 +  0], rcx
	mov			qword ptr [rsp + 16 +  8], rdx
	mov			qword ptr [rsp + 16 + 16], r8
	mov			qword ptr [rsp + 16 + 24], r9
	push		rbx
	push		rbp
	push		rsi
	push		rdi
	push		r12
	push		r13
	push		r14
	push		r15
if numlocal ne 0
	sub			rsp, numlocal
endif
endm

STD_EPILOG macro
if numlocal ne 0
	add			rsp, numlocal
endif
	pop			r15
	pop			r14
	pop			r13
	pop			r12
	pop			rdi
	pop			rsi
	pop			rbp
	pop			rbx
	add			rsp, 8
endm
