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


%macro HUFFMAN_DECODE 5
%push
	MULTI_CONTEXT_XDEFINE procname, %1, accum, %2, step, %3, corrpos, %4, dummyalphapos, %5

global %$procname
%$procname:
	SIMPLE_PROLOGUE 12, pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, cbWidth, scbStride

%define %$byCorrBuf     0
%define %$pLineEnd      4
%define %$dwLineOffset  8

	mov			esi, dword [esp + %$pSrcBegin]
	mov			ebx, dword [esp + %$pDecodeTable]

	mov			edi, dword [esp + %$pDstBegin]
	mov			eax, edi
	add			eax, dword [esp + %$cbWidth]
	mov			dword [esp + %$pLineEnd], eax
	mov			eax, dword [esp + %$scbStride]
	sub			eax, dword [esp + %$cbWidth]
	mov			dword [esp + %$dwLineOffset], eax

%macro DO_OUTPUT_%$procname 1
%push
	MULTI_CONTEXT_XDEFINE dohuffman, %1

%if %$$accum
 %if %$$corrpos != 0
	mov			byte [esp + %$$byCorrBuf], 00h
 %else
	mov			byte [esp + %$$byCorrBuf], 80h
 %endif
%endif

%if %$dohuffman
	mov			cl, 32
	mov			edx, dword [esi]
	sub			esi, 4
%else
	mov			ah, byte [ebx]
%endif

	align		64
%%label1:
	cmp			edi, dword [esp + %$$pLineEnd]
	jae			%%label2

%if %$dohuffman
	cmp			cl, 32
	jb			%%label4
	sub			cl, 32
	mov			ebp, edx
	mov			edx, dword [esi+4+4]
	add			esi, 4

%%label4:
	mov			eax, ebp
	shld		eax, edx, cl
	shr			eax, 20
	movzx		eax, word [ebx + eax*2]
	cmp			ah, 255
	jne			%%label0

	mov			eax, ebp
	shld		eax, edx, cl
	mov			ch, cl
	or			eax, 1
	bsr			ebp, eax
	mov			cl, byte [ebx + 8192 + ebp]					; pDecodeTable->nCodeShift[ebp]
	shr			eax, cl
	mov			cl, ch
	add			eax, dword [ebx + 8192+32 + ebp*4]			; pDecodeTable->dwSymbolBase[ebp]
	mov			eax, dword [ebx + 8192+32+4*32 + eax*2]		; pDecodeTable->SymbolAndCodeLength[eax]
	mov			ebp, dword [esi]

%%label0:
	add			cl, ah
%else
	mov			al, ah
%endif

%if %$$accum
	add			al, byte [esp + %$$byCorrBuf]
	mov			byte [esp + %$$byCorrBuf], al
%endif
%if %$$corrpos != 0
	add			al, byte [edi + %$$corrpos]
%endif
	mov			byte [edi], al
%if %$$dummyalphapos != 0
	mov			byte [edi + %$$dummyalphapos], 0ffh
%endif
	add			edi, %$$step
	jmp			%%label1

%%label2:
	add			edi, dword [esp + %$$dwLineOffset]
	cmp			edi, dword [esp + %$$pDstEnd]
	je			%%label3
	mov			edx, edi
	add			edx, dword [esp + %$$cbWidth]
	mov			dword [esp + %$$pLineEnd], edx
%if %$dohuffman
	mov			edx, dword [esi+4]
%endif

	jmp			%%label1
%%label3:
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

HUFFMAN_DECODE	_i686_HuffmanDecode,                                     0, 1,  0,  0
HUFFMAN_DECODE	_i686_HuffmanDecodeAndAccum,                             1, 1,  0,  0
HUFFMAN_DECODE	_i686_HuffmanDecodeAndAccumStep2,                        1, 2,  0,  0
HUFFMAN_DECODE	_i686_HuffmanDecodeAndAccumStep3,                        1, 3,  0,  0
HUFFMAN_DECODE	_i686_HuffmanDecodeAndAccumStep4,                        1, 4,  0,  0
HUFFMAN_DECODE	_i686_HuffmanDecodeAndAccumStep3ForBGRBlue,              1, 3, +1,  0
HUFFMAN_DECODE	_i686_HuffmanDecodeAndAccumStep3ForBGRRed,               1, 3, -1,  0
HUFFMAN_DECODE	_i686_HuffmanDecodeAndAccumStep4ForBGRXBlue,             1, 4, +1,  0
HUFFMAN_DECODE	_i686_HuffmanDecodeAndAccumStep4ForBGRXRed,              1, 4, -1,  0
HUFFMAN_DECODE	_i686_HuffmanDecodeAndAccumStep4ForBGRXRedAndDummyAlpha, 1, 4, -1, +1
HUFFMAN_DECODE	_i686_HuffmanDecodeAndAccumStep4ForXRGBRedAndDummyAlpha, 1, 4, +1, -1
