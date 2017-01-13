; ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe
; $Id$


%include "Common_asm_x86.mac"


section .text


%push
global _sse2_RestoreWrongMedianBlock4
_sse2_RestoreWrongMedianBlock4:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride

	mov			esi, [esp + %$pSrcBegin]
	mov			edi, [esp + %$pDstBegin]
	mov			ebp, [esp + %$pSrcEnd]
	mov			ebx, [esp + %$cbWidth]
	mov			ecx, [esp + %$scbStride]
	mov			eax, ecx
	sub			ecx, ebx
	neg			eax

	mov			edx, 80808080h
	movd		xmm0, edx

	lea			edx, [esi+ebx]
.label0:
	movd		xmm1, [esi]
	paddb		xmm0, xmm1
	movd		[edi], xmm0
	add 		esi, 4
	add			edi, 4
	cmp			esi, edx
	jb			.label0

	add			esi, ecx
	add			edi, ecx
	cmp			esi, ebp
	je			.label4

	pxor		xmm4, xmm4
	pxor		xmm2, xmm2

.label1:
	lea			edx, [esi+ebx]
.label3:
	movdqa		xmm6, xmm2
	movdqa		xmm7, xmm2
	psubb		xmm6, xmm4
	movd		xmm4, [edi+eax]		; mm4 = above
	paddb		xmm6, xmm4			; mm6 = grad

	pminub		xmm2, xmm6
	pmaxub		xmm6, xmm7
	pmaxub		xmm2, xmm4
	pminub		xmm2, xmm6			; mm2 = median

	movd		xmm0, [esi]
	paddb		xmm2, xmm0
	movd		[edi], xmm2
	add			esi, 4
	add			edi, 4
	cmp			esi, edx
	jb			.label3

.label2:
	add			esi, ecx
	add			edi, ecx
	cmp			esi, ebp
	jne			.label1

.label4:
	SIMPLE_EPILOGUE
%pop
