; �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e
; $Id$


_TEXT_ASM	SEGMENT	page 'CODE'

; void x64_i686_HuffmanEncode(BYTE *pDstBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, const HUFFMAN_ENCODE_TABLE *pEncodeTable)
public	x64_i686_HuffmanEncode
x64_i686_HuffmanEncode	proc

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
	mov			rdi, qword ptr [rsp + 64 + 8 +  0]	; pDstBegin
	mov			r8,  qword ptr [rsp + 64 + 8 + 16]	; pSrcEnd
	mov			rdx, qword ptr [rsp + 64 + 8 + 24]	; pEncodeTable
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
	sub			rax, qword ptr [rsp + 64 + 8 +  0]	; pDstBegin

	pop			r15
	pop			r14
	pop			r13
	pop			r12
	pop			rdi
	pop			rsi
	pop			rbp
	pop			rbx
	ret

x64_i686_HuffmanEncode	endp


HUFFMAN_DECODE	macro	procname, accum, step, multiscan, bottomup, corrpos, dummyalpha

; void procname(BYTE *pDstBegin, BYTE *pDstcEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable)
; void procname(BYTE *pDstBegin, BYTE *pDstcEnd, const BYTE *pSrcBegin, const HUFFMAN_DECODE_TABLE *pDecodeTable, DWORD dwNetWidth, DWORD dwGrossWidth)
public	&procname
&procname	proc

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

	mov			rsi, qword ptr [rsp + 64 + 8 + 16]	; pSrcBegin
	mov			rbx, qword ptr [rsp + 64 + 8 + 24]	; pDecodeTable
	mov			edx, dword ptr [rsi+4]
;if &multiscan
; if &bottomup
;	mov			edi, dword ptr [esp + 16 + 4 +  4]	; pDstEnd
;	sub			edi, dword ptr [esp + 16 + 4 + 20]	; dwGrossWidth
;	mov			eax, edi
;	add			eax, dword ptr [esp + 16 + 4 + 16]	; dwNetWidth
;	mov			dword ptr [esp - 8], eax
;	mov			eax, dword ptr [esp + 16 + 4 + 20]	; dwGrossWidth
;	add			eax, dword ptr [esp + 16 + 4 + 16]	; dwNetWidth
;	mov			dword ptr [esp - 12], eax
; else
;	NOTIMPL
; endif
;else
	mov			rdi, qword ptr [rsp + 64 + 8 +  0]	; pDstBegin
	mov			r8,  qword ptr [rsp + 64 + 8 +  8]	; pDstEnd
;endif
	mov			ch, -32
if &accum
; if &corrpos ne 0
;	mov			byte ptr [esp - 4], 00h
; else
	mov			r11b, 80h
; endif
endif
	mov			r9d, dword ptr [rsi]

	align		64
label1:
	cmp			rdi, r8
	jae			label2
	mov			eax, r9d
	mov			cl, ch

	shld		eax, edx, cl
	shr			eax, 20
	movzx		eax, word ptr [rbx + rax*2]
	cmp			ah, 255
	jne			label0

	mov			eax, r9d
	shld		eax, edx, cl
	or			eax, 1
	bsr			r10, rax
	mov			cl, byte ptr [rbx + 8192 + r10]					; pDecodeTable->nCodeShift[r10]
	shr			eax, cl
	mov			r10d, dword ptr [rbx + 8192+32 + r10*4]			; pDecodeTable->dwSymbolBase[r10]
	add			r10, rax
	mov			eax, dword ptr [rbx + 8192+32+4*32 + r10*2]		; pDecodeTable->SymbolAndCodeLength[r10]

label0:
;if &dummyalpha
;	add			ch, ah
;endif
if &accum
	add			al, r11b
	mov			r11b, al
endif
;if &corrpos ne 0
;	add			al, byte ptr [edi + &corrpos]
;endif
;if &dummyalpha
;	movzx		ax, al
;	or			ax, 0ff00h
;	mov			word ptr [edi], ax
;else
	mov			byte ptr [rdi], al
;endif
;if &step eq 1
	inc			rdi
;else
;	add			edi, &step
;endif
;if &dummyalpha
;	test		ch, 80h
;	jnz			label1
;else
	add			ch, ah
	jnc			label1
;endif
	sub			ch, 32
	add			rsi, 4
	mov			r9d, edx
	mov			edx, dword ptr [rsi+4]
	jmp			label1

label2:
;if &multiscan
; if &bottomup
;	mov			eax, dword ptr [esp - 8]
;	sub			eax, dword ptr [esp + 16 + 4 + 20]
;	cmp			eax, dword ptr [esp + 16 + 4 +  0]
;	jbe			label3
;	mov			dword ptr [esp - 8], eax
;
;	sub			edi, dword ptr [esp -12]
; else
;	NOTIMPL
; endif
;	jmp			label1
;label3:
;endif

	pop			r15
	pop			r14
	pop			r13
	pop			r12
	pop			rdi
	pop			rsi
	pop			rbp
	pop			rbx
	ret

&procname	endp

		endm

HUFFMAN_DECODE	x64_i686_HuffmanDecode, 0, 1, 0, 0, 0, 0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccum, 1, 1, 0, 0, 0, 0
;HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep2, 1, 2, 0, 0, 0, 0
;HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4, 1, 4, 0, 0, 0, 0
;HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green, 1, 4, 1, 1, 0, 0
;HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue, 1, 4, 1, 1, 1, 0
;HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red, 1, 4, 1, 1, -1, 0
;HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha, 1, 4, 1, 1, -1, 1
;HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green, 1, 3, 1, 1, 0, 0
;HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue, 1, 3, 1, 1, 1, 0
;HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red, 1, 3, 1, 1, -1, 0


end