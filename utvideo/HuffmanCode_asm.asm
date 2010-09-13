; •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e
; $Id$

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
	mov			eax, edi			; •Ô‚è’l‚Í eax ‚ÉB
	sub			eax, dword ptr [esp + 16 + 4 +  0]	; pDstBegin

	pop			ebp
	pop			edi
	pop			esi
	pop			ebx
	ret

_i686_HuffmanEncode_align1	endp


HUFFMAN_DECODE	macro	procname, accum

; void procname(BYTE *pDstBegin, BYTE *pDstcEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable)
public	&procname
&procname	proc

	push		ebx
	push		esi
	push		edi
	push		ebp

	mov			esi, dword ptr [esp + 16 + 4 +  8]	; pSrcBegin
	mov			edi, dword ptr [esp + 16 + 4 +  0]	; pDstBegin
	mov			ebx, dword ptr [esp + 16 + 4 + 12]	; pDecodeTable
	mov			edx, dword ptr [esi+4]
	mov			ch, -32
if &accum
	mov			byte ptr [esp - 4], 80h
endif

	cmp			byte ptr [ebx + 8192+32+4*32+1], 0	; pDecodeTable->SymbolAndCodeLength[0].nCodeLength
	jne			label3

	; msmset(pDstBegin, pDecodeTable->dwSymbol[0], pDstEnd-pDstBegin);
	mov			eax, dword ptr [esp + 16 + 4 +  4]	; pDstEnd
	sub			eax, edi
	push		eax
	mov			eax, dword ptr [ebx + 8192+32+4*32]		; pDecodeTable->SymbolAndCodeLength[0].bySymbol
if &accum
	add			eax, 80h
	movzx		eax, al
endif
	push		eax
	push		edi
	call		_memset
	add			esp, 4*3

	jmp			label2

	align		64
label3:
	mov			ebp, dword ptr [esi]
label1:
	cmp			edi, dword ptr [esp + 16 + 4 +  4]	; pDstEnd
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
if &accum
	add			al, byte ptr [esp - 4]
	mov			byte ptr [esp - 4], al
endif
	mov			byte ptr [edi], al
	inc			edi
	add			ch, ah
	jnc			label1
	sub			ch, 32
	add			esi, 4
	mov			ebp, edx
	mov			edx, dword ptr [esi+4]
	jmp			label1

label2:
	pop			ebp
	pop			edi
	pop			esi
	pop			ebx
	ret

&procname	endp

		endm

HUFFMAN_DECODE	_i686_HuffmanDecode_align1, 0
HUFFMAN_DECODE	_i686_HuffmanDecodeAndAccum_align1, 1


end
