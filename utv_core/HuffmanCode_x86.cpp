/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include <stdlib.h>
#include <stdint.h>
#include "HuffmanCode.h"

extern "C" size_t i686_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE<8> *pEncodeTable)
{
	size_t ret;
#ifdef __GNUC__
	asm volatile (
	R"(
	.intel_syntax noprefix

	mov			esi, %[pSrcBegin]
	mov			edi, %[pDstBegin]
	mov			ebp, %[pSrcEnd]
	mov			edx, %[pEncodeTable]
	cmp			dword ptr [edx], 0
	je			3f

	mov			bl, -32
	mov			cl, 0

	.align		64
1:
	shld		eax, ecx, cl
	cmp			esi, ebp
	jnb			4f
	movzx		ecx, byte ptr [esi]
	add			esi, 1
	mov			ecx, dword ptr [edx+ecx*4]
	add			bl, cl
	jnc			1b
	sub			cl, bl
	shld		eax, ecx, cl
	mov			dword ptr [edi], eax
	add			edi, 4
	add			cl, bl
	sub			bl, 32
	jmp			1b

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
	sub			eax, %[pDstBegin]
	)"
		: "=a"(ret)
		: [pDstBegin]"m"(pDstBegin), [pSrcBegin]"m"(pSrcBegin), [pSrcEnd]"m"(pSrcEnd), [pEncodeTable]"m"(pEncodeTable)
		: "ebx", "ecx", "edx", "esi", "edi", "ebp");
#endif
	return ret;
}

extern "C" uint8_t *i686_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable)
{
	uint8_t *ret;
#ifdef __GNUC__
	asm volatile (
	R"(
	.intel_syntax noprefix

	mov			esi, %[pSrcBegin]
	mov			ebx, %[pDecodeTable]

	mov			edi, %[pDstBegin]
	sub			%[pDstEnd], 4

	mov			cl, 32
	sub			esi, 4

.macro DO_OUTPUT perbyte

	.align		64
1:
	cmp			edi, %[pDstEnd]
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
	cmp			edi, %[pDstEnd]
	jae			3f
	sub			al, 1
	ja			5b
	jmp			1b
.endif
3:
.endm

	DO_OUTPUT	0
	add			%[pDstEnd], 4
	DO_OUTPUT	1
	mov			eax, edi
	)"
		: "=a"(ret)
		: [pDstBegin]"m"(pDstBegin), [pDstEnd]"m"(pDstEnd), [pSrcBegin]"m"(pSrcBegin), [pDecodeTable]"m"(pDecodeTable)
		: "ebx", "ecx", "edx", "esi", "edi", "ebp");
#endif
	return ret;
}
