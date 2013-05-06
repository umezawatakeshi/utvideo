; 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ
; $Id$


%include "Common_asm_x86.mac"


section .text


%push

global _sse2_PredictLeftAndCount_align1
_sse2_PredictLeftAndCount_align1:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, pCountTable

	mov			eax, 80h
	movd		xmm1, eax

	mov			esi, dword [esp + %$pSrcBegin]
	mov			edi, dword [esp + %$pDstBegin]
	mov			eax, dword [esp + %$pSrcEnd]
	sub			eax, esi
	and			eax, 0fffffff0h
	add			eax, esi

	mov			ebx, dword [esp + %$pCountTable]

	align		64
.label1:
	movdqu		xmm0, oword [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	psubb		xmm0, xmm2
	movdqu		oword [edi], xmm0

%assign pos 0
%rep 8
	pextrw		ecx, xmm0, pos
	movzx		ebp, cl
	inc			dword [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword [ebx+ebp*4]
%assign pos pos+1
%endrep

	add			esi, 16
	add			edi, 16
	cmp			esi, eax
	jb			.label1

	; 最初のラインの16バイトに満たない部分を処理する。
	; 若干のはみ出し読み込みが発生する。
	mov			eax, dword [esp + %$pSrcEnd]
	cmp			esi, eax
	jae			.label4

	movdqu		xmm0, oword [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	psubb		xmm0, xmm2

.label3:
	movd		ecx, xmm0
	mov			byte [edi], cl
	movzx		ebp, cl
	inc			dword [ebx+ebp*4]
	psrldq		xmm0, 1
	add			esi, 1
	add			edi, 1
	cmp			esi, eax
	jb			.label3

.label4:
	SIMPLE_EPILOGUE

%pop


%push

global _sse2_PredictWrongMedianAndCount_align16
_sse2_PredictWrongMedianAndCount_align16:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, dwStride, pCountTable

	mov			eax, 80h
	movd		xmm1, eax

	mov			esi, dword [esp + %$pSrcBegin]
	mov			edi, dword [esp + %$pDstBegin]
	mov			eax, esi
	mov			edx, dword [esp + %$dwStride]
	add			eax, edx
	neg			edx
	mov			ebx, dword [esp + %$pCountTable]

	align		64
.label1:
	movdqa		xmm0, oword [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	psubb		xmm0, xmm2
	movdqa		oword [edi], xmm0

%assign pos 0
%rep 8
	pextrw		ecx, xmm0, pos
	movzx		ebp, cl
	inc			dword [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword [ebx+ebp*4]
%assign pos pos+1
%endrep

	add			esi, 16
	add			edi, 16
	cmp			esi, eax
	jb			.label1

	mov			eax, dword [esp + %$pSrcEnd]

	pxor		xmm1, xmm1
	pxor		xmm5, xmm5

	align		64
.label2:
	movdqa		xmm0, oword [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	movdqa		xmm4, oword [esi+edx]
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
	movdqa		oword [edi], xmm0

%assign pos 0
%rep 8
	pextrw		ecx, xmm0, pos
	movzx		ebp, cl
	inc			dword [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword [ebx+ebp*4]
%assign pos pos+1
%endrep

	add			esi, 16
	add			edi, 16
	cmp			esi, eax
	jb			.label2

	SIMPLE_EPILOGUE

%pop


; prediction は前後にマージンを持つ CFrameBuffer 上で行うので、計算結果が変わらない限り、はみ出し読み込みは許容される。
; 一方、マルチスレッド動作した時に問題が発生するので、はみ出し書き込みは許容されない。

%push

global _sse2_PredictWrongMedianAndCount_align1
_sse2_PredictWrongMedianAndCount_align1:
	SIMPLE_PROLOGUE 0, pDstBegin, pSrcBegin, pSrcEnd, dwStride, pCountTable

	mov			eax, 80h
	movd		xmm1, eax

	mov			esi, dword [esp + %$pSrcBegin]
	mov			edi, dword [esp + %$pDstBegin]
	mov			eax, esi
	mov			edx, dword [esp + %$dwStride]
	and			edx, 0fffffff0h
	add			eax, edx
	mov			ebx, dword [esp + %$pCountTable]

	; 最初のラインを16バイトずつ処理する。
	align		64
.label1:
	movdqu		xmm0, oword [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	psubb		xmm0, xmm2
	movdqu		oword [edi], xmm0

%assign pos 0
%rep 8
	pextrw		ecx, xmm0, pos
	movzx		ebp, cl
	inc			dword [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword [ebx+ebp*4]
%assign pos pos+1
%endrep

	add			esi, 16
	add			edi, 16
	cmp			esi, eax
	jb			.label1

	; 最初のラインの16バイトに満たない部分を処理する。
	; 若干のはみ出し読み込みが発生する。
	mov			eax, dword [esp + %$pSrcBegin]
	add			eax, dword [esp + %$dwStride]
	cmp			esi, eax
	jae			.label4

	movdqu		xmm0, oword [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	psubb		xmm0, xmm2

.label3:
	movd		ecx, xmm0
	mov			byte [edi], cl
	movzx		ebp, cl
	inc			dword [ebx+ebp*4]
	psrldq		xmm0, 1
	add			esi, 1
	add			edi, 1
	cmp			esi, eax
	jb			.label3

	; 残りのラインを16バイトずつ処理する。
.label4:
	mov			edx, dword [esp + %$dwStride]
	neg			edx
	mov			eax, dword [esp + %$pSrcEnd]
	sub			eax, esi
	and			eax, 0fffffff0h
	add			eax, esi

	pxor		xmm1, xmm1
	pxor		xmm5, xmm5

	align		64
.label2:
	movdqu		xmm0, oword [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	movdqu		xmm4, oword [esi+edx]
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
	movdqu		oword [edi], xmm0

%assign pos 0
%rep 8
	pextrw		ecx, xmm0, pos
	movzx		ebp, cl
	inc			dword [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword [ebx+ebp*4]
%assign pos pos+1
%endrep

	add			esi, 16
	add			edi, 16
	cmp			esi, eax
	jb			.label2

	; 残りのラインの16バイトに満たない部分を処理する。
	; 若干のはみ出し読み込みが発生する。
	mov			eax, dword [esp + %$pSrcEnd]
	cmp			esi, eax
	jae			.label6

	movdqu		xmm0, oword [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1

	movdqu		xmm4, oword [esi+edx]
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
	mov			byte [edi], cl
	movzx		ebp, cl
	inc			dword [ebx+ebp*4]
	psrldq		xmm0, 1
	add			esi, 1
	add			edi, 1
	cmp			esi, eax
	jb			.label5

.label6:
	SIMPLE_EPILOGUE

%pop


%push

global _i686_RestoreWrongMedian_align1
_i686_RestoreWrongMedian_align1:
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

global _sse1mmx_RestoreWrongMedian_align1
_sse1mmx_RestoreWrongMedian_align1:
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
