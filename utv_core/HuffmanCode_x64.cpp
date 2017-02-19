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

	mov			rsi, %[pSrcBegin]
	mov			rdi, %[pDstBegin]
	mov			r8,  %[pSrcEnd]
	mov			rdx, %[pEncodeTable]
	cmp			qword ptr [rdx], 0
	je			3f

	mov			bl, -64
	mov			cl, 0

	.align		64
1:
	shld		rax, rcx, cl
	cmp			rsi, r8
	jnb			4f
	movzx		rcx, byte ptr [rsi]
	add			rsi, 1
	mov			rcx, qword ptr [rdx+rcx*8]
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
	sub			rax, %[pDstBegin]
	)"
		: "=a"(ret)
		: [pDstBegin]"m"(pDstBegin), [pSrcBegin]"m"(pSrcBegin), [pSrcEnd]"m"(pSrcEnd), [pEncodeTable]"m"(pEncodeTable)
		: "rbx", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15");
#endif
	return ret;
}

#ifdef __GNUC__
	asm volatile (
	R"(
	.intel_syntax noprefix

.macro HUFFMAN_DECODE_8 procname, bmi2
	sub			r8, 4

	mov			cl, 32
	mov			edx, dword ptr [rsi]
	add			rsi, 4

.macro DO_OUTPUT_\procname perbyte
	.align		64
1:
	cmp			rdi, r8
	jae			3f

	cmp			cl, 32
	jb			4f
	sub			cl, 32
	shl			rdx, 32
	mov			r9d, dword ptr [rsi]
	or			rdx, r9
	add			rsi, 4

4:
.if \bmi2
	shlx		rax, rdx, rcx
.else
	mov			rax, rdx
	shl			rax, cl
.endif
	shr			rax, 20+32
	mov			r11d, dword ptr [rbx + 8192 + rax*4]						# pDecodeTable->MultiSpeedTable_sym[rax]
	movzx		eax, word ptr [rbx + rax*2]									# pDecodeTable->MultiSpeedTable_cs[rax]
	cmp			ah, 255
	jne			0f

.if \bmi2
	shlx		rax, rdx, rcx
.else
	mov			rax, rdx
	shl			rax, cl
	mov			r15d, ecx
.endif
	shr			rax, 32
	or			eax, 1
	bsr			r10, rax
.if \bmi2
	movzx		r13d, byte ptr [rbx + 8192 + 16384 + r10]					# pDecodeTable->nCodeShift[r10]
	shrx		eax, eax, r13d
.else
	mov			cl, byte ptr [rbx + 8192 + 16384 + r10]						# pDecodeTable->nCodeShift[r10]
	shr			eax, cl
	mov			ecx, r15d
.endif
	mov			r10d, dword ptr [rbx + 8192 + 16384 + 32 + r10*4]			# pDecodeTable->dwSymbolBase[r10]
	add			r10, rax
	mov			eax, dword ptr [rbx + 8192 + 16384 + 32 + 4*32 + r10*2]		# pDecodeTable->SymbolAndCodeLength[r10]
	mov			r11d, eax
	mov			al, 1

0:
	add			cl, ah

.if !\perbyte
	mov			dword ptr [rdi], r11d
	movzx		rax, al
	add			rdi, rax
	jmp			1b
.else
5:
	mov			byte ptr [rdi], r11b
	shr			r11d, 8
	add			rdi, 1
	cmp			rdi, r8
	jae			3f
	sub			al, 1
	ja			5b
	jmp			1b
.endif

3:
.endm

	DO_OUTPUT_\procname	0
	add			r8, 4
	DO_OUTPUT_\procname	1
	mov			rax, rdi
.endm
	)");
#endif

extern "C" uint8_t *i686_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable)
{
	uint8_t *ret;
#ifdef __GNUC__
	asm volatile (
	R"(
	.intel_syntax noprefix

	mov			rsi, %[pSrcBegin]
	mov			rbx, %[pDecodeTable]

	mov			rdi, %[pDstBegin]
	mov			r8,  %[pDstEnd]
	HUFFMAN_DECODE_8 i686_HuffmanDecode, 0
	)"
		: "=a"(ret)
		: [pDstBegin]"m"(pDstBegin), [pDstEnd]"m"(pDstEnd), [pSrcBegin]"m"(pSrcBegin), [pDecodeTable]"m"(pDecodeTable)
		: "rbx", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15");
#endif
	return ret;
}

extern "C" uint8_t *bmi2_HuffmanDecode(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE<8> *pDecodeTable)
{
	uint8_t *ret;
#ifdef __GNUC__
	asm volatile (
	R"(
	.intel_syntax noprefix

	mov			rsi, %[pSrcBegin]
	mov			rbx, %[pDecodeTable]

	mov			rdi, %[pDstBegin]
	mov			r8,  %[pDstEnd]
	HUFFMAN_DECODE_8 bmi2_HuffmanDecode, 1
	)"
		: "=a"(ret)
		: [pDstBegin]"m"(pDstBegin), [pDstEnd]"m"(pDstEnd), [pSrcBegin]"m"(pSrcBegin), [pDecodeTable]"m"(pDecodeTable)
		: "rbx", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15");
#endif
	return ret;
}
