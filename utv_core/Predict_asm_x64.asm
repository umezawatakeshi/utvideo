; 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ
; $Id$


%include "Common_asm_x64.mac"


section .text


; p{min,max}ub は SSE1 で追加された MMX 命令（いわゆる MMX2 命令）である。

%push

global sse1mmx_RestoreWrongMedian
sse1mmx_RestoreWrongMedian:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, dwStride

	mov			rsi, qword [rsp + %$pSrcBegin]
	mov			rdi, qword [rsp + %$pDstBegin]
	mov			rax, rsi
	mov			rbp, qword [rsp + %$dwStride]
	add			rax, rbp
	neg			rbp

	mov			edx, 80h

	align		64
.label1:
	add			dl, byte [rsi]
	mov			byte [rdi], dl
	add 		rsi, 1
	add			rdi, 1
	cmp			rsi, rax
	jb			.label1

	pxor		mm4, mm4
	pxor		mm2, mm2

	align		64
.label2:
	movq		mm6, mm2
	movq		mm7, mm2
	psubb		mm6, mm4
	movd		mm4, dword [rdi+rbp]		; mm4 = above
	paddb		mm6, mm4					; mm6 = grad

	pminub		mm2, mm6
	pmaxub		mm6, mm7
	pmaxub		mm2, mm4
	pminub		mm2, mm6					; mm2 = median

	paddb		mm2, qword [rsi]		; アライメントがずれていても xmm レジスタの場合と違って一般保護例外にはならない
	movd		eax, mm2
	mov			byte [rdi], al

	add			rsi, 1
	add			rdi, 1
	cmp			rsi, qword [rsp + %$pSrcEnd]
	jb			.label2

	emms
	SIMPLE_EPILOGUE

%pop


%push
global sse2_RestoreWrongMedianBlock4
sse2_RestoreWrongMedianBlock4:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride

	mov			rsi, [rsp + %$pSrcBegin]
	mov			rdi, [rsp + %$pDstBegin]
	mov			r9,  [rsp + %$pSrcEnd]
	mov			rbx, [rsp + %$cbWidth]
	mov			rcx, [rsp + %$scbStride]
	mov			rax, rcx
	sub			rcx, rbx
	neg			rax

	mov			edx, 80808080h
	movd		xmm0, edx

	lea			rdx, [rsi+rbx]
.label0:
	movd		xmm1, [rsi]
	paddb		xmm0, xmm1
	movd		[rdi], xmm0
	add 		rsi, 4
	add			rdi, 4
	cmp			rsi, rdx
	jb			.label0

	add			rsi, rcx
	add			rdi, rcx
	cmp			rsi, r9
	je			.label4

	pxor		xmm4, xmm4
	pxor		xmm2, xmm2

.label1:
	lea			rdx, [rsi+rbx]
.label3:
	movdqa		xmm6, xmm2
	movdqa		xmm7, xmm2
	psubb		xmm6, xmm4
	movd		xmm4, [rdi+rax]		; mm4 = above
	paddb		xmm6, xmm4			; mm6 = grad

	pminub		xmm2, xmm6
	pmaxub		xmm6, xmm7
	pmaxub		xmm2, xmm4
	pminub		xmm2, xmm6			; mm2 = median

	movd		xmm0, [rsi]
	paddb		xmm2, xmm0
	movd		[rdi], xmm2
	add			rsi, 4
	add			rdi, 4
	cmp			rsi, rdx
	jb			.label3

.label2:
	add			rsi, rcx
	add			rdi, rcx
	cmp			rsi, r9
	jne			.label1

.label4:
	SIMPLE_EPILOGUE
%pop
