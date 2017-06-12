/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

	size_t ret;
	void* clobber;
#ifdef __GNUC__
	asm volatile (
	R"(
	.intel_syntax noprefix

.macro HUFFMAN_ENCODE symtype, symsize

	# rdi = pDstBegin
	# rsi = pSrcBegin
	# rax = pSrcEnd
	# rdx = pEncodeTable

	mov			r8, rax
	mov			r15, rdi
	cmp			qword ptr [rdx], 0
	je			3f

	mov			bl, -64
	mov			cl, 0

	mov			rax, r8
	sub			rax, rsi
	and			rax, 3 * \symsize

	cmp			rax, 3 * \symsize
	jne			1f
	sub			rsi, 1 * \symsize
	jmp			91f

1:
	cmp			rax, 2 * \symsize
	jne			1f
	sub			rsi, 2 * \symsize
	jmp			92f

1:
	cmp			rax, 1 * \symsize
	jne			1f
	sub			rsi, 3 * \symsize
	jmp			93f

	.balign		64
1:
.irp offset, 0, 1, 2, 3
9\offset:
	shld		rax, rcx, cl
.if \offset == 0
	cmp			rsi, r8
	jnb			4f
.endif

	movzx		rcx, \symtype ptr [rsi+\offset * \symsize]
.if \offset == 3
	add			rsi, 4 * \symsize
.endif
	mov			rcx, qword ptr [rdx+rcx*8]
	add			bl, cl
.if \offset != 3
	jnc			2f
.else
	jnc			1b
.endif
	sub			cl, bl
	shld		rax, rcx, cl
	rol			rax, 32
	mov			qword ptr [rdi], rax
	add			rdi, 8
	add			cl, bl
	sub			bl, 64
.if \offset == 3
	jmp			1b
.endif
2:
.endr

4:
	test		bl, 0x3f
	jz			3f
	neg			bl
	mov			cl, bl
	shl			rax, cl
	rol			rax, 32
	mov			dword ptr [rdi], eax
	add			rdi, 4
	cmp			bl, 32
	jae			3f
	rol			rax, 32
	mov			dword ptr [rdi], eax
	add			rdi, 4
3:
	mov			rax, rdi
	sub			rax, r15
.endm
	HUFFMAN_ENCODE )" ARGS R"(
.purgem HUFFMAN_ENCODE
	)"
		: "=a"(ret), "=D"(clobber), "=S"(clobber), "=d"(clobber)
		: "D"(pDstBegin), "S"(pSrcBegin), "a"(pSrcEnd), "d"(pEncodeTable)
		: "rbx", "rcx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15");
#endif
	return ret;

#undef ARGS
