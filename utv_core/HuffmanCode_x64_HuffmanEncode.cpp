/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

	typedef typename std::remove_reference<decltype(*pEncodeTable)>::type encodetable_t;
	size_t ret;
	void* clobber;
#if defined(__GNUC__) || defined(__clang__)
	asm volatile (
	R"(
	.intel_syntax noprefix

.macro HUFFMAN_ENCODE symtype, symsize, msymtype, msymsize

	# rdi = pDstBegin
	# rsi = pSrcBegin
	# rax = pSrcEnd
	# rdx = pEncodeTable

.equ regfirst, 8
.equ unroll_stride, 8
	push		rbp
	push		rdi
	mov			r8, rax
	sub			r8, \msymsize * unroll_stride * 2
	push		r8
	cmp			qword ptr [rdx], 0
	je			3f

	mov			bl, -64
	xor			eax, eax

.irp regidx, 8, 9, 10, 11, 12, 13, 14, 15
.if (\regidx - regfirst) != unroll_stride - 1
	movzx		r\regidx, \msymtype ptr [rsi + \msymsize * (\regidx - regfirst)]
	mov			r\regidx, qword ptr [rdx+r\regidx*8 + %c[offsetof_dwtablemuxmulti]]
.endif
.endr

	.balign		64
1:
	movzx		r15, \msymtype ptr [rsi + \msymsize * (unroll_stride - 1)]
	mov			r15, qword ptr [rdx+r15*8 + %c[offsetof_dwtablemuxmulti]]
.irp regidx, 8, 9, 10, 11, 12, 13, 14, 15
.if (\regidx - regfirst) == 0
	cmp			rsi, qword ptr [rsp]
	jnb			4f
.endif

.if (\regidx - regfirst) == unroll_stride - 1
	add			rsi, unroll_stride * \msymsize
.endif
	mov			rbp, r\regidx
	and			rbp, -256
	shrx		rcx, rbp, rbx
	or			rax, rcx
	add			bl, r\regidx\(\())b
.if (\regidx - regfirst) != unroll_stride - 1
	jnc			2f
.else
	jnc			1b
.endif

	rorx		rax, rax, 32
	mov			qword ptr [rdi], rax
	add			rdi, 8
	sub			r\regidx\(\())b, bl
	shlx		rax, rbp, r\regidx
	sub			bl, 64
.if (\regidx - regfirst) == unroll_stride - 1
	jmp			1b
.else
2:
	movzx		r\regidx, \msymtype ptr [rsi + \msymsize * ((\regidx - regfirst) + unroll_stride)]
	mov			r\regidx, qword ptr [rdx + r\regidx * 8 + %c[offsetof_dwtablemuxmulti]]
.endif
.endr

4:
	add			qword ptr [rsp], \msymsize * unroll_stride * 2
1:
	cmp			rsi, qword ptr [rsp]
	jnb			4f
	movzx		r8, \symtype ptr [rsi]
	add			rsi, \symsize
	mov			r8, qword ptr [rdx+r8*8 + %c[offsetof_dwtablemux]]
	mov			rbp, r8
	and			rbp, -256
	shrx		rcx, rbp, rbx
	or			rax, rcx
	add			bl, r8b
	jnc			1b

	rorx		rax, rax, 32
	mov			qword ptr [rdi], rax
	add			rdi, 8
	sub			r8b, bl
	shlx		rax, rbp, r8
	sub			bl, 64
	jmp			1b

4:
	test		bl, 0x3f
	jz			3f
	rorx		rax, rax, 32
	mov			dword ptr [rdi], eax
	add			rdi, 4
	cmp			bl, -32
	jbe			3f
	shr			rax, 32
	mov			dword ptr [rdi], eax
	add			rdi, 4
3:
	pop			rax
	pop			rbx
	pop			rbp
	mov			rax, rdi
	sub			rax, rbx
.endm
	HUFFMAN_ENCODE )" ARGS R"(
.purgem HUFFMAN_ENCODE
	)"
		: "=a"(ret), "=D"(clobber), "=S"(clobber), "=d"(clobber)
		: "D"(pDstBegin), "S"(pSrcBegin), "a"(pSrcEnd), "d"(pEncodeTable),
		  [offsetof_dwtablemux]"i"(offsetof(encodetable_t, dwTableMux)),
		  [offsetof_dwtablemuxmulti]"i"(offsetof(encodetable_t, dwTableMuxMulti))
		: "rbx", "rcx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15");
#endif
	return ret;

#undef ARGS
