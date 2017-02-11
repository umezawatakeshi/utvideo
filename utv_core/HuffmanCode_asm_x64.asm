; •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e
; $Id$


%include "Common_asm_x64.mac"


section .text


%push

global i686_HuffmanEncode
i686_HuffmanEncode:
	SIMPLE_PROLOGUE	0, pDstBegin, pSrcBegin, pSrcEnd, pEncodeTable

	mov			rsi, qword [rsp + %$pSrcBegin]
	mov			rdi, qword [rsp + %$pDstBegin]
	mov			r8,  qword [rsp + %$pSrcEnd]
	mov			rdx, qword [rsp + %$pEncodeTable]
	cmp			qword [rdx], 0
	je			.label3

	mov			bl, -64
	mov			cl, 0

	align		64
.label1:
	shld		rax, rcx, cl
	cmp			rsi, r8
	jnb			.label4
	movzx		rcx, byte [rsi]
	add			rsi, 1
	mov			rcx, qword [rdx+rcx*8]
	add			bl, cl
	jnc			.label1
	sub			cl, bl
	shld		rax, rcx, cl
	rol			rax, 32
	mov			qword [rdi], rax
	add			rdi, 8
	add			cl, bl
	sub			bl, 64
	jmp			.label1

.label4:
	test		bl, 3fh
	jz			.label3
	neg			bl
	mov			cl, bl
	shl			rax, cl
	rol			rax, 32
	mov			dword [rdi], eax
	add			rdi, 4
	cmp			bl, 32
	jae			.label3
	rol			rax, 32
	mov			dword [rdi], eax
	add			rdi, 4
.label3:
	mov			rax, rdi
	sub			rax, qword [rsp + %$pDstBegin]

	SIMPLE_EPILOGUE

%pop


%macro HUFFMAN_DECODE 2
%push
	MULTI_CONTEXT_XDEFINE procname, %1, bmi2, %2

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pSrcBegin, pDecodeTable

	mov			rsi, qword [rsp + %$pSrcBegin]
	mov			rbx, qword [rsp + %$pDecodeTable]

	mov			rdi, qword [rsp + %$pDstBegin]
	mov			r8,  qword [rsp + %$pDstEnd]
	sub			r8, 4

	mov			cl, 32
	mov			edx, dword [rsi]
	add			rsi, 4

%macro DO_OUTPUT_%$procname 1
%push
	MULTI_CONTEXT_XDEFINE perbyte, %1

	align		64
%%label1:
	cmp			rdi, r8
	jae			%%label3

	cmp			cl, 32
	jb			%%label4
	sub			cl, 32
	shl			rdx, 32
	mov			r9d, dword [rsi]
	or			rdx, r9
	add			rsi, 4

%%label4:
%if %$$bmi2
	shlx		rax, rdx, rcx
%else
	mov			rax, rdx
	shl			rax, cl
%endif
	shr			rax, 20+32
	mov			r11d, dword [rbx + 8192 + rax*4]					; pDecodeTable->MultiSpeedTable_sym[rax]
	movzx		eax, word [rbx + rax*2]							; pDecodeTable->MultiSpeedTable_cs[rax]
	cmp			ah, 255
	jne			%%label0

%if %$$bmi2
	shlx		rax, rdx, rcx
%else
	mov			rax, rdx
	shl			rax, cl
	mov			r15d, ecx
%endif
	shr			rax, 32
	or			eax, 1
	bsr			r10, rax
%if %$$bmi2
	movzx		r13d, byte [rbx + 8192 + 16384 + r10]				; pDecodeTable->nCodeShift[r10]
	shrx		eax, eax, r13d
%else
	mov			cl, byte [rbx + 8192 + 16384 + r10]					; pDecodeTable->nCodeShift[r10]
	shr			eax, cl
	mov			ecx, r15d
%endif
	mov			r10d, dword [rbx + 8192 + 16384 + 32 + r10*4]			; pDecodeTable->dwSymbolBase[r10]
	add			r10, rax
	mov			eax, dword [rbx + 8192 + 16384 + 32+4*32 + r10*2]		; pDecodeTable->SymbolAndCodeLength[r10]
	mov			r11d, eax
	mov			al, 1

%%label0:
	add			cl, ah

%if !%$perbyte
	mov			dword [rdi], r11d
	movzx		rax, al
	add			rdi, rax
	jmp			%%label1
%else
%%label5:
	mov			byte [rdi], r11b
	shr			r11d, 8
	add			rdi, 1
	cmp			rdi, r8
	jae			%%label3
	sub			al, 1
	ja			%%label5
	jmp			%%label1
%endif

%%label3:
%pop
%endmacro

	DO_OUTPUT_%$procname	0
	add			r8, 4
	DO_OUTPUT_%$procname	1
	mov			rax, rdi
	SIMPLE_EPILOGUE

%pop
%endmacro

HUFFMAN_DECODE	i686_HuffmanDecode, 0
HUFFMAN_DECODE	bmi2_HuffmanDecode, 1
