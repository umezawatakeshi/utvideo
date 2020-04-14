/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

	typedef typename std::remove_reference<decltype(*pDecodeTable)>::type decodetable_t;
	typedef typename std::remove_reference<decltype(*pDstBegin)>::type sym_t;
	sym_t *ret;
	void* clobber;
#if defined(__GNUC__) || defined(__clang__)
	asm volatile (
	R"(
	.intel_syntax noprefix

.macro HUFFMAN_DECODE_8

	# edi = pDstBegin
	# eax = pDstEnd
	# esi = pSrcBegin
	# ebx = pDecodeTable

	push		ebx
	push		eax

	mov			cl, 32
	sub			esi, 4

.irp persym, 0, 1
	sub			ebx, %c[sizeof_decodetable_t]
.if !\persym
	sub			dword ptr [esp], %c[sizeof_combined_t]
.else
	add			dword ptr [esp], %c[sizeof_combined_t]
.endif

	.balign		64
0:
	add			ebx, %c[sizeof_decodetable_t]
1:
	cmp			edi, dword ptr [esp]
	jae			3f

	cmp			cl, 32
	jb			4f
	sub			cl, 32
	mov			ebp, dword ptr [esi+4+4]
	add			esi, 4

4:
	mov			eax, dword ptr [esi]
	shld		eax, ebp, cl
	shr			eax, 32 - %c[lookup_bits]
.if %c[sizeof_combined_t] == 4
	mov			edx, dword ptr [ebx + %c[offsetof_sym] + eax*4]
.else
.err
.endif
	movzx		eax, word ptr [ebx + %c[offsetof_cslen] + eax*2]

	add			cl, ah
	cmp			al, 0
	je			0b
	mov			ebx, dword ptr [esp + 4]

.if !\persym
.if %c[sizeof_combined_t] == 4
	mov			dword ptr [edi], edx
.else
.err
.endif
	movzx		eax, al
.if %c[sizeof_sym_t] == 1
	add			edi, eax
.elseif %c[sizeof_sym_t] == 2
	lea			edi, [edi + eax * 2]
.else
.err
.endif
	jmp			1b
.else
5:
.if %c[sizeof_combined_t] == 4
.if %c[sizeof_sym_t] == 1
	mov			byte ptr [edi], dl
.elseif %c[sizeof_sym_t] == 2
	mov			word ptr [edi], dx
.else
.err
.endif
	shr			edx, 8 * %c[sizeof_sym_t]
.else
.err
.endif
	add			edi, %c[sizeof_sym_t]
	cmp			edi, dword ptr [esp]
	jae			3f
	sub			al, 1
	ja			5b
	jmp			1b
.endif
3:
.endr

	pop			eax
	pop			ebx
	mov			eax, edi
.endm
	HUFFMAN_DECODE_8 )" ARGS R"(
.purgem HUFFMAN_DECODE_8
	)"
		: "=a"(ret), "=D"(clobber), "=S"(clobber), "=b"(clobber)
		: "D"(pDstBegin), "a"(pDstEnd), "S"(pSrcBegin), "b"(pDecodeTable),
		  [lookup_bits]"i"(decodetable_t::LOOKUP_BITS),
		  [offsetof_cslen]"i"(offsetof(decodetable_t, cslen)),
		  [offsetof_sym]"i"(offsetof(decodetable_t, sym)),
		  [sizeof_sym_t]"i"(sizeof(sym_t)),
		  [sizeof_combined_t]"i"(sizeof(decodetable_t::combined_t)),
		  [sizeof_decodetable_t]"i"(sizeof(decodetable_t))
		: "ecx", "edx", "ebp");
#endif
	return ret;

#undef ARGS
