; •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e
; $Id$

include Common_asm_x64.inc

_TEXT_ASM	SEGMENT	page 'CODE'

; void x64_i686_HuffmanEncode(uint8_t *pDstBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable)
public	x64_i686_HuffmanEncode
x64_i686_HuffmanEncode	proc

	STD_PROLOG	0
$pDstBegin    = argsoffset +  0
$pSrcBegin    = argsoffset +  8
$pSrcEnd      = argsoffset + 16
$pEncodeTable = argsoffset + 24

	mov			rsi, qword ptr [rsp + $pSrcBegin]
	mov			rdi, qword ptr [rsp + $pDstBegin]
	mov			r8,  qword ptr [rsp + $pSrcEnd]
	mov			rdx, qword ptr [rsp + $pEncodeTable]
	cmp			qword ptr [rdx], 0
	je			label3

	mov			bl, -32
	mov			cl, 0

	align		64
label1:
	shld		eax, ecx, cl
	cmp			rsi, r8
	jnb			label4
	movzx		rcx, byte ptr [rsi]
	inc			rsi
	mov			ecx, dword ptr [rdx+rcx*4]
	add			bl, cl
	jnc			label1
	sub			cl, bl
	shld		eax, ecx, cl
	mov			dword ptr [rdi], eax
	add			rdi, 4
	add			cl, bl
	sub			bl, 32
	jmp			label1

label4:
	test		bl, 1fh
	jz			label3
	neg			bl
	mov			cl, bl
	shl			eax, cl
	mov			dword ptr [rdi], eax
	add			rdi, 4
label3:
	mov			rax, rdi
	sub			rax, qword ptr [rsp + $pDstBegin]

	STD_EPILOG
	ret

x64_i686_HuffmanEncode	endp


HUFFMAN_DECODE	macro	procname, accum, step, multiscan, bottomup, corrpos, dummyalpha

; void procname(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable)
; void procname(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, uint32_t dwNetWidth, uint32_t dwGrossWidth)
public	&procname
&procname	proc

	STD_PROLOG	0
$pDstBegin    = argsoffset +  0
$pDstEnd      = argsoffset +  8
$pSrcBegin    = argsoffset + 16
$pDecodeTable = argsoffset + 24
$dwNetWidth   = argsoffset + 32
$dwGrossWidth = argsoffset + 40

	mov			rsi, qword ptr [rsp + $pSrcBegin]
	mov			rbx, qword ptr [rsp + $pDecodeTable]
	mov			edx, dword ptr [rsi+4]
if &multiscan
 if &bottomup
	mov			rdi, qword ptr [rsp + $pDstEnd]
	sub			rdi, qword ptr [rsp + $dwGrossWidth]
	mov			r8, rdi
	add			r8, qword ptr [rsp + $dwNetWidth]
	mov			r12, qword ptr [rsp + $dwGrossWidth]
	add			r12, qword ptr [rsp + $dwNetWidth]
 else
	NOTIMPL
 endif
else
	mov			rdi, qword ptr [rsp + $pDstBegin]
	mov			r8,  qword ptr [rsp + $pDstEnd]
endif
	mov			cl, -32
	mov			ah, 32
if &accum
 if &corrpos ne 0
	mov			r11b, 00h
 else
	mov			r11b, 80h
 endif
endif
	mov			edx, dword ptr [rsi]
	sub			rsi, 4

	align		64
label1:
	cmp			rdi, r8
	jae			label2

	add			cl, ah
	jnc			label4
	sub			cl, 32
	add			rsi, 4
	mov			r9d, edx
	mov			edx, dword ptr [rsi+4]

label4:
	mov			eax, r9d
	shld		eax, edx, cl
	shr			eax, 20
	movzx		eax, word ptr [rbx + rax*2]
	cmp			ah, 255
	jne			label0

	mov			eax, r9d
	shld		eax, edx, cl
	mov			ch, cl
	or			eax, 1
	bsr			r10, rax
	mov			cl, byte ptr [rbx + 8192 + r10]					; pDecodeTable->nCodeShift[r10]
	shr			eax, cl
	mov			cl, ch
	mov			r10d, dword ptr [rbx + 8192+32 + r10*4]			; pDecodeTable->dwSymbolBase[r10]
	add			r10, rax
	mov			eax, dword ptr [rbx + 8192+32+4*32 + r10*2]		; pDecodeTable->SymbolAndCodeLength[r10]

label0:
if &accum
	add			al, r11b
	mov			r11b, al
endif
if &corrpos ne 0
	add			al, byte ptr [rdi + &corrpos]
endif
	mov			byte ptr [rdi], al
if &dummyalpha
	mov			byte ptr [rdi+1], 0ffh
endif
if &step eq 1
	inc			rdi
else
	add			rdi, &step
endif
	jmp			label1

label2:
if &multiscan
 if &bottomup
	sub			r8, qword ptr [rsp + $dwGrossWidth]
	cmp			r8, qword ptr [rsp + $pDstBegin]
	jbe			label3

	sub			rdi, r12
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

HUFFMAN_DECODE	x64_i686_HuffmanDecode, 0, 1, 0, 0, 0, 0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccum, 1, 1, 0, 0, 0, 0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep2, 1, 2, 0, 0, 0, 0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4, 1, 4, 0, 0, 0, 0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green, 1, 4, 1, 1, 0, 0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue, 1, 4, 1, 1, 1, 0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red, 1, 4, 1, 1, -1, 0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha, 1, 4, 1, 1, -1, 1
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green, 1, 3, 1, 1, 0, 0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue, 1, 3, 1, 1, 1, 0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red, 1, 3, 1, 1, -1, 0


end
