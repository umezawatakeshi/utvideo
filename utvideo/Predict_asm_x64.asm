; 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ
; $Id$


_TEXT_ASM	SEGMENT	page 'CODE'

; void x64_sse2_PredictLeftAndCount_align1(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD *pCountTable)
public	x64_sse2_PredictLeftAndCount_align1
x64_sse2_PredictLeftAndCount_align1	proc

	mov			[rsp +  8], rcx
	mov			[rsp + 16], rdx
	mov			[rsp + 24], r8
	mov			[rsp + 32], r9
	push		rbx
	push		rbp
	push		rsi
	push		rdi
	push		r12
	push		r13
	push		r14
	push		r15

	mov			eax, 80h
	movd		xmm1, eax

	mov			rsi, qword ptr [rsp + 64 + 8 +  8]	; pSrcBegin
	mov			rdi, qword ptr [rsp + 64 + 8 +  0]	; pDst
	mov			rax, qword ptr [rsp + 64 + 8 + 16]	; pSrcEnd
	sub			rax, rsi
	and			rax, 0fffffffffffffff0h
	add			rax, rsi

	mov			rbx, qword ptr [rsp + 64 + 8 + 24]	; pCountTable

	align		64
label1:
	movdqu		xmm0, oword ptr [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	psubb		xmm0, xmm2
	movdqu		oword ptr [rdi], xmm0

	pextrw		ecx, xmm0, 0
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 1
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 2
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 3
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 4
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 5
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 6
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 7
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]

	add			rsi, 16
	add			rdi, 16
	cmp			rsi, rax
	jb			label1

	; 最初のラインの16バイトに満たない部分を処理する。
	; 若干のはみ出し読み込みが発生する。
	mov			rax, qword ptr [rsp + 64 + 8 + 16]	; pSrcEnd
	cmp			rsi, rax
	jae			label4

	movdqu		xmm0, oword ptr [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	psubb		xmm0, xmm2

label3:
	movd		ecx, xmm0
	mov			byte ptr [rdi], cl
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	psrldq		xmm0, 1
	inc			rsi
	inc			rdi
	cmp			rsi, rax
	jb			label3

label4:

	pop			r15
	pop			r14
	pop			r13
	pop			r12
	pop			rdi
	pop			rsi
	pop			rbp
	pop			rbx
	ret

x64_sse2_PredictLeftAndCount_align1	endp


; void x64_sse2_PredictMedianAndCount_align16(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride, DWORD *pCountTable)
public	x64_sse2_PredictMedianAndCount_align16
x64_sse2_PredictMedianAndCount_align16	proc

	mov			[rsp +  8], rcx
	mov			[rsp + 16], rdx
	mov			[rsp + 24], r8
	mov			[rsp + 32], r9
	push		rbx
	push		rbp
	push		rsi
	push		rdi
	push		r12
	push		r13
	push		r14
	push		r15

	mov			eax, 80h
	movd		xmm1, eax

	mov			rsi, qword ptr [rsp + 64 + 8 +  8]	; pSrcBegin
	mov			rdi, qword ptr [rsp + 64 + 8 +  0]	; pDst
	mov			rax, rsi
	mov			rdx, qword ptr [rsp + 64 + 8 + 24]	; dwStride
	add			rax, rdx
	neg			rdx
	mov			rbx, qword ptr [rsp + 64 + 8 + 32]	; pCountTable

	align		64
label1:
	movdqa		xmm0, oword ptr [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	psubb		xmm0, xmm2
	movdqa		oword ptr [rdi], xmm0

	pextrw		ecx, xmm0, 0
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 1
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 2
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 3
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 4
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 5
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 6
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 7
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]

	add			rsi, 16
	add			rdi, 16
	cmp			rsi, rax
	jb			label1

	mov			rax, qword ptr [rsp + 64 + 8 + 16]	; pSrcEnd

	pxor		xmm1, xmm1
	pxor		xmm5, xmm5

	align		64
label2:
	movdqa		xmm0, oword ptr [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	movdqa		xmm4, oword ptr [rsi+rdx]
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
	movdqa		oword ptr [rdi], xmm0

	pextrw		ecx, xmm0, 0
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 1
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 2
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 3
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 4
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 5
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 6
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 7
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]

	add			rsi, 16
	add			rdi, 16
	cmp			rsi, rax
	jb			label2

	pop			r15
	pop			r14
	pop			r13
	pop			r12
	pop			rdi
	pop			rsi
	pop			rbp
	pop			rbx
	ret

x64_sse2_PredictMedianAndCount_align16	endp


; void x64_sse2_PredictMedianAndCount_align1(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride, DWORD *pCountTable)
public	x64_sse2_PredictMedianAndCount_align1
x64_sse2_PredictMedianAndCount_align1	proc

	mov			[rsp +  8], rcx
	mov			[rsp + 16], rdx
	mov			[rsp + 24], r8
	mov			[rsp + 32], r9
	push		rbx
	push		rbp
	push		rsi
	push		rdi
	push		r12
	push		r13
	push		r14
	push		r15

	mov			eax, 80h
	movd		xmm1, eax

	mov			rsi, qword ptr [rsp + 64 + 8 +  8]	; pSrcBegin
	mov			rdi, qword ptr [rsp + 64 + 8 +  0]	; pDst
	mov			rax, rsi
	mov			rdx, qword ptr [rsp + 64 + 8 + 24]	; dwStride
	and			rdx, 0fffffffffffffff0h
	add			rax, rdx
	mov			rbx, qword ptr [rsp + 64 + 8 + 32]	; pCountTable

	; 最初のラインを16バイトずつ処理する。
	align		64
label1:
	movdqu		xmm0, oword ptr [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	psubb		xmm0, xmm2
	movdqu		oword ptr [rdi], xmm0

	pextrw		ecx, xmm0, 0
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 1
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 2
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 3
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 4
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 5
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 6
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 7
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]

	add			rsi, 16
	add			rdi, 16
	cmp			rsi, rax
	jb			label1

	; 最初のラインの16バイトに満たない部分を処理する。
	; 若干のはみ出し読み込みが発生する。
	mov			rax, qword ptr [rsp + 64 + 8 +  8]	; pSrcBegin
	add			rax, qword ptr [rsp + 64 + 8 + 24]	; dwStride
	cmp			rsi, rax
	jae			label4

	movdqu		xmm0, oword ptr [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	psubb		xmm0, xmm2

label3:
	movd		ecx, xmm0
	mov			byte ptr [rdi], cl
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	psrldq		xmm0, 1
	inc			rsi
	inc			rdi
	cmp			rsi, rax
	jb			label3

	; 残りのラインを16バイトずつ処理する。
label4:
	mov			rdx, qword ptr [rsp + 64 + 8 + 24]	; dwStride
	neg			rdx
	mov			rax, qword ptr [rsp + 64 + 8 + 16]	; pSrcEnd
	sub			rax, rsi
	and			rax, 0fffffffffffffff0h
	add			rax, rsi

	pxor		xmm1, xmm1
	pxor		xmm5, xmm5

	align		64
label2:
	movdqu		xmm0, oword ptr [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	movdqu		xmm4, oword ptr [rsi+rdx]
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
	movdqu		oword ptr [rdi], xmm0

	pextrw		ecx, xmm0, 0
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 1
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 2
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 3
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 4
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 5
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 6
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]
	pextrw		ecx, xmm0, 7
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	movzx		ebp, ch
	inc			dword ptr [rbx+rbp*4]

	add			rsi, 16
	add			rdi, 16
	cmp			rsi, rax
	jb			label2

	; 残りのラインの16バイトに満たない部分を処理する。
	; 若干のはみ出し読み込みが発生する。
	mov			rax, qword ptr [rsp + 64 + 8 + 16]	; pSrcEnd
	cmp			rsi, rax
	jae			label6

	movdqu		xmm0, oword ptr [rsi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1

	movdqu		xmm4, oword ptr [rsi+rdx]
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

label5:
	movd		ecx, xmm0
	mov			byte ptr [rdi], cl
	movzx		ebp, cl
	inc			dword ptr [rbx+rbp*4]
	psrldq		xmm0, 1
	inc			rsi
	inc			rdi
	cmp			rsi, rax
	jb			label5


label6:

	pop			r15
	pop			r14
	pop			r13
	pop			r12
	pop			rdi
	pop			rsi
	pop			rbp
	pop			rbx
	ret

x64_sse2_PredictMedianAndCount_align1	endp


; p{min,max}ub は SSE1 で追加された MMX 命令（いわゆる MMX2 命令）である。
; void x64_sse1mmx_RestoreMedian_align1(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride)
x64_sse1mmx_RestoreMedian_align1	proc

	mov			[rsp +  8], rcx
	mov			[rsp + 16], rdx
	mov			[rsp + 24], r8
	mov			[rsp + 32], r9
	push		rbx
	push		rbp
	push		rsi
	push		rdi
	push		r12
	push		r13
	push		r14
	push		r15

	mov			rsi, qword ptr [rsp + 64 + 8 +  8]	; pSrcBegin
	mov			rdi, qword ptr [rsp + 64 + 8 +  0]	; pDst
	mov			rax, rsi
	mov			rbp, qword ptr [rsp + 64 + 8 + 24]	; dwStride
	add			rax, rbp
	neg			rbp

	mov			edx, 80h

	align		64
label1:
	add			dl, byte ptr [rsi]
	mov			byte ptr [rdi], dl
	inc 		rsi
	inc			rdi
	cmp			rsi, rax
	jb			label1

	pxor		mm4, mm4
	pxor		mm2, mm2

	align		64
label2:
	movq		mm6, mm2
	movq		mm7, mm2
	psubb		mm6, mm4
	movd		mm4, dword ptr [rdi+rbp]	; mm4 = above
	paddb		mm6, mm4					; mm6 = grad

	pminub		mm2, mm6
	pmaxub		mm6, mm7
	pmaxub		mm2, mm4
	pminub		mm2, mm6					; mm2 = median

	paddb		mm2, qword ptr [rsi]		; アライメントがずれていても xmm レジスタの場合と違って一般保護例外にはならない
	movd		eax, mm2
	mov			byte ptr [rdi], al

	inc			rsi
	inc			rdi
	cmp			rsi, qword ptr [rsp + 64 + 8 + 16]	; pSrcEnd
	jb			label2

	pop			r15
	pop			r14
	pop			r13
	pop			r12
	pop			rdi
	pop			rsi
	pop			rbp
	pop			rbx
	emms
	ret

x64_sse1mmx_RestoreMedian_align1	endp


end
