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


%macro HUFFMAN_DECODE 5
%push
	MULTI_CONTEXT_XDEFINE procname, %1, accum, %2, step, %3, corrpos, %4, dummyalphapos, %5

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, cbWidth, scbStride

	mov			rsi, qword [rsp + %$pSrcBegin]
	mov			rbx, qword [rsp + %$pDecodeTable]

	mov			rdi, qword [rsp + %$pDstBegin]
	mov			r8,  rdi
	add			r8,  qword [rsp + %$cbWidth]
	mov			r12, qword [rsp + %$scbStride]
	sub			r12, qword [rsp + %$cbWidth]

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
	add			rdi, r12
	cmp			rdi, qword [rsp + %$$pDstEnd]
	je			%%label3
	add			r8, qword [rsp + %$$scbStride]
	jmp			%%label1
%%label3:
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

HUFFMAN_DECODE	i686_HuffmanDecode,                                     0, 1,  0,  0
HUFFMAN_DECODE	i686_HuffmanDecodeAndAccum,                             1, 1,  0,  0
HUFFMAN_DECODE	i686_HuffmanDecodeAndAccumStep2,                        1, 2,  0,  0
HUFFMAN_DECODE	i686_HuffmanDecodeAndAccumStep3,                        1, 3,  0,  0
HUFFMAN_DECODE	i686_HuffmanDecodeAndAccumStep4,                        1, 4,  0,  0
HUFFMAN_DECODE	i686_HuffmanDecodeAndAccumStep3ForBGRBlue,              1, 3, +1,  0
HUFFMAN_DECODE	i686_HuffmanDecodeAndAccumStep3ForBGRRed,               1, 3, -1,  0
HUFFMAN_DECODE	i686_HuffmanDecodeAndAccumStep4ForBGRXBlue,             1, 4, +1,  0
HUFFMAN_DECODE	i686_HuffmanDecodeAndAccumStep4ForBGRXRed,              1, 4, -1,  0
HUFFMAN_DECODE	i686_HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha, 1, 4, -1, +1
HUFFMAN_DECODE	i686_HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha, 1, 4, +1, -1
