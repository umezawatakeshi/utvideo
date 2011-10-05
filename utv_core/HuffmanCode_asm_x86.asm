; •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e
; $Id$

include Common_asm_x86.inc

.686
.no87
.xmm

.model	flat

_TEXT_ASM	SEGMENT	page public flat 'CODE'

; void x86_i686_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable)
public	_x86_i686_HuffmanEncode
_x86_i686_HuffmanEncode	proc

	STD_PROLOG	0
$pDstBegin    = argsoffset
$pSrcBegin    = argsoffset +  4
$pSrcEnd      = argsoffset +  8
$pEncodeTable = argsoffset + 12

	mov			esi, dword ptr [esp + $pSrcBegin]
	mov			edi, dword ptr [esp + $pDstBegin]
	mov			edx, dword ptr [esp + $pSrcEnd]
	mov			ebp, dword ptr [esp + $pEncodeTable]
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
	sub			eax, dword ptr [esp + $pDstBegin]

	STD_EPILOG
	ret

_x86_i686_HuffmanEncode	endp


HUFFMAN_DECODE	macro	procname, accum, step, multiscan, bottomup, corrpos, dummyalpha

; void procname(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable)
; void procname(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, uint32_t dwNetWidth, uint32_t dwGrossWidth)
public	&procname
&procname	proc

	STD_PROLOG	12
$pDstBegin    = argsoffset
$pDstEnd      = argsoffset +  4
$pSrcBegin    = argsoffset +  8
$pDecodeTable = argsoffset + 12
$dwNetWidth   = argsoffset + 16
$dwGrossWidth = argsoffset + 20
$byCorrBuf    =  0
$pLineEnd     =  4
$dwLineOffset =  8

	mov			esi, dword ptr [esp + $pSrcBegin]
	mov			ebx, dword ptr [esp + $pDecodeTable]
if &multiscan
 if &bottomup
	mov			edi, dword ptr [esp + $pDstEnd]
	sub			edi, dword ptr [esp + $dwGrossWidth]
	mov			eax, edi
	add			eax, dword ptr [esp + $dwNetWidth]
	mov			dword ptr [esp + $pLineEnd], eax
	mov			eax, dword ptr [esp + $dwGrossWidth]
	add			eax, dword ptr [esp + $dwNetWidth]
	mov			dword ptr [esp + $dwLineOffset], eax
 else
	NOTIMPL
 endif
else
	mov			edi, dword ptr [esp + $pDstBegin]
	mov			eax, dword ptr [esp + $pDstEnd]
	mov			dword ptr [esp + $pLineEnd], eax
endif
if &accum
 if &corrpos ne 0
	mov			byte ptr [esp + $byCorrBuf], 00h
 else
	mov			byte ptr [esp + $byCorrBuf], 80h
 endif
endif
	mov			cl, -32
	mov			ah, 32
	mov			edx, dword ptr [esi]
	sub			esi, 4

	align		64
label1:
	cmp			edi, dword ptr [esp + $pLineEnd]
	jae			label2

	add			cl, ah
	jnc			label4
	sub			cl, 32
	add			esi, 4
	mov			ebp, edx
	mov			edx, dword ptr [esi+4]

label4:
	mov			eax, ebp
	shld		eax, edx, cl
	shr			eax, 20
	movzx		eax, word ptr [ebx + eax*2]
	cmp			ah, 255
	jne			label0

	mov			eax, ebp
	shld		eax, edx, cl
	mov			ch, cl
	or			eax, 1
	bsr			ebp, eax
	mov			cl, byte ptr [ebx + 8192 + ebp]					; pDecodeTable->nCodeShift[ebp]
	shr			eax, cl
	mov			cl, ch
	mov			ebp, dword ptr [ebx + 8192+32 + ebp*4]			; pDecodeTable->dwSymbolBase[ebp]
	add			ebp, eax
	mov			eax, dword ptr [ebx + 8192+32+4*32 + ebp*2]		; pDecodeTable->SymbolAndCodeLength[ebp]
	mov			ebp, dword ptr [esi]

label0:
if &accum
	add			al, byte ptr [esp + $byCorrBuf]
	mov			byte ptr [esp + $byCorrBuf], al
endif
if &corrpos ne 0
	add			al, byte ptr [edi + &corrpos]
endif
	mov			byte ptr [edi], al
if &dummyalpha
	mov			byte ptr [edi+1], 0ffh
endif
if &step eq 1
	inc			edi
else
	add			edi, &step
endif
	jmp			label1

label2:
if &multiscan
 if &bottomup
	mov			edx, dword ptr [esp + $pLineEnd]
	sub			edx, dword ptr [esp + $dwGrossWidth]
	cmp			edx, dword ptr [esp + $pDstBegin]
	jbe			label3
	mov			dword ptr [esp + $pLineEnd], edx
	mov			edx, dword ptr [esi+4]

	sub			edi, dword ptr [esp + $dwLineOffset]
 else
	NOTIMPL
 endif
	jmp			label1
label3:
endif
	STD_EPILOG
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
