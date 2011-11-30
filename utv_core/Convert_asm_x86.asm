; 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ
; $Id$


%include "Common_asm_x86.mac"


section .text


	align	64
;			 fedcba9876543210
yoff	dq	00010001000100010h
		dq	00000000000000000h
uvoff	dq	00080008000800080h
		dq	00000000000000000h

y2rgb	dq	000004A8500004A85h
		dq	000004A8500004A85h
uv2g	dq	0E5FCF377E5FCF377h
		dq	0E5FCF377E5FCF377h
uv2r	dq	03313000033130000h
		dq	03313000033130000h
uv2b	dq	00000408D0000408Dh
		dq	00000408D0000408Dh

%macro CONVERT_ULY2_TO_BOTTOMUP_RGB	2
%push
	MULTI_CONTEXT_XDEFINE procname, %1, rgb32, %2

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, dwStride, dwDataStride

	mov			ebp, dword [esp + %$pYBegin]
	mov			ebx, dword [esp + %$pUBegin]
	mov			ecx, dword [esp + %$pVBegin]
	mov			edx, dword [esp + %$dwStride]
	mov			esi, dword [esp + %$pDstEnd]			; esi なのに dst のポインタを保持するのは気持ちが悪いが。
	sub			esi, edx
	add			esi, dword [esp + %$dwDataStride]

	pxor		xmm7, xmm7				; xmm7 = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	pcmpeqb		xmm6, xmm6				; xmm6 = ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
	psrlw		xmm6, 8					; xmm6 = 00 ff 00 ff 00 ff 00 ff 00 ff 00 ff 00 ff 00 ff

	align	64
.label0:
	mov			edi, esi
	sub			edi, dword [esp + %$dwDataStride]

	; align	64	; さすがに入れすぎな気がするのでコメントアウト。
.label1:
	movd		xmm0, dword [ebp]		; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 Y3 Y2 Y1 Y0
	punpcklbw	xmm0, xmm7				; xmm0 = 00 00 00 00 00 00 00 00 00 Y3 00 Y2 00 Y1 00 Y0
	psubw		xmm0, oword [yoff]		; xmm0 = 00 00 00 00 00 00 00 00 ---Y3 ---Y2 ---Y1 ---Y0 (de-offset)
	punpcklwd	xmm0, xmm7				; xmm0 = 00 00 ---Y3 00 00 ---Y2 00 00 ---Y1 00 00 ---Y0 (de-offset)

	movd		xmm1, dword [ebx]		; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 U6 U4 U2 U0
	movd		xmm2, dword [ecx]		; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 V6 V4 V2 V0
	punpcklbw	xmm1, xmm2				; xmm1 = 00 00 00 00 00 00 00 00 V6 U6 V4 U4 V2 U2 V0 U0
	punpcklbw	xmm1, xmm7				; xmm1 = 00 V6 00 U6 00 V4 00 U4 00 V2 00 U2 00 V0 00 U0
	psubw		xmm1, oword [uvoff]		; xmm1 = 00 V6 00 U6 00 V4 00 U4 ---V2 ---U2 ---V0 ---U0 (de-offset)
	punpckldq	xmm1, xmm1				; xmm1 = ---V2 ---U2 ---V2 ---U2 ---V0 ---U0 ---V0 ---U0 (de-offset)
	paddw		xmm1, xmm1

	pmaddwd		xmm0, oword [y2rgb]

	movdqa		xmm3, xmm1
	pmaddwd		xmm3, oword [uv2r]
	paddd		xmm3, xmm0				; xmm3 = -R3-------- -R2-------- -R1-------- -R0--------
	psrad		xmm3, 14				; xmm3 = ---------R3 ---------R2 ---------R1 ---------R0

	movdqa		xmm2, xmm1
	pmaddwd		xmm2, oword [uv2b]
	paddd		xmm2, xmm0				; xmm2 = -B3-------- -B2-------- -B1-------- -B0--------
	psrad		xmm2, 14				; xmm2 = ---------B3 ---------B2 ---------B1 ---------B0

	pmaddwd		xmm1, oword [uv2g]
	paddd		xmm1, xmm0				; xmm1 = -G3-------- -G2-------- -G1-------- -G0--------
	psrad		xmm1, 14				; xmm1 = ---------G3 ---------G2 ---------G1 ---------G0

	packssdw	xmm1, xmm3				; xmm1 = ---R3 ---R2 ---R1 ---R0 ---G3 ---G2 ---G1 ---G0
	packssdw	xmm2, xmm7				; xmm2 = 00 00 00 00 00 00 00 00 ---B3 ---B2 ---B1 ---B0
	pmaxsw		xmm1, xmm7				; 計算過程でマイナスになることがあるので、
	pmaxsw		xmm2, xmm7				; ここの pmaxsw xmmN, xmm7 は必要。
	punpcklwd	xmm2, xmm1				; xmm2 = ---G3 ---B3 ---G2 ---B2 ---G1 ---B1 ---G0 ---B0
	punpckhwd	xmm1, xmm6				; xmm1 = 00 ff ---R3 00 ff ---R2 00 ff ---R1 00 ff ---R0
	packuswb	xmm2, xmm2				; xmm2 = XX XX XX XX XX XX XX XX G3 B3 G2 B2 G1 B1 G0 B0
	packuswb	xmm1, xmm1				; xmm1 = XX XX XX XX XX XX XX XX ff R3 ff R2 ff R1 ff R0
	punpcklwd	xmm2, xmm1				; xmm2 = ff R3 G3 B3 ff R2 G2 B2 ff R1 G1 B1 ff R0 G0 B0

%if %$rgb32
	add			edi, 16
	add			ebp, 4
	add			ebx, 2
	add			ecx, 2
	cmp			edi, esi
	ja			.label2
	movdqu		oword [edi-16], xmm2
	jne			.label1
	jmp			.label3

.label2:
	sub			edi, 16
	sub			ebp, 4
	sub			ebx, 2
	sub			ecx, 2
	test		edx, 8
	jz			.label4
	movq		qword [edi], xmm2
	psrldq		xmm2, 8
	add			edi, 8
	add			ebp, 2
	add			ebx, 1
	add			ecx, 1
.label4:
	test		edx, 4
	jz			.label3
	movd		dword [edi], xmm2
	add			edi, 4
	add			ebp, 2
	add			ebx, 1
	add			ecx, 1
%else
	movd		eax, xmm2
	psrldq		xmm2, 4
	mov			word [edi], ax
	shr			eax, 16
	mov			byte [edi+2], al
	add			edi, 3
	add			ebp, 2
	add			ebx, 1
	add			ecx, 1
	cmp			edi, esi
	jae			.label3
	movd		eax, xmm2
	psrldq		xmm2, 4
	mov			word [edi], ax
	shr			eax, 16
	mov			byte [edi+2], al
	add			edi, 3
	cmp			edi, esi
	jae			.label3
	movd		eax, xmm2
	psrldq		xmm2, 4
	mov			word [edi], ax
	shr			eax, 16
	mov			byte [edi+2], al
	add			edi, 3
	add			ebp, 2
	add			ebx, 1
	add			ecx, 1
	cmp			edi, esi
	jae			.label3
	movd		eax, xmm2
	mov			word [edi], ax
	shr			eax, 16
	mov			byte [edi+2], al
	add			edi, 3
	cmp			edi, esi
	jb			.label1
%endif

.label3:
	sub			esi, edx
	cmp			esi, dword [esp + %$pDstBegin]
	ja			.label0

	SIMPLE_EPILOGUE

%pop
%endmacro

CONVERT_ULY2_TO_BOTTOMUP_RGB	_x86_sse2_ConvertULY2ToBottomupRGB24, 0
CONVERT_ULY2_TO_BOTTOMUP_RGB	_x86_sse2_ConvertULY2ToBottomupRGB32, 1


; Y  =  0.29891 R + 0.58661 G + 0.11448 B
; Cb = -0.16874 R - 0.33126 G + 0.50000 B
; Cr =  0.50000 R - 0.41869 G - 0.08131 B

; Y  =  0.257 R + 0.504 G + 0.098 B + 16
; Cb = -0.148 R - 0.291 G + 0.439 B + 128
; Cr =  0.439 R - 0.368 G - 0.071 B + 128

	align	64
;			 fedcba9876543210
;b2yuv	dq	0064b00000000064bh
;		dq	0fdb7fdb70e0e0e0eh
;g2yuv	dq	0203e00000000203eh
;		dq	0f3fbf3fbf6b0f6b0h
;r2yuv	dq	0106e00000000106eh
;		dq	00e0e0e0efb42fb42h
b2yuv	dq	00646000000000646h
		dq	0fdbafdba0e0c0e0ch
g2yuv	dq	02042000000002042h
		dq	0f439f439f6b0f6b0h
r2yuv	dq	01073000000001073h
		dq	00e0c0e0cfb44fb44h
yuvoff	dq	00004200000042000h
		dq	00020200000202000h

%macro CONVERT_BOTTOMUP_RGB_TO_ULY2 2
%push
	MULTI_CONTEXT_XDEFINE procname, %1, rgb32, %2

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, dwStride, dwDataStride

	mov			edi, dword [esp + %$pYBegin]
	mov			ebx, dword [esp + %$pUBegin]
	mov			ecx, dword [esp + %$pVBegin]
	mov			ebp, dword [esp + %$pSrcEnd]
	sub			ebp, dword [esp + %$dwStride]
	add			ebp, dword [esp + %$dwDataStride]

	align	64
.label0:
	mov			esi, ebp
	sub			esi, dword [esp + %$dwDataStride]
%if %$rgb32
	add			esi, 4
%else
	add			esi, 3
%endif

	;align	64
.label1:
%if %$rgb32
	movd		xmm0, dword [esi-4]					; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0
	movd		xmm1, dword [esi]					; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R1 G1 B1
%else
	movd		xmm0, dword [esi-3]					; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0
	movd		xmm1, dword [esi-1]					; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 R1 G1 B1 XX
	psrld		xmm1, 8								; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 00 R1 G1 B1
%endif

	punpcklbw	xmm0, xmm1							; xmm0 = 00 00 00 00 00 00 00 00 00 XX R1 R0 G1 G0 B1 B0
.label3:
	pxor		xmm1, xmm1
	punpcklbw	xmm0, xmm1							; xmm0 = 00 00 00 XX 00 R1 00 R0 00 G1 00 G0 00 B1 00 B0

	pshufd		xmm1, xmm0, 055h					; xmm1 = 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0
	pshufd		xmm2, xmm0, 0aah					; xmm2 = 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0
	pshufd		xmm0, xmm0, 000h					; xmm0 = 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0

	pmaddwd		xmm0, oword [b2yuv]					; xmm0 = ----B2V---- ----B2U---- ----B2Y1--- ----B2Y0---
	pmaddwd		xmm1, oword [g2yuv]					; xmm1 = ----G2V---- ----G2U---- ----G2Y1--- ----G2Y0---
	pmaddwd		xmm2, oword [r2yuv]					; xmm2 = ----R2V---- ----R2U---- ----R2Y1--- ----R2Y0---

	paddd		xmm0, oword [yuvoff]
	paddd		xmm2, xmm1
	paddd		xmm0, xmm2							; xmm0 = -----V----- -----U----- -----Y1---- -----Y0----

	psrld		xmm0, 14							; xmm0 = ---------V0 ---------U0 ---------Y1 ---------Y0
	packssdw	xmm0, xmm0							; xmm0 = XX XX XX XX XX XX XX XX ---V0 ---U0 ---Y1 ---Y0
	packuswb	xmm0, xmm0							; xmm0 = XX XX XX XX XX XX XX XX XX XX XX XX V0 U0 Y1 Y0
	movd		eax, xmm0
	mov			word [edi], ax
	shr			eax, 16
	mov			byte [ebx], al
	mov			byte [ecx], ah

	add			edi, 2
	add			ebx, 1
	add			ecx, 1
%if %$rgb32
	add			esi, 8
%else
	add			esi, 6
%endif
	cmp			esi, ebp
	jb			.label1
	ja			.label2

%if %$rgb32
	movd		xmm0, dword [esi-4]					; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0
%else
	movd		xmm0, dword [esi-3]					; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0
%endif

	punpcklbw	xmm0, xmm0							; xmm0 = 00 00 00 00 00 00 00 00 XX XX R0 R0 G0 G0 B0 B0
	jmp			.label3

.label2:
	sub			ebp, dword [esp + %$dwStride]
	cmp			ebp, dword [esp + %$pSrcBegin]
	ja			.label0

	SIMPLE_EPILOGUE

%pop
%endmacro

CONVERT_BOTTOMUP_RGB_TO_ULY2	_x86_sse2_ConvertBottomupRGB24ToULY2, 0
CONVERT_BOTTOMUP_RGB_TO_ULY2	_x86_sse2_ConvertBottomupRGB32ToULY2, 1
