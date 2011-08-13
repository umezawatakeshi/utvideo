; ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe
; $Id$

STD_PROLOG	macro	_numlocal
numlocal = &_numlocal
argsoffset = numlocal + 16 * 10 + 8 * 8 + 8 + 8
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
	movdqa		[rsp -  16], xmm6
	movdqa		[rsp -  32], xmm7
	movdqa		[rsp -  48], xmm8
	movdqa		[rsp -  64], xmm9
	movdqa		[rsp -  80], xmm10
	movdqa		[rsp -  96], xmm11
	movdqa		[rsp - 112], xmm12
	movdqa		[rsp - 128], xmm13
	movdqa		[rsp - 144], xmm14
	movdqa		[rsp - 160], xmm15
	sub			rsp, numlocal + 16 * 10
endm

STD_EPILOG macro
	add			rsp, numlocal + 16 * 10
	movdqa		xmm15, [rsp - 160]
	movdqa		xmm14, [rsp - 144]
	movdqa		xmm13, [rsp - 128]
	movdqa		xmm12, [rsp - 112]
	movdqa		xmm11, [rsp -  96]
	movdqa		xmm10, [rsp -  80]
	movdqa		xmm9,  [rsp -  64]
	movdqa		xmm8,  [rsp -  48]
	movdqa		xmm7,  [rsp -  32]
	movdqa		xmm6,  [rsp -  16]
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
