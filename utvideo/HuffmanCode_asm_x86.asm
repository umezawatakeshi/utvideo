; •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e
; $Id$

.686
.no87
.xmm

.model	flat


_TEXT_ASM	SEGMENT	page public flat 'CODE'

; void x86_i686_HuffmanEncode(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable)
public	_x86_i686_HuffmanEncode
_x86_i686_HuffmanEncode	proc

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
	mov			eax, edi
	sub			eax, dword ptr [esp + 16 + 4 +  0]	; pDstBegin

	pop			ebp
	pop			edi
	pop			esi
	pop			ebx
	ret

_x86_i686_HuffmanEncode	endp


HUFFMAN_DECODE	macro	procname, accum, step, multiscan, bottomup, corrpos, dummyalpha

; void procname(BYTE *pDstBegin, BYTE *pDstcEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable)
; void procname(BYTE *pDstBegin, BYTE *pDstcEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, DWORD dwNetWidth, DWORD dwGrossWidth)
public	&procname
&procname	proc

	push		ebx
	push		esi
	push		edi
	push		ebp

	mov			esi, dword ptr [esp + 16 + 4 +  8]	; pSrcBegin
	mov			ebx, dword ptr [esp + 16 + 4 + 12]	; pDecodeTable
	mov			edx, dword ptr [esi+4]
if &multiscan
 if &bottomup
	mov			edi, dword ptr [esp + 16 + 4 +  4]	; pDstEnd
	sub			edi, dword ptr [esp + 16 + 4 + 20]	; dwGrossWidth
	mov			eax, edi
	add			eax, dword ptr [esp + 16 + 4 + 16]	; dwNetWidth
	mov			dword ptr [esp - 8], eax
	mov			eax, dword ptr [esp + 16 + 4 + 20]	; dwGrossWidth
	add			eax, dword ptr [esp + 16 + 4 + 16]	; dwNetWidth
	mov			dword ptr [esp - 12], eax
 else
	NOTIMPL
 endif
else
	mov			edi, dword ptr [esp + 16 + 4 +  0]	; pDstBegin
	mov			eax, dword ptr [esp + 16 + 4 +  4]	; pDstEnd
	mov			dword ptr [esp - 8], eax
endif
	mov			ch, -32
if &accum
 if &corrpos ne 0
	mov			byte ptr [esp - 4], 00h
 else
	mov			byte ptr [esp - 4], 80h
 endif
endif
	mov			ebp, dword ptr [esi]

	align		64
label1:
	cmp			edi, dword ptr [esp - 8]	; pDstEnd
	jae			label2
	mov			eax, ebp
	mov			cl, ch

	shld		eax, edx, cl
	shr			eax, 20
	movzx		eax, word ptr [ebx + eax*2]
	cmp			ah, 255
	jne			label0

	mov			eax, dword ptr [esi]
	shld		eax, edx, cl
	or			eax, 1
	bsr			ebp, eax
	mov			cl, byte ptr [ebx + 8192 + ebp]					; pDecodeTable->nCodeShift[ebp]
	shr			eax, cl
	mov			ebp, dword ptr [ebx + 8192+32 + ebp*4]			; pDecodeTable->dwSymbolBase[ebp]
	add			ebp, eax
	mov			eax, dword ptr [ebx + 8192+32+4*32 + ebp*2]		; pDecodeTable->SymbolAndCodeLength[ebp]
	mov			ebp, dword ptr [esi]

label0:
if &dummyalpha
	add			ch, ah
endif
if &accum
	add			al, byte ptr [esp - 4]
	mov			byte ptr [esp - 4], al
endif
if &corrpos ne 0
	add			al, byte ptr [edi + &corrpos]
endif
if &dummyalpha
	movzx		ax, al
	or			ax, 0ff00h
	mov			word ptr [edi], ax
else
	mov			byte ptr [edi], al
endif
if &step eq 1
	inc			edi
else
	add			edi, &step
endif
if &dummyalpha
	test		ch, 80h
	jnz			label1
else
	add			ch, ah
	jnc			label1
endif
	sub			ch, 32
	add			esi, 4
	mov			ebp, edx
	mov			edx, dword ptr [esi+4]
	jmp			label1

label2:
if &multiscan
 if &bottomup
	mov			eax, dword ptr [esp - 8]
	sub			eax, dword ptr [esp + 16 + 4 + 20]
	cmp			eax, dword ptr [esp + 16 + 4 +  0]
	jbe			label3
	mov			dword ptr [esp - 8], eax

	sub			edi, dword ptr [esp -12]
 else
	NOTIMPL
 endif
	jmp			label1
label3:
endif
	pop			ebp
	pop			edi
	pop			esi
	pop			ebx
	ret

&procname	endp

		endm

HUFFMAN_DECODE	_x86_i686_HuffmanDecode, 0, 1, 0, 0, 0, 0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccum, 1, 1, 0, 0, 0, 0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep2, 1, 2, 0, 0, 0, 0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep4, 1, 4, 0, 0, 0, 0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green, 1, 4, 1, 1, 0, 0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue, 1, 4, 1, 1, 1, 0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red, 1, 4, 1, 1, -1, 0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha, 1, 4, 1, 1, -1, 1
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green, 1, 3, 1, 1, 0, 0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue, 1, 3, 1, 1, 1, 0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red, 1, 3, 1, 1, -1, 0


end
