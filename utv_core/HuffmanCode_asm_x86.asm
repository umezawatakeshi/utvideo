; •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e
; $Id$


%include "Common_asm_x86.mac"


section .text


%push

global _i686_HuffmanEncode
_i686_HuffmanEncode:
	SIMPLE_PROLOGUE	0, pDstBegin, pSrcBegin, pSrcEnd, pEncodeTable

	mov			esi, dword [esp + %$pSrcBegin]
	mov			edi, dword [esp + %$pDstBegin]
	mov			edx, dword [esp + %$pSrcEnd]
	mov			ebp, dword [esp + %$pEncodeTable]
	cmp			dword [ebp], 0
	je			.label3

	mov			bl, -32
	mov			cl, 0

	align		64
.label1:
	shld		eax, ecx, cl
	cmp			esi, edx
	jnb			.label4
	movzx		ecx, byte [esi]
	add			esi, 1
	mov			ecx, dword [ebp+ecx*4]
	add			bl, cl
	jnc			.label1
	sub			cl, bl
	shld		eax, ecx, cl
	mov			dword [edi], eax
	add			edi, 4
	add			cl, bl
	sub			bl, 32
	jmp			.label1

.label4:
	test		bl, 1fh
	jz			.label3
	neg			bl
	mov			cl, bl
	shl			eax, cl
	mov			dword [edi], eax
	add			edi, 4
.label3:
	mov			eax, edi
	sub			eax, dword [esp + %$pDstBegin]

	SIMPLE_EPILOGUE

%pop


%macro HUFFMAN_DECODE 1
%push
	MULTI_CONTEXT_XDEFINE procname, %1

global %$procname
%$procname:
	SIMPLE_PROLOGUE 12, pDstBegin, pDstEnd, pSrcBegin, pDecodeTable

	mov			esi, dword [esp + %$pSrcBegin]
	mov			ebx, dword [esp + %$pDecodeTable]

	mov			edi, dword [esp + %$pDstBegin]
	sub			dword [esp + %$pDstEnd], 4

	mov			cl, 32
	sub			esi, 4

%macro DO_OUTPUT_%$procname 1
%push
	MULTI_CONTEXT_XDEFINE perbyte, %1

	align		64
%%label1:
	cmp			edi, dword [esp + %$$pDstEnd]
	jae			%%label3

	cmp			cl, 32
	jb			%%label4
	sub			cl, 32
	mov			ebp, dword [esi+4+4]
	add			esi, 4

%%label4:
	mov			eax, dword [esi]
	shld		eax, ebp, cl
	shr			eax, 20
	mov			edx, dword [ebx + 8192 + eax*4]
	movzx		eax, word [ebx + eax*2]
	cmp			ah, 255
	jne			%%label0

	mov			eax, dword [esi]
	shld		eax, ebp, cl
	mov			ch, cl
	or			eax, 1
	bsr			edx, eax
	mov			cl, byte [ebx + 8192 + 16384 + edx]					; pDecodeTable->nCodeShift[edx]
	shr			eax, cl
	mov			cl, ch
	add			eax, dword [ebx + 8192 + 16384 + 32 + edx*4]			; pDecodeTable->dwSymbolBase[edx]
	mov			eax, dword [ebx + 8192 + 16384 + 32 + 4*32 + eax*2]		; pDecodeTable->SymbolAndCodeLength[eax]
	mov			edx, eax
	mov			al, 1

%%label0:
	add			cl, ah

%if !%$perbyte
	mov			dword [edi], edx
	movzx		eax, al
	add			edi, eax
	jmp			%%label1
%else
%%label5:
	mov			byte [edi], dl
	shr			edx, 8
	add			edi, 1
	cmp			edi, dword [esp + %$$pDstEnd]
	jae			%%label3
	sub			al, 1
	ja			%%label5
	jmp			%%label1
%endif
%%label3:
%pop
%endmacro

	DO_OUTPUT_%$procname	0
	add			dword [esp + %$pDstEnd], 4
	DO_OUTPUT_%$procname	1
	mov			eax, edi
	SIMPLE_EPILOGUE

%pop
%endmacro

HUFFMAN_DECODE	_i686_HuffmanDecode
