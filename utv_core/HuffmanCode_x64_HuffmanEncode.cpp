/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

	typedef typename std::remove_reference<decltype(*pEncodeTable)>::type encodetable_t;
	size_t ret;
	void* clobber;
#ifdef __GNUC__
	asm volatile (
	R"(
	.intel_syntax noprefix

.macro HUFFMAN_ENCODE symtype, symsize, ursymtype, ursymsize

	# rdi = pDstBegin
	# rsi = pSrcBegin
	# rax = pSrcEnd
	# rdx = pEncodeTable

	mov			r8, rax
	sub			r8, \ursymsize * 6 * 2
	mov			r9, rdi
	cmp			qword ptr [rdx], 0
	je			3f

	mov			bl, -64
	xor			ecx, ecx

	movzx		r10, \ursymtype ptr [rsi]
	mov			r10, qword ptr [rdx+r10*8 + %c[offsetof_dwtablemuxur]]
	movzx		r11, \ursymtype ptr [rsi + \ursymsize]
	mov			r11, qword ptr [rdx+r11*8 + %c[offsetof_dwtablemuxur]]
	movzx		r12, \ursymtype ptr [rsi + \ursymsize * 2]
	mov			r12, qword ptr [rdx+r12*8 + %c[offsetof_dwtablemuxur]]
	movzx		r13, \ursymtype ptr [rsi + \ursymsize * 3]
	mov			r13, qword ptr [rdx+r13*8 + %c[offsetof_dwtablemuxur]]
	movzx		r14, \ursymtype ptr [rsi + \ursymsize * 4]
	mov			r14, qword ptr [rdx+r14*8 + %c[offsetof_dwtablemuxur]]
	xor			r15, r15

	.balign		64
1:
	shld		rax, r15, cl
	movzx		r15, \ursymtype ptr [rsi + \ursymsize * 5]
	mov			r15, qword ptr [rdx+r15*8 + %c[offsetof_dwtablemuxur]]
.irp offset, 0, 1, 2, 3, 4, 5
9\offset:
.if \offset == 0
	cmp			rsi, r8
	jnb			4f
.endif

	mov			rcx, r1\offset
.if \offset == 5
	add			rsi, 6 * \ursymsize
.endif
	add			bl, cl
.if \offset != 5
	jnc			2f
.else
	jnc			1b
.endif
	sub			cl, bl
	shld		rax, r1\offset, cl
	mov			rcx, r1\offset
	rol			rax, 32
	mov			qword ptr [rdi], rax
	add			rdi, 8
	sub			bl, 64
.if \offset == 5
	jmp			1b
.else
2:
	shld		rax, r1\offset, cl
	movzx		r1\offset, \ursymtype ptr [rsi + \ursymsize * (\offset + 6)]
	mov			r1\offset, qword ptr [rdx + r1\offset * 8 + %c[offsetof_dwtablemuxur]]
.endif
.endr

4:
	add			r8, \ursymsize * 6 * 2
	xor			ecx, ecx
1:
	shld		rax, rcx, cl
	cmp			rsi, r8
	jnb			4f
	movzx		rcx, \symtype ptr [rsi]
	add			rsi, \symsize
	mov			rcx, qword ptr [rdx+rcx*8 + %c[offsetof_dwtablemux]]
	add			bl, cl
	jnc			1b
	sub			cl, bl
	shld		rax, rcx, cl
	rol			rax, 32
	mov			qword ptr [rdi], rax
	add			rdi, 8
	add			cl, bl
	sub			bl, 64
	jmp			1b

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
	sub			rax, r9
.endm
	HUFFMAN_ENCODE )" ARGS R"(
.purgem HUFFMAN_ENCODE
	)"
		: "=a"(ret), "=D"(clobber), "=S"(clobber), "=d"(clobber)
		: "D"(pDstBegin), "S"(pSrcBegin), "a"(pSrcEnd), "d"(pEncodeTable),
		  [offsetof_dwtablemux]"i"(offsetof(encodetable_t, dwTableMux)),
		  [offsetof_dwtablemuxur]"i"(offsetof(encodetable_t, dwTableMuxUnrolled))
		: "rbx", "rcx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15");
#endif
	return ret;

#undef ARGS
