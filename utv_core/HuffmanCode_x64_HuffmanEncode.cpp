/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
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

.equ regfirst, 8
.if \useshld
.equ unroll_stride, 8
.else
.equ unroll_stride, 7
.endif

	push		rdi
	mov			r8, rax
	sub			r8, \ursymsize * unroll_stride * 2
	push		r8
	cmp			qword ptr [rdx], 0
	je			3f

	mov			bl, -64
	xor			ecx, ecx

.irp regidx, 8, 9, 10, 11, 12, 13, 14, 15
.if (\regidx - regfirst) == unroll_stride - 1
	xor			r\regidx, r\regidx
.elseif (\regidx - regfirst) < unroll_stride - 1
	movzx		r\regidx, \ursymtype ptr [rsi + \ursymsize * (\regidx - regfirst)]
	mov			r\regidx, qword ptr [rdx+r\regidx*8 + %c[offsetof_dwtablemuxur]]
.endif
.endr

	.balign		64
1:
.if \useshld
	shld		rax, r15, cl
	movzx		r15, \ursymtype ptr [rsi + \ursymsize * (unroll_stride - 1)]
	mov			r15, qword ptr [rdx+r15*8 + %c[offsetof_dwtablemuxur]]
.else
	shl			rax, cl
	neg			cl
	shr			r14, cl
	or			rax, r14
	movzx		r14, \ursymtype ptr [rsi + \ursymsize * (unroll_stride - 1)]
	mov			r14, qword ptr [rdx+r14*8 + %c[offsetof_dwtablemuxur]]
.endif
.irp regidx, 8, 9, 10, 11, 12, 13, 14, 15
.if (\regidx - regfirst) < unroll_stride
9\regidx:
.if (\regidx - regfirst) == 0
	cmp			rsi, qword ptr [rsp]
	jnb			4f
.endif

	mov			rcx, r\regidx
.if (\regidx - regfirst) == unroll_stride - 1
	add			rsi, unroll_stride * \ursymsize
.endif
	add			bl, cl
.if (\regidx - regfirst) != unroll_stride - 1
	jnc			2f
.else
	jnc			1b
.endif
	sub			cl, bl
.if \useshld
	shld		rax, r\regidx, cl
.else
	shl			rax, cl
	mov			r15, r\regidx
	neg			cl
	shr			r15, cl
	or			rax, r15
.endif

	mov			rcx, r\regidx
	rol			rax, 32
	mov			qword ptr [rdi], rax
	add			rdi, 8
	sub			bl, 64
.if (\regidx - regfirst) == unroll_stride - 1
	jmp			1b
.else
2:
.if \useshld
	shld		rax, r\regidx, cl
.else
	shl			rax, cl
	neg			cl
	shr			r\regidx, cl
	or			rax, r\regidx
.endif
	movzx		r\regidx, \ursymtype ptr [rsi + \ursymsize * ((\regidx - regfirst) + unroll_stride)]
	mov			r\regidx, qword ptr [rdx + r\regidx * 8 + %c[offsetof_dwtablemuxur]]
.endif
.endif
.endr

4:
	add			qword ptr [rsp], \ursymsize * unroll_stride * 2
	xor			ecx, ecx
1:
	shld		rax, rcx, cl
	cmp			rsi, qword ptr [rsp]
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
	pop			rax
	pop			rbx
	mov			rax, rdi
	sub			rax, rbx
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
