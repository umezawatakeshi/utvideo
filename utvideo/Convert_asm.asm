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

	align	64
;			 fedcba9876543210
yoff	dq	00010001000100010h
		dq	00000000000000000h
uvoff	dq	00080008000800080h
		dq	00000000000000000h

y2rgb	dq	000004A8500004A85h
		dq	000004A8500004A85h
uv2g	dq	0E5FCF377E5FCF377h
		dq	0E5FCF377E5FCF377h
uv2r	dq	03313000033130000h
		dq	03313000033130000h
uv2b	dq	00000408D0000408Dh
		dq	00000408D0000408Dh

; sse2_ConvertULY2ToBottomupRGB32(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, DWORD dwStride);
public	_sse2_ConvertULY2ToBottomupRGB32
_sse2_ConvertULY2ToBottomupRGB32	proc

	push		ebx
	push		esi
	push		edi
	push		ebp

	mov			eax, dword ptr [esp + 16 + 4 +  8]	; pYBegin
	mov			ebx, dword ptr [esp + 16 + 4 + 12]	; pUBegin
	mov			ecx, dword ptr [esp + 16 + 4 + 16]	; pVBegin
	mov			edx, dword ptr [esp + 16 + 4 + 20]	; dwStride
	mov			esi, dword ptr [esp + 16 + 4 +  4]	; pDstEnd	; esi なのに dst のポインタを保持するのは気持ちが悪いが。

	align	64
label0:
	mov			edi, esi
	sub			edi, edx

	; align	64	; さすがに入れすぎな気がするのでコメントアウト。
label1:
	pxor		xmm3, xmm3				; xmm3 = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

	movd		xmm0, dword ptr [eax]	; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 Y3 Y2 Y1 Y0
	punpcklbw	xmm0, xmm3				; xmm0 = 00 00 00 00 00 00 00 00 00 Y3 00 Y2 00 Y1 00 Y0
	psubw		xmm0, oword ptr [yoff]	; xmm0 = 00 00 00 00 00 00 00 00 ---Y3 ---Y2 ---Y1 ---Y0 (de-offset)
	punpcklwd	xmm0, xmm3				; xmm0 = 00 00 ---Y3 00 00 ---Y2 00 00 ---Y1 00 00 ---Y0 (de-offset)

	movd		xmm1, dword ptr [ebx]	; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 U6 U4 U2 U0
	movd		xmm2, dword ptr [ecx]	; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 V6 V4 V2 V0
	punpcklbw	xmm1, xmm2				; xmm1 = 00 00 00 00 00 00 00 00 V6 U6 V4 U4 V2 U2 V0 U0
	punpcklbw	xmm1, xmm3				; xmm1 = 00 V6 00 U6 00 V4 00 U4 00 V2 00 U2 00 V0 00 U0
	psubw		xmm1, oword ptr [uvoff]	; xmm1 = 00 V6 00 U6 00 V4 00 U4 ---V2 ---U2 ---V0 ---U0 (de-offset)
	punpckldq	xmm1, xmm1				; xmm1 = ---V2 ---U2 ---V2 ---U2 ---V0 ---U0 ---V0 ---U0 (de-offset)
	paddw		xmm1, xmm1

	pmaddwd		xmm0, oword ptr [y2rgb]

	movdqa		xmm3, xmm1
	pmaddwd		xmm3, oword ptr [uv2r]
	paddd		xmm3, xmm0				; xmm3 = -R3-------- -R2-------- -R1-------- -R0--------
	psrad		xmm3, 14				; xmm3 = ---------R3 ---------R2 ---------R1 ---------R0

	movdqa		xmm2, xmm1
	pmaddwd		xmm2, oword ptr [uv2b]
	paddd		xmm2, xmm0				; xmm2 = -B3-------- -B2-------- -B1-------- -B0--------
	psrad		xmm2, 14				; xmm2 = ---------B3 ---------B2 ---------B1 ---------B0

	pmaddwd		xmm1, oword ptr [uv2g]
	paddd		xmm1, xmm0				; xmm1 = -G3-------- -G2-------- -G1-------- -G0--------
	psrad		xmm1, 14				; xmm1 = ---------G3 ---------G2 ---------G1 ---------G0

	pxor		xmm0, xmm0
	packssdw	xmm1, xmm3				; xmm1 = ---R3 ---R2 ---R1 ---R0 ---G3 ---G2 ---G1 ---G0
	packssdw	xmm2, xmm0				; xmm2 = 00 00 00 00 00 00 00 00 ---B3 ---B2 ---B1 ---B0
	pmaxsw		xmm1, xmm0				; 計算過程でマイナスになることがあるので、
	pmaxsw		xmm2, xmm0				; ここの pmaxsw xmmN, xmm0 は必要。
	punpcklwd	xmm2, xmm1				; xmm2 = ---G3 ---B3 ---G2 ---B2 ---G1 ---B1 ---G0 ---B0
	punpckhwd	xmm1, xmm0				; xmm1 = 00 00 ---R3 00 00 ---R2 00 00 ---R1 00 00 ---R0
	packuswb	xmm2, xmm2				; xmm2 = XX XX XX XX XX XX XX XX G3 B3 G2 B2 G1 B1 G0 B0
	packuswb	xmm1, xmm1				; xmm2 = XX XX XX XX XX XX XX XX 00 R3 00 R2 00 R1 00 R0
	punpcklwd	xmm2, xmm1				; xmm2 = 00 R3 G3 B3 00 R2 G2 B2 00 R1 G1 B1 00 R0 G0 B0

	add			edi, 16
	add			eax, 4
	add			ebx, 2
	add			ecx, 2
	cmp			edi, esi
	ja			label2
	movdqu		oword ptr [edi-16], xmm2
	jne			label1
	jmp			label3

label2:
	sub			edi, 16
	sub			eax, 4
	sub			ebx, 2
	sub			ecx, 2
	test		edx, 8
	jz			label4
	movq		qword ptr [edi], xmm2
	psrldq		xmm2, 8
	add			edi, 8
	add			eax, 2
	add			ebx, 1
	add			ecx, 1
label4:
	test		edx, 4
	jz			label3
	movd		dword ptr [edi], xmm2
	add			edi, 4
	add			eax, 2
	add			ebx, 1
	add			ecx, 1

label3:
	sub			esi, edx
	cmp			esi, dword ptr [esp + 16 + 4 +  0]	; pDstBegin
	ja			label0

	pop			ebp
	pop			edi
	pop			esi
	pop			ebx
	ret

_sse2_ConvertULY2ToBottomUpRGB32	endp

end
