; 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ
; $Id$


%include "Common_asm_x86.mac"


section .text


%push

global _i686_RestoreWrongMedian
_i686_RestoreWrongMedian:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, dwStride

	mov			esi, dword [esp + %$pSrcBegin]
	mov			edi, dword [esp + %$pDstBegin]
	mov			eax, esi
	mov			ebp, dword [esp + %$dwStride]
	add			eax, ebp
	neg			ebp

	mov			edx, 80h

	align		64
.label1:
	add			dl, byte [esi]
	mov			byte [edi], dl
	add 		esi, 1
	add			edi, 1
	cmp			esi, eax
	jb			.label1

	xor			ecx, ecx
	xor			edx, edx

	align		64
.label2:
	mov			ebx, edx
	sub			ebx, ecx
	movzx		ecx, byte [edi+ebp]
	add			ebx, ecx	; bl = grad

	mov			eax, edx
	cmp			dl, cl
	cmovae		edx, ecx	; dl = min(dl,cl)
	cmovb		eax, ecx	; al = max(dl,cl)

	cmp			dl, bl
	cmovb		edx, ebx	; dl = max(dl,bl)
	cmp			dl, al
	cmovae		edx, eax	; dl = min(dl,al)

	movzx		ebx, byte [esi]
	add			edx, ebx
	mov			byte [edi], dl

	add			esi, 1
	add			edi, 1
	cmp			esi, dword [esp + %$pSrcEnd]	; pSrcEnd
	jb			.label2

	SIMPLE_EPILOGUE

%pop


; p{min,max}ub は SSE1 で追加された MMX 命令（いわゆる MMX2 命令）である。

%push

global _sse1mmx_RestoreWrongMedian
_sse1mmx_RestoreWrongMedian:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, dwStride

	mov			esi, dword [esp + %$pSrcBegin]
	mov			edi, dword [esp + %$pDstBegin]
	mov			eax, esi
	mov			ebp, dword [esp + %$dwStride]
	add			eax, ebp
	neg			ebp

	mov			edx, 80h

	align		64
.label1:
	add			dl, byte [esi]
	mov			byte [edi], dl
	add 		esi, 1
	add			edi, 1
	cmp			esi, eax
	jb			.label1

	pxor		mm4, mm4
	pxor		mm2, mm2

	align		64
.label2:
	movq		mm6, mm2
	movq		mm7, mm2
	psubb		mm6, mm4
	movd		mm4, dword [edi+ebp]		; mm4 = above
	paddb		mm6, mm4					; mm6 = grad

	pminub		mm2, mm6
	pmaxub		mm6, mm7
	pmaxub		mm2, mm4
	pminub		mm2, mm6					; mm2 = median

	paddb		mm2, qword [esi]			; アライメントがずれていても xmm レジスタの場合と違って一般保護例外にはならない
	movd		eax, mm2
	mov			byte [edi], al

	add			esi, 1
	add			edi, 1
	cmp			esi, dword [esp + %$pSrcEnd]
	jb			.label2

	emms
	SIMPLE_EPILOGUE

%pop


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
