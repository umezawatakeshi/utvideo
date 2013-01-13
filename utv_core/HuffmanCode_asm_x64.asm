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

	mov			bl, -64
	mov			cl, 0

	align		64
.label1:
	shld		rax, rcx, cl
	cmp			rsi, r8
	jnb			.label4
	movzx		rcx, byte [rsi]
	inc			rsi
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


%macro HUFFMAN_DECODE 7
%push
	MULTI_CONTEXT_XDEFINE procname, %1, accum, %2, step, %3, multiscan, %4, bottomup, %5, corrpos, %6, dummyalphapos, %7

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, cbNetWidth, cbGrossWidth

	mov			rsi, qword [rsp + %$pSrcBegin]
	mov			rbx, qword [rsp + %$pDecodeTable]
%if %$multiscan
 %if %$bottomup
	mov			rdi, qword [rsp + %$pDstEnd]
	sub			rdi, qword [rsp + %$cbGrossWidth]
	mov			r8,  rdi
	add			r8,  qword [rsp + %$cbNetWidth]
	mov			r12, qword [rsp + %$cbGrossWidth]
	add			r12, qword [rsp + %$cbNetWidth]
 %else
	mov			rdi, qword [rsp + %$pDstBegin]
	mov			r8,  rdi
	add			r8,  qword [rsp + %$cbNetWidth]
	mov			r12, qword [rsp + %$cbGrossWidth]
	sub			r12, qword [rsp + %$cbNetWidth]
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
	mov			ah, byte [rbx]
%endif

	align		64
%%label1:
	cmp			rdi, r8
	jae			%%label2

%if %$dohuffman
	add			cl, ah
	jnc			%%label4
	sub			cl, 32
	mov			r9d, edx
	mov			edx, dword [rsi+4+4]
	add			rsi, 4

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
	sub			r8, qword [rsp + %$$cbGrossWidth]
	cmp			r8, qword [rsp + %$$pDstBegin]
	jbe			%%label3

	sub			rdi, r12
 %else
	add			r8, qword [rsp + %$$cbGrossWidth]
	cmp			r8, qword [rsp + %$$pDstEnd]
	ja			%%label3

	add			rdi, r12
 %endif
	jmp			%%label1
%%label3:
%endif
%pop
%endmacro

	cmp			byte [rbx + 1], 0
	je			.solidframe
	DO_OUTPUT_%$procname	1
	jmp			.funcend
.solidframe:
	DO_OUTPUT_%$procname	0
.funcend:
	SIMPLE_EPILOGUE

%pop
%endmacro

HUFFMAN_DECODE	x64_i686_HuffmanDecode,                                             0, 1, 0, 0,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccum,                                     1, 1, 0, 0,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep2,                                1, 2, 0, 0,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4,                                1, 4, 0, 0,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXGreen,            1, 4, 1, 1,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXBlue,             1, 4, 1, 1, +1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXRed,              1, 4, 1, 1, -1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXRedAndDummyAlpha, 1, 4, 1, 1, -1, +1
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRGreen,             1, 3, 1, 1,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRBlue,              1, 3, 1, 1, +1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRRed,               1, 3, 1, 1, -1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBGreen,             1, 4, 1, 0,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBBlue,              1, 4, 1, 0, -1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBRed,               1, 4, 1, 0, +1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBRedAndDummyAlpha,  1, 4, 1, 0, +1, -1
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBGreen,              1, 3, 1, 0,  0,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBBlue,               1, 3, 1, 0, -1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBRed,                1, 3, 1, 0, +1,  0
HUFFMAN_DECODE	x64_i686_HuffmanDecodeAndAccumStep4ForTopdownBGRXRedAndDummyAlpha,  1, 4, 1, 0, -1, +1
