/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include <stdlib.h>
#include <stdint.h>
#include "HuffmanCode.h"

extern "C" size_t i686_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<8> *pEncodeTable)
{
	size_t ret;
	void* clobber;
#ifdef __GNUC__
	asm volatile (
	R"(
	.intel_syntax noprefix

	# edi = pDstBegin
	# esi = pSrcBegin
	# eax = pSrcEnd
	# edx = pEncodeTable

	push		edi
	mov			ebp, eax
	cmp			dword ptr [edx], 0
	je			3f

	mov			bl, -32
	mov			cl, 0

	mov			eax, ebp
	sub			eax, esi
	and			eax, 3

	cmp			eax, 3
	jne			1f
	sub			esi, 1
	jmp			label1

1:
	cmp			eax, 2
	jne			1f
	sub			esi, 2
	jmp			label2

1:
	cmp			eax, 1
	jne			1f
	sub			esi, 3
	jmp			label3

	.balign		64
1:
.irp offset, 0, 1, 2, 3
label\offset:
	shld		eax, ecx, cl
.if \offset == 0
	cmp			esi, ebp
	jnb			4f
.endif

	movzx		ecx, byte ptr [esi+\offset]
.if \offset == 3
	add			esi, 4
.endif
	mov			ecx, dword ptr [edx+ecx*4]
	add			bl, cl
.if \offset != 3
	jnc			2f
.else
	jnc			1b
.endif
	sub			cl, bl
	shld		eax, ecx, cl
	mov			dword ptr [edi], eax
	add			edi, 4
	add			cl, bl
	sub			bl, 32
.if \offset == 3
	jmp			1b
.endif
2:
.endr

4:
	test		bl, 0x1f
	jz			3f
	neg			bl
	mov			cl, bl
	shl			eax, cl
	mov			dword ptr [edi], eax
	add			edi, 4
3:
	mov			eax, edi
	pop			edi
	sub			eax, edi
	)"
		: "=a"(ret), "=D"(clobber), "=S"(clobber), "=d"(clobber)
		: "D"(pDstBegin), "S"(pSrcBegin), "a"(pSrcEnd), "d"(pEncodeTable)
		: "ebx", "ecx", "ebp");
#endif
	return ret;
}

extern "C" uint8_t *i686_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable)
{
	uint8_t *ret;
	void* clobber;
#ifdef __GNUC__
	asm volatile (
	R"(
	.intel_syntax noprefix

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
	)"
		: "=a"(ret), "=D"(clobber), "=S"(clobber), "=b"(clobber)
		: "D"(pDstBegin), "a"(pDstEnd), "S"(pSrcBegin), "b"(pDecodeTable)
		: "ecx", "edx", "ebp");
#endif
	return ret;
}
