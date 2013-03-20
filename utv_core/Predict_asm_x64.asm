; 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ
; $Id$


%include "Common_asm_x64.mac"


section .text


%push

global x64_sse2_PredictLeftAndCount_align1
x64_sse2_PredictLeftAndCount_align1:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, pCountTable

	mov			eax, 80h
	movd		xmm1, eax

	mov			rsi, qword [rsp + %$pSrcBegin]
	mov			rdi, qword [rsp + %$pDstBegin]
	mov			rax, qword [rsp + %$pSrcEnd]
	sub			rax, rsi
	and			rax, 0fffffffffffffff0h
	add			rax, rsi

	mov			rbx, qword [rsp + %$pCountTable]

	align		64
.label1:
	movdqu		xmm0, oword [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	psubb		xmm0, xmm2
	movdqu		oword [rdi], xmm0

%assign pos 0
%rep 8
	pextrw		ecx, xmm0, pos
	movzx		ebp, cl
	inc			dword [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword [rbx+rbp*4]
%assign pos pos+1
%endrep

	add			rsi, 16
	add			rdi, 16
	cmp			rsi, rax
	jb			.label1

	; 最初のラインの16バイトに満たない部分を処理する。
	; 若干のはみ出し読み込みが発生する。
	mov			rax, qword [rsp + %$pSrcEnd]
	cmp			rsi, rax
	jae			.label4

	movdqu		xmm0, oword [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	psubb		xmm0, xmm2

.label3:
	movd		ecx, xmm0
	mov			byte [rdi], cl
	movzx		ebp, cl
	inc			dword [rbx+rbp*4]
	psrldq		xmm0, 1
	inc			rsi
	inc			rdi
	cmp			rsi, rax
	jb			.label3

.label4:
	SIMPLE_EPILOGUE

%pop


%push

global x64_sse2_PredictWrongMedianAndCount_align16
x64_sse2_PredictWrongMedianAndCount_align16:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, dwStride, pCountTable

	mov			eax, 80h
	movd		xmm1, eax

	mov			rsi, qword [rsp + %$pSrcBegin]
	mov			rdi, qword [rsp + %$pDstBegin]
	mov			rax, rsi
	mov			rdx, qword [rsp + %$dwStride]
	add			rax, rdx
	neg			rdx
	mov			rbx, qword [rsp + %$pCountTable]

	align		64
.label1:
	movdqa		xmm0, oword [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	psubb		xmm0, xmm2
	movdqa		oword [rdi], xmm0

%assign pos 0
%rep 8
	pextrw		ecx, xmm0, pos
	movzx		ebp, cl
	inc			dword [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword [rbx+rbp*4]
%assign pos pos+1
%endrep

	add			rsi, 16
	add			rdi, 16
	cmp			rsi, rax
	jb			.label1

	mov			rax, qword [rsp + %$pSrcEnd]

	pxor		xmm1, xmm1
	pxor		xmm5, xmm5

	align		64
.label2:
	movdqa		xmm0, oword [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	movdqa		xmm4, oword [rsi+rdx]
	movdqa		xmm6, xmm4
	pslldq		xmm6, 1
	por			xmm6, xmm5
	movdqa		xmm5, xmm4
	psrldq		xmm5, 15

	movdqa		xmm7, xmm2
	paddb		xmm7, xmm4
	psubb		xmm7, xmm6

	movdqa		xmm3, xmm2
	pminub		xmm2, xmm4
	pmaxub		xmm3, xmm4
	pmaxub		xmm7, xmm2
	pminub		xmm7, xmm3	; predicted = min(max(min(left, above), grad), max(left, above))

	psubb		xmm0, xmm7
	movdqa		oword [rdi], xmm0

%assign pos 0
%rep 8
	pextrw		ecx, xmm0, pos
	movzx		ebp, cl
	inc			dword [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword [rbx+rbp*4]
%assign pos pos+1
%endrep

	add			rsi, 16
	add			rdi, 16
	cmp			rsi, rax
	jb			.label2

	SIMPLE_EPILOGUE

%pop


; prediction は前後にマージンを持つ CFrameBuffer 上で行うので、計算結果が変わらない限り、はみ出し読み込みは許容される。
; 一方、マルチスレッド動作した時に問題が発生するので、はみ出し書き込みは許容されない。

%push

global x64_sse2_PredictWrongMedianAndCount_align1
x64_sse2_PredictWrongMedianAndCount_align1:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, dwStride, pCountTable

	mov			eax, 80h
	movd		xmm1, eax

	mov			rsi, qword [rsp + %$pSrcBegin]
	mov			rdi, qword [rsp + %$pDstBegin]
	mov			rax, rsi
	mov			rdx, qword [rsp + %$dwStride]
	and			rdx, 0fffffffffffffff0h
	add			rax, rdx
	mov			rbx, qword [rsp + %$pCountTable]

	; 最初のラインを16バイトずつ処理する。
	align		64
.label1:
	movdqu		xmm0, oword [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	psubb		xmm0, xmm2
	movdqu		oword [rdi], xmm0

%assign pos 0
%rep 8
	pextrw		ecx, xmm0, pos
	movzx		ebp, cl
	inc			dword [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword [rbx+rbp*4]
%assign pos pos+1
%endrep

	add			rsi, 16
	add			rdi, 16
	cmp			rsi, rax
	jb			.label1

	; 最初のラインの16バイトに満たない部分を処理する。
	; 若干のはみ出し読み込みが発生する。
	mov			rax, qword [rsp + %$pSrcBegin]
	add			rax, qword [rsp + %$dwStride]
	cmp			rsi, rax
	jae			.label4

	movdqu		xmm0, oword [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	psubb		xmm0, xmm2

.label3:
	movd		ecx, xmm0
	mov			byte [rdi], cl
	movzx		ebp, cl
	inc			dword [rbx+rbp*4]
	psrldq		xmm0, 1
	inc			rsi
	inc			rdi
	cmp			rsi, rax
	jb			.label3

	; 残りのラインを16バイトずつ処理する。
.label4:
	mov			rdx, qword [rsp + %$dwStride]
	neg			rdx
	mov			rax, qword [rsp + %$pSrcEnd]
	sub			rax, rsi
	and			rax, 0fffffffffffffff0h
	add			rax, rsi

	pxor		xmm1, xmm1
	pxor		xmm5, xmm5

	align		64
.label2:
	movdqu		xmm0, oword [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	movdqu		xmm4, oword [rsi+rdx]
	movdqa		xmm6, xmm4
	pslldq		xmm6, 1
	por			xmm6, xmm5
	movdqa		xmm5, xmm4
	psrldq		xmm5, 15

	movdqa		xmm7, xmm2
	paddb		xmm7, xmm4
	psubb		xmm7, xmm6

	movdqa		xmm3, xmm2
	pminub		xmm2, xmm4
	pmaxub		xmm3, xmm4
	pmaxub		xmm7, xmm2
	pminub		xmm7, xmm3	; predicted = min(max(min(left, above), grad), max(left, above))

	psubb		xmm0, xmm7
	movdqu		oword [rdi], xmm0

%assign pos 0
%rep 8
	pextrw		ecx, xmm0, pos
	movzx		ebp, cl
	inc			dword [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword [rbx+rbp*4]
%assign pos pos+1
%endrep

	add			rsi, 16
	add			rdi, 16
	cmp			rsi, rax
	jb			.label2

	; 残りのラインの16バイトに満たない部分を処理する。
	; 若干のはみ出し読み込みが発生する。
	mov			rax, qword [rsp + %$pSrcEnd]
	cmp			rsi, rax
	jae			.label6

	movdqu		xmm0, oword [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1

	movdqu		xmm4, oword [rsi+rdx]
	movdqa		xmm6, xmm4
	pslldq		xmm6, 1
	por			xmm6, xmm5

	movdqa		xmm7, xmm2
	paddb		xmm7, xmm4
	psubb		xmm7, xmm6

	movdqa		xmm3, xmm2
	pminub		xmm2, xmm4
	pmaxub		xmm3, xmm4
	pmaxub		xmm7, xmm2
	pminub		xmm7, xmm3	; predicted = min(max(min(left, above), grad), max(left, above))

	psubb		xmm0, xmm7

.label5:
	movd		ecx, xmm0
	mov			byte [rdi], cl
	movzx		ebp, cl
	inc			dword [rbx+rbp*4]
	psrldq		xmm0, 1
	inc			rsi
	inc			rdi
	cmp			rsi, rax
	jb			.label5

.label6:
	SIMPLE_EPILOGUE

%pop


; p{min,max}ub は SSE1 で追加された MMX 命令（いわゆる MMX2 命令）である。

%push

global x64_sse1mmx_RestoreWrongMedian_align1
x64_sse1mmx_RestoreWrongMedian_align1:
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
	inc 		rsi
	inc			rdi
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

	inc			rsi
	inc			rdi
	cmp			rsi, qword [rsp + %$pSrcEnd]
	jb			.label2

	emms
	SIMPLE_EPILOGUE

%pop
