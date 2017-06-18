/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

	typedef typename std::remove_reference<decltype(*pDecodeTable)>::type decodetable_t;
	uint8_t *ret;
	void* clobber;
#ifdef __GNUC__
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

.irp perbyte, 0, 1
.if !\perbyte
	sub			dword ptr [esp], 4
.else
	add			dword ptr [esp], 4
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
	mov			edx, dword ptr [ebx + %c[offsetof_multispeedtable_sym] + eax*4]
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
	mov			eax, dword ptr [ebx + %c[offsetof_symbolandcodelength] + eax*2]
	mov			edx, eax
	mov			al, 1

0:
	add			cl, ah

.if !\perbyte
	mov			dword ptr [edi], edx
	movzx		eax, al
	add			edi, eax
	jmp			1b
.else
5:
	mov			byte ptr [edi], dl
	shr			edx, 8
	add			edi, 1
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
		  [offsetof_symbolandcodelength]"i"(offsetof(decodetable_t, SymbolAndCodeLength))
		: "ecx", "edx", "ebp");
#endif
	return ret;

#undef ARGS
