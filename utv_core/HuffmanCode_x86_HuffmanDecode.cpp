/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

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
	shr			eax, 20
	mov			edx, dword ptr [ebx + 8192 + eax*4]							# pDecodeTable->MultiSpeedTable_sym[eax]
	movzx		eax, word ptr [ebx + eax*2]									# pDecodeTable->MultiSpeedTable_cs[eax]
	cmp			ah, 255
	jne			0f

	mov			eax, dword ptr [esi]
	shld		eax, ebp, cl
	mov			ch, cl
	or			eax, 1
	bsr			edx, eax
	mov			cl, byte ptr [ebx + 8192 + 16384 + edx]						# pDecodeTable->nCodeShift[edx]
	shr			eax, cl
	mov			cl, ch
	add			eax, dword ptr [ebx + 8192 + 16384 + 32 + edx*4]			# pDecodeTable->dwSymbolBase[edx]
	mov			eax, dword ptr [ebx + 8192 + 16384 + 32 + 4*32 + eax*2]		# pDecodeTable->SymbolAndCodeLength[eax]
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
		: "D"(pDstBegin), "a"(pDstEnd), "S"(pSrcBegin), "b"(pDecodeTable)
		: "ecx", "edx", "ebp");
#endif
	return ret;

#undef ARGS
