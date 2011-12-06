; •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e
; $Id$


%include "Common_asm_x64.mac"


section .text


%push

global x64_i686_HuffmanEncode
x64_i686_HuffmanEncode:
	SIMPLE_PROLOGUE	0, pDstBegin, pSrcBegin, pSrcEnd, pEncodeTable

	mov			rsi, qword [rsp + %$pSrcBegin]
	mov			rdi, qword [rsp + %$pDstBegin]
	mov			r8,  qword [rsp + %$pSrcEnd]
	mov			rdx, qword [rsp + %$pEncodeTable]
	cmp			qword [rdx], 0
	je			.label3

	mov			bl, -32
	mov			cl, 0

	align		64
.label1:
	shld		eax, ecx, cl
	cmp			rsi, r8
	jnb			.label4
	movzx		rcx, byte [rsi]
	inc			rsi
	mov			ecx, dword [rdx+rcx*4]
	add			bl, cl
	jnc			.label1
	sub			cl, bl
	shld		eax, ecx, cl
	mov			dword [rdi], eax
	add			rdi, 4
	add			cl, bl
	sub			bl, 32
	jmp			.label1

.label4:
	test		bl, 1fh
	jz			.label3
	neg			bl
	mov			cl, bl
	shl			eax, cl
	mov			dword [rdi], eax
	add			rdi, 4
.label3:
	mov			rax, rdi
	sub			rax, qword [rsp + %$pDstBegin]

	SIMPLE_EPILOGUE

%pop


%macro HUFFMAN_DECODE 7
%push
	MULTI_CONTEXT_XDEFINE procname, %1, accum, %2, step, %3, multiscan, %4, bottomup, %5, corrpos, %6, dummyalphapos, %7

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, dwNetWidth, dwGrossWidth

	mov			rsi, qword [rsp + %$pSrcBegin]
	mov			rbx, qword [rsp + %$pDecodeTable]
%if %$multiscan
 %if %$bottomup
	mov			rdi, qword [rsp + %$pDstEnd]
	sub			rdi, qword [rsp + %$dwGrossWidth]
	mov			r8,  rdi
	add			r8,  qword [rsp + %$dwNetWidth]
	mov			r12, qword [rsp + %$dwGrossWidth]
	add			r12, qword [rsp + %$dwNetWidth]
 %else
	mov			rdi, qword [rsp + %$pDstBegin]
	mov			r8,  rdi
	add			r8,  qword [rsp + %$dwNetWidth]
	mov			r12, qword [rsp + %$dwGrossWidth]
	sub			r12, qword [rsp + %$dwNetWidth]
 %endif
%else
	mov			rdi, qword [rsp + %$pDstBegin]
	mov			r8,  qword [rsp + %$pDstEnd]
%endif

%macro DO_OUTPUT_%$procname 1
%push
	MULTI_CONTEXT_XDEFINE dohuffman, %1

%if %$$accum
 %if %$$corrpos != 0
	mov			r11b, 00h
 %else
	mov			r11b, 80h
 %endif
%endif

%if %$dohuffman
	mov			cl, -32
	mov			ah, 32
	mov			edx, dword [rsi]
	sub			rsi, 4
%else
	mov			ah, byte [ebx]
%endif

	align		64
%%label1:
	cmp			rdi, r8
	jae			%%label2

%if %$dohuffman
	add			cl, ah
	jnc			%%label4
	sub			cl, 32
	add			rsi, 4
	mov			r9d, edx
	mov			edx, dword [rsi+4]

%%label4:
	mov			eax, r9d
	shld		eax, edx, cl
	shr			eax, 20
	movzx		eax, word [rbx + rax*2]
	cmp			ah, 255
	jne			%%label0

	mov			eax, r9d
	shld		eax, edx, cl
	mov			ch, cl
	or			eax, 1
	bsr			r10, rax
	mov			cl, byte [rbx + 8192 + r10]					; pDecodeTable->nCodeShift[r10]
	shr			eax, cl
	mov			cl, ch
	mov			r10d, dword [rbx + 8192+32 + r10*4]			; pDecodeTable->dwSymbolBase[r10]
	add			r10, rax
	mov			eax, dword [rbx + 8192+32+4*32 + r10*2]		; pDecodeTable->SymbolAndCodeLength[r10]

%%label0:
%else
	mov			al, ah
%endif

%if %$$accum
	add			al, r11b
	mov			r11b, al
%endif
%if %$$corrpos != 0
	add			al, byte [rdi + %$$corrpos]
%endif
	mov			byte [rdi], al
%if %$$dummyalphapos != 0
	mov			byte [rdi + %$$dummyalphapos], 0ffh
%endif
%if %$$step == 1
	inc			rdi
%else
	add			rdi, %$$step
%endif
	jmp			%%label1

%%label2:
%if %$$multiscan
 %if %$$bottomup
	sub			r8, qword [rsp + %$$dwGrossWidth]
	cmp			r8, qword [rsp + %$$pDstBegin]
	jbe			%%label3

	sub			rdi, r12
 %else
	add			r8, qword [rsp + %$$dwGrossWidth]
	cmp			r8, qword [rsp + %$$pDstEnd]
	jae			%%label3

	add			rdi, r12
 %endif
	jmp			%%label1
%%label3:
%endif
%pop
%endmacro

	cmp			byte [ebx + 1], 0
	je			.solidframe
	DO_OUTPUT_%$procname	1
	jmp			.funcend
.solidframe:
	DO_OUTPUT_%$procname	0
.funcend:
	SIMPLE_EPILOGUE

%pop
%endmacro

HUFFMAN_DECODE	x64_i686_HuffmanDecode,                                              0, 1, 0, 0,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccum,                                      1, 1, 0, 0,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep2,                                 1, 2, 0, 0,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4,                                 1, 4, 0, 0,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Green,            1, 4, 1, 1,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Blue,             1, 4, 1, 1, +1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32Red,              1, 4, 1, 1, -1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupRGB32RedAndDummyAlpha, 1, 4, 1, 1, -1, +1
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Green,            1, 3, 1, 1,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Blue,             1, 3, 1, 1, +1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupRGB24Red,              1, 3, 1, 1, -1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Green,             1, 4, 1, 0,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Blue,              1, 4, 1, 0, -1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32Red,               1, 4, 1, 0, +1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForTopdownRGB32RedAndDummyAlpha,  1, 4, 1, 0, +1, -1
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Green,             1, 3, 1, 0,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Blue,              1, 3, 1, 0, -1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForTopdownRGB24Red,               1, 3, 1, 0, +1,  0
