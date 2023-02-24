/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

	typedef typename std::remove_reference<decltype(*pEncodeTable)>::type encodetable_t;
	size_t ret;
	void* clobber;
#if defined(__GNUC__) || defined(__clang__)
	asm volatile (
	R"(
	.intel_syntax noprefix

.macro HUFFMAN_ENCODE symtype, symsize, ursymtype, ursymsize, useshld

	# rdi = pDstBegin
	# rsi = pSrcBegin
	# rax = pSrcEnd
	# rdx = pEncodeTable

.if \useshld
.equ unroll_stride, 6
.else
.equ unroll_stride, 5
.endif

	mov			r8, rax
	sub			r8, \ursymsize * unroll_stride * 2
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
.if \useshld
	movzx		r14, \ursymtype ptr [rsi + \ursymsize * 4]
	mov			r14, qword ptr [rdx+r14*8 + %c[offsetof_dwtablemuxur]]
	xor			r15, r15
.else
	xor			r14, r14
.endif

	.balign		64
1:
.if \useshld
	shld		rax, r15, cl
	movzx		r15, \ursymtype ptr [rsi + \ursymsize * 5]
	mov			r15, qword ptr [rdx+r15*8 + %c[offsetof_dwtablemuxur]]
.else
	shl			rax, cl
	neg			cl
	shr			r14, cl
	or			rax, r14
	movzx		r14, \ursymtype ptr [rsi + \ursymsize * 4]
	mov			r14, qword ptr [rdx+r14*8 + %c[offsetof_dwtablemuxur]]
.endif
.irp offset, 0, 1, 2, 3, 4, 5
.if \offset < unroll_stride
9\offset:
.if \offset == 0
	cmp			rsi, r8
	jnb			4f
.endif

	mov			rcx, r1\offset
.if \offset == unroll_stride - 1
	add			rsi, unroll_stride * \ursymsize
.endif
	add			bl, cl
.if \offset != unroll_stride - 1
	jnc			2f
.else
	jnc			1b
.endif
	sub			cl, bl
.if \useshld
	shld		rax, r1\offset, cl
.else
	shl			rax, cl
	mov			r15, r1\offset
	neg			cl
	shr			r15, cl
	or			rax, r15
.endif

	mov			rcx, r1\offset
	rol			rax, 32
	mov			qword ptr [rdi], rax
	add			rdi, 8
	sub			bl, 64
.if \offset == unroll_stride - 1
	jmp			1b
.else
2:
.if \useshld
	shld		rax, r1\offset, cl
.else
	shl			rax, cl
	neg			cl
	shr			r1\offset, cl
	or			rax, r1\offset
.endif
	movzx		r1\offset, \ursymtype ptr [rsi + \ursymsize * (\offset + unroll_stride)]
	mov			r1\offset, qword ptr [rdx + r1\offset * 8 + %c[offsetof_dwtablemuxur]]
.endif
.endif
.endr

4:
	add			r8, \ursymsize * unroll_stride * 2
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
