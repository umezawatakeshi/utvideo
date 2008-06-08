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

extrn	_memset:PROC


_TEXT_ASM	SEGMENT	page public flat 'CODE'

; void i686_HuffmanEncode_align1(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable)
public	_i686_HuffmanEncode_align1
_i686_HuffmanEncode_align1	proc

	push		ebx
	push		esi
	push		edi
	push		ebp

	mov			esi, dword ptr [esp + 16 + 4 +  4]	; pSrcBegin
	mov			edi, dword ptr [esp + 16 + 4 +  0]	; pDstBegin
	mov			edx, dword ptr [esp + 16 + 4 +  8]	; pSrcEnd
	mov			ebp, dword ptr [esp + 16 + 4 + 12]	; pEncodeTable
	cmp			dword ptr [ebp], 0
	je			label3

	mov			bl, -32
	mov			cl, 0

	align		64
label1:
	shld		eax, ecx, cl
	cmp			esi, edx
	jnb			label4
	movzx		ecx, byte ptr [esi]
	inc			esi
	mov			ecx, dword ptr [ebp+ecx*4]
	add			bl, cl
	jnc			label1
	sub			cl, bl
	shld		eax, ecx, cl
	mov			dword ptr [edi], eax
	add			edi, 4
	add			cl, bl
	sub			bl, 32
	jmp			label1

label4:
	test		bl, 1fh
	jz			label3
	neg			bl
	mov			cl, bl
	shl			eax, cl
	mov			dword ptr [edi], eax
	add			edi, 4
label3:
	mov			eax, edi			; 返り値は eax に。
	sub			eax, dword ptr [esp + 16 + 4 +  0]	; pDstBegin

	pop			ebp
	pop			edi
	pop			esi
	pop			ebx
	ret

_i686_HuffmanEncode_align1	endp


; void i686_HuffmanDecode_align1(BYTE *pDstBegin, BYTE *pDstcEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable)
public	_i686_HuffmanDecode_align1
_i686_HuffmanDecode_align1	proc

	push		ebx
	push		esi
	push		edi
	push		ebp

	mov			esi, dword ptr [esp + 16 + 4 +  8]	; pSrcBegin
	mov			edi, dword ptr [esp + 16 + 4 +  0]	; pDstBegin
	mov			ebx, dword ptr [esp + 16 + 4 + 12]	; pDecodeTable
	mov			edx, dword ptr [esi+4]
	mov			ch, -32

	cmp			byte ptr [ebx + 32+4*32+1], 0	; pDecodeTable->SymbolAndCodeLength[0].nCodeLength
	jne			label1

	; msmset(pDstBegin, pDecodeTable->dwSymbol[0], pDstEnd-pDstBegin);
	mov			eax, dword ptr [esp + 16 + 4 +  4]	; pDstEnd
	sub			eax, edi
	push		eax
	mov			eax, dword ptr [ebx + 32+4*32]		; pDecodeTable->SymbolAndCodeLength[0].bySymbol
	push		eax
	push		edi
	call		_memset
	add			esp, 4*3

	jmp			label2

	align		64
label1:
	cmp			edi, dword ptr [esp + 16 + 4 +  4]	; pDstEnd
	jae			label2
	mov			eax, dword ptr [esi]
	mov			cl, ch
	shld		eax, edx, cl
	or			eax, 1
	bsr			ebp, eax
	mov			cl, byte ptr [ebx + ebp]					; pDecodeTable->nCodeShift[ebp]
	shr			eax, cl
	mov			ebp, dword ptr [ebx + 32 + ebp*4]			; pDecodeTable->dwSymbolBase[ebp]
	add			ebp, eax
	mov			eax, dword ptr [ebx + 32+4*32 + ebp*4]		; pDecodeTable->SymbolAndCodeLength[ebp]
	mov			byte ptr [edi], al
	inc			edi
	add			ch, ah
	jnc			label1
	sub			ch, 32
	add			esi, 4
	mov			edx, dword ptr [esi+4]
	jmp			label1

label2:
	pop			ebp
	pop			edi
	pop			esi
	pop			ebx
	ret

_i686_HuffmanDecode_align1	endp


end
