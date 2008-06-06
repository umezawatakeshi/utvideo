; 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ
; $Id$

; Ut Video Codec Suite
; Copyright (C) 2008  UMEZAWA Takeshi
; 
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
; 
; 
; Ut Video Codec Suite
; Copyright (C) 2008  梅澤 威志
; 
; このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフ
; トウェア財団によって発行された GNU 一般公衆利用許諾契約書(バージョ
; ン2か、希望によってはそれ以降のバージョンのうちどれか)の定める条件
; の下で再頒布または改変することができます。
; 
; このプログラムは有用であることを願って頒布されますが、*全くの無保
; 証* です。商業可能性の保証や特定の目的への適合性は、言外に示された
; ものも含め全く存在しません。詳しくはGNU 一般公衆利用許諾契約書をご
; 覧ください。
; 
; あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を
; 一部受け取ったはずです。もし受け取っていなければ、フリーソフトウェ
; ア財団まで請求してください(宛先は the Free Software Foundation,
; Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA)。

.686
.no87
.xmm

.model	flat

_TEXT_ASM	SEGMENT	page public flat 'CODE'

; 似た関数が増えてきた…マクロ技で統合せねば…


; void sse2_PredictMedian_align16(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride)
public	_sse2_PredictMedian_align16
_sse2_PredictMedian_align16	proc

	push		ebx
	push		esi
	push		edi
	push		ebp

	mov			eax, 80h
	movd		xmm1, eax

	mov			esi, dword ptr [esp + 16 + 4 +  4]	; pSrcBegin
	mov			edi, dword ptr [esp + 16 + 4 +  0]	; pDst
	mov			eax, esi
	mov			edx, dword ptr [esp + 16 + 4 + 12]	; dwStride
	add			eax, edx
	neg			edx

	align		64
label1:
	movdqa		xmm0, oword ptr [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	psubb		xmm0, xmm2
	movdqa		oword ptr [edi], xmm0

	add			esi, 16
	add			edi, 16
	cmp			esi, eax
	jb			label1

	mov			eax, dword ptr [esp + 16 + 4 +  8]	; pSrcEnd

	pxor		xmm1, xmm1
	pxor		xmm5, xmm5

	align		64
label2:
	movdqa		xmm0, oword ptr [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	movdqa		xmm4, oword ptr [esi+edx]
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
	movdqa		oword ptr [edi], xmm0

	add			esi, 16
	add			edi, 16
	cmp			esi, eax
	jb			label2

	pop			ebp
	pop			edi
	pop			esi
	pop			ebx
	ret

_sse2_PredictMedian_align16	endp


; void sse2_PredictMedianAndCount_align16(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride, DWORD *pCountTable)
public	_sse2_PredictMedianAndCount_align16
_sse2_PredictMedianAndCount_align16	proc

	push		ebx
	push		esi
	push		edi
	push		ebp

	mov			eax, 80h
	movd		xmm1, eax

	mov			esi, dword ptr [esp + 16 + 4 +  4]	; pSrcBegin
	mov			edi, dword ptr [esp + 16 + 4 +  0]	; pDst
	mov			eax, esi
	mov			edx, dword ptr [esp + 16 + 4 + 12]	; dwStride
	add			eax, edx
	neg			edx
	mov			ebx, dword ptr [esp + 16 + 4 + 16]	; pCountTable

	align		64
label1:
	movdqa		xmm0, oword ptr [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	psubb		xmm0, xmm2
	movdqa		oword ptr [edi], xmm0

	pextrw		ecx, xmm0, 0
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 1
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 2
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 3
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 4
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 5
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 6
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 7
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]

	add			esi, 16
	add			edi, 16
	cmp			esi, eax
	jb			label1

	mov			eax, dword ptr [esp + 16 + 4 +  8]	; pSrcEnd

	pxor		xmm1, xmm1
	pxor		xmm5, xmm5

	align		64
label2:
	movdqa		xmm0, oword ptr [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	movdqa		xmm4, oword ptr [esi+edx]
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
	movdqa		oword ptr [edi], xmm0

	pextrw		ecx, xmm0, 0
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 1
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 2
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 3
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 4
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 5
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 6
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 7
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]

	add			esi, 16
	add			edi, 16
	cmp			esi, eax
	jb			label2

	pop			ebp
	pop			edi
	pop			esi
	pop			ebx
	ret

_sse2_PredictMedianAndCount_align16	endp


; prediction は前後にマージンを持つ CFrameBuffer 上で行うので、計算結果が変わらない限り、はみ出し読み込みは許容される。
; 一方、マルチスレッド動作した時に問題が発生するので、はみ出し書き込みは許容されない。

; void sse2_PredictMedianAndCount_align1(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride, DWORD *pCountTable)
public	_sse2_PredictMedianAndCount_align1
_sse2_PredictMedianAndCount_align1	proc

	push		ebx
	push		esi
	push		edi
	push		ebp

	mov			eax, 80h
	movd		xmm1, eax

	mov			esi, dword ptr [esp + 16 + 4 +  4]	; pSrcBegin
	mov			edi, dword ptr [esp + 16 + 4 +  0]	; pDst
	mov			eax, esi
	mov			edx, dword ptr [esp + 16 + 4 + 12]	; dwStride
	and			edx, 0fffffff0h
	add			eax, edx
	mov			ebx, dword ptr [esp + 16 + 4 + 16]	; pCountTable

	; 最初のラインを16バイトずつ処理する。
	align		64
label1:
	movdqu		xmm0, oword ptr [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	psubb		xmm0, xmm2
	movdqu		oword ptr [edi], xmm0

	pextrw		ecx, xmm0, 0
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 1
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 2
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 3
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 4
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 5
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 6
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 7
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]

	add			esi, 16
	add			edi, 16
	cmp			esi, eax
	jb			label1

	; 最初のラインの16バイトに満たない部分を処理する。
	; 若干のはみ出し読み込みが発生する。
	mov			eax, dword ptr [esp + 16 + 4 +  4]	; pSrcBegin
	add			eax, dword ptr [esp + 16 + 4 + 12]	; dwStride
	cmp			esi, eax
	jae			label4

	movdqu		xmm0, oword ptr [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	psubb		xmm0, xmm2

label3:
	movd		ecx, xmm0
	mov			byte ptr [edi], cl
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	psrldq		xmm0, 1
	inc			esi
	inc			edi
	cmp			esi, eax
	jb			label3

	; 残りのラインを16バイトずつ処理する。
label4:
	mov			edx, dword ptr [esp + 16 + 4 + 12]	; dwStride
	neg			edx
	mov			eax, dword ptr [esp + 16 + 4 +  8]	; pSrcEnd
	sub			eax, esi
	and			eax, 0fffffff0h
	add			eax, esi

	pxor		xmm1, xmm1
	pxor		xmm5, xmm5

	align		64
label2:
	movdqu		xmm0, oword ptr [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1
	movdqa		xmm1, xmm0
	psrldq		xmm1, 15

	movdqu		xmm4, oword ptr [esi+edx]
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
	movdqu		oword ptr [edi], xmm0

	pextrw		ecx, xmm0, 0
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 1
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 2
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 3
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 4
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 5
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 6
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]
	pextrw		ecx, xmm0, 7
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	movzx		ebp, ch
	inc			dword ptr [ebx+ebp*4]

	add			esi, 16
	add			edi, 16
	cmp			esi, eax
	jb			label2

	; 残りのラインの16バイトに満たない部分を処理する。
	; 若干のはみ出し読み込みが発生する。
	mov			eax, dword ptr [esp + 16 + 4 +  8]	; pSrcEnd
	cmp			esi, eax
	jae			label6

	movdqu		xmm0, oword ptr [esi]
	movdqa		xmm2, xmm0
	pslldq		xmm2, 1
	por			xmm2, xmm1

	movdqu		xmm4, oword ptr [esi+edx]
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
	mov			byte ptr [edi], cl
	movzx		ebp, cl
	inc			dword ptr [ebx+ebp*4]
	psrldq		xmm0, 1
	inc			esi
	inc			edi
	cmp			esi, eax
	jb			label5


label6:
	pop			ebp
	pop			edi
	pop			esi
	pop			ebx
	ret

_sse2_PredictMedianAndCount_align1	endp


; void i686_RestoreMedian_align1(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride)
_i686_RestoreMedian_align1	proc

	push		ebx
	push		esi
	push		edi
	push		ebp

	mov			esi, dword ptr [esp + 16 + 4 +  4]	; pSrcBegin
	mov			edi, dword ptr [esp + 16 + 4 +  0]	; pDst
	mov			eax, esi
	mov			ebp, dword ptr [esp + 16 + 4 + 12]	; dwStride
	add			eax, ebp
	neg			ebp

	mov			edx, 80h

	align		64
label1:
	add			dl, byte ptr [esi]
	mov			byte ptr [edi], dl
	inc 		esi
	inc			edi
	cmp			esi, eax
	jb			label1

	xor			ecx, ecx
	xor			edx, edx

	align		64
label2:
	mov			ebx, edx
	sub			ebx, ecx
	movzx		ecx, byte ptr [edi+ebp]
	add			ebx, ecx	; bl = grad

	mov			eax, edx
	cmp			dl, cl
	cmovae		edx, ecx	; dl = min(dl,cl)
	cmovb		eax, ecx	; al = max(dl,cl) ; ebx の上位 16bit は保護する必要があるので、ここは cmovb ebx, ecx ではいけない。また、cmov は 8bit オペランドをサポートしないので、そもそも cmovb bl, cl とは書けない。

	cmp			dl, bl
	cmovb		edx, ebx	; dl = max(dl,bl)
	cmp			dl, al
	cmovae		edx, eax	; dl = min(dl,al)

	movzx		ebx, byte ptr [esi]
	add			edx, ebx
	mov			byte ptr [edi], dl

	inc			esi
	inc			edi
	cmp			esi, dword ptr [esp + 16 + 4 +  8]	; pSrcEnd
	jb			label2

	pop			ebp
	pop			edi
	pop			esi
	pop			ebx
	ret

_i686_RestoreMedian_align1	endp


; p{min,max}ub は SSE1 で追加された MMX 命令（いわゆる MMX2 命令）である。
; void sse1mmx_RestoreMedian_align1(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride)
_sse1mmx_RestoreMedian_align1	proc

	push		ebx
	push		esi
	push		edi
	push		ebp

	mov			esi, dword ptr [esp + 16 + 4 +  4]	; pSrcBegin
	mov			edi, dword ptr [esp + 16 + 4 +  0]	; pDst
	mov			eax, esi
	mov			ebp, dword ptr [esp + 16 + 4 + 12]	; dwStride
	add			eax, ebp
	neg			ebp

	mov			edx, 80h

	align		64
label1:
	add			dl, byte ptr [esi]
	mov			byte ptr [edi], dl
	inc 		esi
	inc			edi
	cmp			esi, eax
	jb			label1

	pxor		mm4, mm4
	pxor		mm2, mm2

	align		64
label2:
	movq		mm6, mm2
	movq		mm7, mm2
	psubb		mm6, mm4
	movd		mm4, dword ptr [edi+ebp]	; mm4 = above
	paddb		mm6, mm4	; mm6 = grad

	pminub		mm2, mm6
	pmaxub		mm6, mm7
	pmaxub		mm2, mm4
	pminub		mm2, mm6	; mm2 = median

	paddb		mm2, qword ptr [esi]
	movd		eax, mm2
	mov			byte ptr [edi], al

	inc			esi
	inc			edi
	cmp			esi, dword ptr [esp + 16 + 4 +  8]	; pSrcEnd
	jb			label2

	pop			ebp
	pop			edi
	pop			esi
	pop			ebx
	emms
	ret

_sse1mmx_RestoreMedian_align1	endp


end
