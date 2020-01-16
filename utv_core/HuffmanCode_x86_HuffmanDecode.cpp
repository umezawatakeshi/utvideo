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

	push		eax

	mov			cl, 32
	sub			esi, 4

.irp persym, 0, 1
.if !\persym
	sub			dword ptr [esp], %c[sizeof_combined_t]
.else
	add			dword ptr [esp], %c[sizeof_combined_t]
.endif

	.balign		64
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
	mov			edx, dword ptr [ebx + %c[offsetof_multispeedtable_sym] + eax*4]
.else
.err
.endif
	movzx		eax, word ptr [ebx + %c[offsetof_multispeedtable_cs] + eax*2]
	cmp			ah, 255
	jne			0f

	mov			eax, dword ptr [esi]
	shld		eax, ebp, cl
	mov			ch, cl
	or			eax, 1
	bsr			edx, eax
	mov			cl, byte ptr [ebx + %c[offsetof_codeshift] + edx]
	shr			eax, cl
	mov			cl, ch
	add			eax, dword ptr [ebx + %c[offsetof_symbolbase] + edx*4]
.if %c[sizeof_combined_t] == 4
.if %c[sizeof_sym_t] == 1
	mov			eax, dword ptr [ebx + %c[offsetof_symbolandcodelength] + eax*2]
	mov			edx, eax
.elseif %c[sizeof_sym_t] == 2
	mov			eax, dword ptr [ebx + %c[offsetof_symbolandcodelength] + eax*4]
	mov			edx, eax
	shr			eax, 8
.else
.err
.endif
.else
.err
.endif
	mov			al, 1

0:
	add			cl, ah

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
	mov			eax, edi
.endm
	HUFFMAN_DECODE_8 )" ARGS R"(
.purgem HUFFMAN_DECODE_8
	)"
		: "=a"(ret), "=D"(clobber), "=S"(clobber), "=b"(clobber)
		: "D"(pDstBegin), "a"(pDstEnd), "S"(pSrcBegin), "b"(pDecodeTable),
		  [lookup_bits]"i"(decodetable_t::LOOKUP_BITS),
		  [offsetof_multispeedtable_cs]"i"(offsetof(decodetable_t, MultiSpeedTable_cs)),
		  [offsetof_multispeedtable_sym]"i"(offsetof(decodetable_t, MultiSpeedTable_sym)),
		  [offsetof_codeshift]"i"(offsetof(decodetable_t, nCodeShift)),
		  [offsetof_symbolbase]"i"(offsetof(decodetable_t, dwSymbolBase)),
		  [offsetof_symbolandcodelength]"i"(offsetof(decodetable_t, SymbolAndCodeLength)),
		  [sizeof_sym_t]"i"(sizeof(sym_t)),
		  [sizeof_combined_t]"i"(sizeof(decodetable_t::combined_t))
		: "ecx", "edx", "ebp");
#endif
	return ret;

#undef ARGS
