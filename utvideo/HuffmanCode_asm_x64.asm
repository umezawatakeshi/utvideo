; ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe
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
	mov			rax, rdi			; ï‘ÇËílÇÕ eax Ç…ÅB
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


end
