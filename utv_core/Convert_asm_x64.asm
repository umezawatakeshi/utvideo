; 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ
; $Id$


%include "Common_asm_x64.mac"


section .text


%include "Coefficient_asm_x86x64.mac"


align	64
pshufb16bgrx2bgr	dq	0908060504020100h
					dq	808080800e0d0c0ah
pshufb16xrgb2rgb	dq	0a09070605030201h
					dq	808080800f0e0d0bh
pshufb16uvxpand		dq	8001800080018000h
					dq	8003800280038002h

%macro CONVERT_ULY2_TO_RGB 5
%push
	MULTI_CONTEXT_XDEFINE procname, %1, use_sse41, %2, coeffvar, %3, littleendian, %4, rgb32, %5

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride

	mov			rdi, [rsp + %$pDstBegin]
	mov			rsi, [rsp + %$pYBegin]
	mov			rbx, [rsp + %$pUBegin]
	mov			rdx, [rsp + %$pVBegin]
	mov			r9,  [rsp + %$pDstEnd]
	mov			r10, [rsp + %$cbWidth]
	mov			r11, [rsp + %$scbStride]
	sub			r11, r10
	xor			rcx, rcx

	pxor		xmm7, xmm7				; xmm7 = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	pcmpeqb		xmm6, xmm6				; xmm6 = ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
	psrlw		xmm6, 8					; xmm6 = 00 ff 00 ff 00 ff 00 ff 00 ff 00 ff 00 ff 00 ff

	movdqa		xmm8, [yoff]
	movdqa		xmm9, [uvoff]
	movdqa		xmm10, [%$coeffvar + yuvcoeff.y2rgb]
	movdqa		xmm11, [%$coeffvar + yuvcoeff.uv2r]
	movdqa		xmm12, [%$coeffvar + yuvcoeff.uv2b]
	movdqa		xmm13, [%$coeffvar + yuvcoeff.uv2g]
%if %$use_sse41
 %if %$littleendian
	movdqa		xmm5, [pshufb16bgrx2bgr]
 %else
	movdqa		xmm5, [pshufb16xrgb2rgb]
 %endif
	movdqa		xmm14, [pshufb16uvxpand]
%endif

	align	64
.label0:
	lea			r8, [rdi+r10]

	; align	64	; さすがに入れすぎな気がするのでコメントアウト。
.label1:
%if ! %$use_sse41
	movd		xmm0, [rsi+rcx*2]		; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 Y3 Y2 Y1 Y0
	punpcklbw	xmm0, xmm7				; xmm0 = 00 00 00 00 00 00 00 00 00 Y3 00 Y2 00 Y1 00 Y0
	punpcklwd	xmm0, xmm7				; xmm0 = 00 00 00 Y3 00 00 00 Y2 00 00 00 Y1 00 00 00 Y0
%else
	pmovzxbd	xmm0, [rsi+rcx*2]		; xmm0 = 00 00 00 Y3 00 00 00 Y2 00 00 00 Y1 00 00 00 Y0
%endif
	psubw		xmm0, xmm8				; xmm0 = 00 00 ---Y3 00 00 ---Y2 00 00 ---Y1 00 00 ---Y0 (de-offset)

	movd		xmm1, [rbx+rcx]			; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 U6 U4 U2 U0
	movd		xmm2, [rdx+rcx]			; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 V6 V4 V2 V0
	punpcklbw	xmm1, xmm2				; xmm1 = 00 00 00 00 00 00 00 00 V6 U6 V4 U4 V2 U2 V0 U0
%if ! %$use_sse41
	punpcklbw	xmm1, xmm7				; xmm1 = 00 V6 00 U6 00 V4 00 U4 00 V2 00 U2 00 V0 00 U0
	punpckldq	xmm1, xmm1				; xmm1 = 00 V2 00 U2 00 V2 00 U2 00 V0 00 U0 00 V0 00 U0
%else
	pshufb		xmm1, xmm14				; xmm1 = 00 V2 00 U2 00 V2 00 U2 00 V0 00 U0 00 V0 00 U0
%endif
	psubw		xmm1, xmm9				; xmm1 = ---V2 ---U2 ---V2 ---U2 ---V0 ---U0 ---V0 ---U0 (de-offset)

	pmaddwd		xmm0, xmm10

	movdqa		xmm3, xmm1
	pmaddwd		xmm3, xmm11
	paddd		xmm3, xmm0				; xmm3 = -R3-------- -R2-------- -R1-------- -R0--------
	psrad		xmm3, 13				; xmm3 = ---------R3 ---------R2 ---------R1 ---------R0

	movdqa		xmm2, xmm1
	pmaddwd		xmm2, xmm12
	paddd		xmm2, xmm0				; xmm2 = -B3-------- -B2-------- -B1-------- -B0--------
	psrad		xmm2, 13				; xmm2 = ---------B3 ---------B2 ---------B1 ---------B0

	pmaddwd		xmm1, xmm13
	paddd		xmm1, xmm0				; xmm1 = -G3-------- -G2-------- -G1-------- -G0--------
	psrad		xmm1, 13				; xmm1 = ---------G3 ---------G2 ---------G1 ---------G0

%if %$littleendian
	packssdw	xmm1, xmm3				; xmm1 = ---R3 ---R2 ---R1 ---R0 ---G3 ---G2 ---G1 ---G0
	packssdw	xmm2, xmm7				; xmm2 = 00 00 00 00 00 00 00 00 ---B3 ---B2 ---B1 ---B0
	pmaxsw		xmm1, xmm7				; 計算過程でマイナスになることがあるので、
	pmaxsw		xmm2, xmm7				; ここの pmaxsw xmmN, xmm7 は必要。
	punpcklwd	xmm2, xmm1				; xmm2 = ---G3 ---B3 ---G2 ---B2 ---G1 ---B1 ---G0 ---B0
	punpckhwd	xmm1, xmm6				; xmm1 = 00 ff ---R3 00 ff ---R2 00 ff ---R1 00 ff ---R0
	packuswb	xmm2, xmm2				; xmm2 = XX XX XX XX XX XX XX XX G3 B3 G2 B2 G1 B1 G0 B0
	packuswb	xmm1, xmm1				; xmm1 = XX XX XX XX XX XX XX XX ff R3 ff R2 ff R1 ff R0
	punpcklwd	xmm2, xmm1				; xmm2 = ff R3 G3 B3 ff R2 G2 B2 ff R1 G1 B1 ff R0 G0 B0
%else
	packssdw	xmm3, xmm2				; xmm3 = ---B3 ---B2 ---B1 ---B0 ---R3 ---R2 ---R1 ---R0
	movdqa		xmm2, xmm6
	packssdw	xmm1, xmm1				; xmm1 = ---G3 ---G2 ---G1 ---G0 ---G3 ---G2 ---G1 ---G0
	pmaxsw		xmm3, xmm7				; 計算過程でマイナスになることがあるので、
	pmaxsw		xmm1, xmm7				; ここの pmaxsw xmmN, xmm7 は必要。
	punpcklwd	xmm2, xmm3				; xmm2 = ---R3 00 ff ---R2 00 ff ---R1 00 ff ---R0 00 ff
	punpckhwd	xmm1, xmm3				; xmm1 = ---B3 ---G3 ---B2 ---G2 ---B1 ---G1 ---B0 ---G0
	packuswb	xmm2, xmm2				; xmm2 = XX XX XX XX XX XX XX XX R3 ff R2 ff R1 ff R0 ff
	packuswb	xmm1, xmm1				; xmm1 = XX XX XX XX XX XX XX XX B3 G3 B2 G2 B1 G1 B0 G0
	punpcklwd	xmm2, xmm1				; xmm2 = B3 G3 R3 ff B2 G2 R2 ff B1 G1 R1 ff B0 G0 R0 ff
 %if ! %$rgb32
	; めんどくさいので
	movdqa		xmm1, xmm2
	psrldq		xmm2, 1
	pslldq		xmm1, 15
	por			xmm2, xmm1
 %endif
%endif

%if %$rgb32
	add			rdi, 16
	add			rcx, 2
	cmp			rdi, r8
	ja			.label2
	movdqu		[rdi-16], xmm2
	jne			.label1
	jmp			.label3

.label2:
	movq		[rdi-16], xmm2
	sub			rdi, 8
	sub			rcx, 1
%else
 %if ! %$use_sse41
	movq		rax, xmm2
	psrldq		xmm2, 8
	mov			[rdi], ax
	shr			rax, 16
	mov			[rdi+2], al
	shr			rax, 16
	mov			[rdi+3], ax
	shr			rax, 16
	mov			[rdi+5], al
	add			rdi, 6
	add			rcx, 1
	cmp			rdi, r8
	jae			.label3

	movq		rax, xmm2
	mov			[rdi], ax
	shr			rax, 16
	mov			[rdi+2], al
	shr			rax, 16
	mov			[rdi+3], ax
	shr			rax, 16
	mov			[rdi+5], al
	add			rdi, 6
	add			rcx, 1
	cmp			rdi, r8
	jb			.label1
 %else
	pshufb		xmm2, xmm5
	add			rdi, 12
	add			rcx, 2
	cmp			rdi, r8
	ja			.label2
	movq		[rdi-12], xmm2
	psrldq		xmm2, 8
	movd		[rdi-4], xmm2
	jne			.label1
	jmp			.label3

.label2:
	movd		[rdi-12], xmm2
	psrldq		xmm2, 4
	movd		eax, xmm2
	mov			[rdi-8], ax
	sub			rdi, 6
	sub			rcx, 1
 %endif
%endif

.label3:
	add			rdi, r11
	cmp			rdi, r9
	jne			.label0

	SIMPLE_EPILOGUE

%pop
%endmacro

CONVERT_ULY2_TO_RGB	sse2_ConvertULY2ToBGR,   0, bt601coeff, 1, 0
CONVERT_ULY2_TO_RGB	sse2_ConvertULY2ToBGRX,  0, bt601coeff, 1, 1
CONVERT_ULY2_TO_RGB	sse2_ConvertULY2ToRGB,   0, bt601coeff, 0, 0
CONVERT_ULY2_TO_RGB	sse2_ConvertULY2ToXRGB,  0, bt601coeff, 0, 1
CONVERT_ULY2_TO_RGB	sse2_ConvertULH2ToBGR,   0, bt709coeff, 1, 0
CONVERT_ULY2_TO_RGB	sse2_ConvertULH2ToBGRX,  0, bt709coeff, 1, 1
CONVERT_ULY2_TO_RGB	sse2_ConvertULH2ToRGB,   0, bt709coeff, 0, 0
CONVERT_ULY2_TO_RGB	sse2_ConvertULH2ToXRGB,  0, bt709coeff, 0, 1

CONVERT_ULY2_TO_RGB	sse41_ConvertULY2ToBGR,  1, bt601coeff, 1, 0
CONVERT_ULY2_TO_RGB	sse41_ConvertULY2ToBGRX, 1, bt601coeff, 1, 1
CONVERT_ULY2_TO_RGB	sse41_ConvertULY2ToRGB,  1, bt601coeff, 0, 0
CONVERT_ULY2_TO_RGB	sse41_ConvertULY2ToXRGB, 1, bt601coeff, 0, 1
CONVERT_ULY2_TO_RGB	sse41_ConvertULH2ToBGR,  1, bt709coeff, 1, 0
CONVERT_ULY2_TO_RGB	sse41_ConvertULH2ToBGRX, 1, bt709coeff, 1, 1
CONVERT_ULY2_TO_RGB	sse41_ConvertULH2ToRGB,  1, bt709coeff, 0, 0
CONVERT_ULY2_TO_RGB	sse41_ConvertULH2ToXRGB, 1, bt709coeff, 0, 1


align	64
shufdata32	dq	8005800180048000h
			dq	8007800380068002h
shufdata24	dq	8006800380058002h
			dq	8080808080078004h
shufdata_pack	dq	808080800c080400h
				dq	8080808080808080h

%macro CONVERT_RGB_TO_ULY2 5
%push
	MULTI_CONTEXT_XDEFINE procname, %1, use_ssse3, %2, coeffvar, %3, littleendian, %4, rgb32, %5

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride

	mov			rsi, [rsp + %$pSrcBegin]
	mov			rdi, [rsp + %$pYBegin]
	mov			rbx, [rsp + %$pUBegin]
	mov			rdx, [rsp + %$pVBegin]
	mov			r9,  [rsp + %$pSrcEnd]
	mov			r10, [rsp + %$cbWidth]
	mov			r11, [rsp + %$scbStride]
	sub			r11, r10
	xor			rcx, rcx

%if %$use_ssse3
 %if %$rgb32
	movdqa		xmm3, [shufdata32]
 %else
	movdqa		xmm3, [shufdata24]
 %endif
	movdqa		xmm8, [shufdata_pack]
%endif
	movdqa		xmm4, [%$coeffvar + yuvcoeff.b2yuv]
	movdqa		xmm5, [%$coeffvar + yuvcoeff.g2yuv]
	movdqa		xmm6, [%$coeffvar + yuvcoeff.r2yuv]
	movdqa		xmm7, [yuvoff]

	align	64
.label0:
	lea			r8, [rsi+r10]

	;align	64
.label1:
%if ! %$use_ssse3
 %if %$rgb32
	movd		xmm0, [rsi  ]						; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0 / B0 G0 R0 XX
	movd		xmm1, [rsi+4]						; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R1 G1 B1 / B1 G1 R1 XX
 %else
	movd		xmm0, [rsi  ]						; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0 / XX B0 G0 R0
	movd		xmm1, [rsi+2]						; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 R1 G1 B1 XX / B1 G1 R1 XX
	psrld		xmm1, 8								; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 00 R1 G1 B1 / 00 B1 G1 R1
 %endif

	punpcklbw	xmm0, xmm1							; xmm0 = 00 00 00 00 00 00 00 00 00 XX R1 R0 G1 G0 B1 B0 / B1 B0 G1 G0 R1 R0 XX XX
													;                                                        / XX XX B1 B0 G1 G0 R1 R0 (rgb24be)
	pxor		xmm1, xmm1
	punpcklbw	xmm0, xmm1							; xmm0 = 00 00 00 XX 00 R1 00 R0 00 G1 00 G0 00 B1 00 B0
													;        00 B1 00 B0 00 G1 00 G0 00 R1 00 R0 00 XX 00 XX (rgb32be)
													;        00 00 00 XX 00 B1 00 B0 00 G1 00 G0 00 R1 00 R0 (rgb24be)
%else
 %if %$rgb32
	movq		xmm0, [rsi]							; xmm0 = 00 00 00 00 00 00 00 00 XX R1 G1 B1 XX R0 G0 B0 / B1 G1 R1 XX B0 G0 R0 XX
	pshufb		xmm0, xmm3							; xmm0 = 00 XX 00 XX 00 R1 00 R0 00 G1 00 G0 00 B1 00 B0
													;        00 B1 00 B0 00 G1 00 G0 00 R1 00 R0 00 XX 00 XX (rgb32be)
 %else
	movq		xmm0, [rsi]							; xmm0 = 00 00 00 00 00 00 00 00 XX XX R1 G1 B1 R0 G0 B0 / XX XX B1 G1 R1 B0 G0 R0
	pslldq		xmm0, 2								; xmm0 = 00 00 00 00 00 00 00 00 R1 G1 B1 R0 G0 B0 00 00 / B1 G1 R1 B0 G0 R0 00 00
	jmp			.label4
.label3:
	movq		xmm0, [rsi-2]						; xmm0 = 00 00 00 00 00 00 00 00 R1 G1 B1 R0 G0 B0 XX XX / B1 G1 R1 B0 G0 R0 XX XX
.label4:
	pshufb		xmm0, xmm3							; xmm0 = 00 00 00 00 00 R1 00 R0 00 G1 00 G0 00 B1 00 B0
													;        00 00 00 00 00 B1 00 B0 00 G1 00 G0 00 R1 00 R0 (rgb24be)
 %endif
%endif

%if %$littleendian
	pshufd		xmm1, xmm0, 055h					; xmm1 = 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0
	pshufd		xmm2, xmm0, 0aah					; xmm2 = 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0
	pshufd		xmm0, xmm0, 000h					; xmm0 = 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0
%else
 %if %$rgb32
	pshufd		xmm1, xmm0, 0aah					; xmm1 = 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0
	pshufd		xmm2, xmm0, 055h					; xmm2 = 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0
	pshufd		xmm0, xmm0, 0ffh					; xmm0 = 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0
 %else
	pshufd		xmm1, xmm0, 055h					; xmm1 = 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0
	pshufd		xmm2, xmm0, 000h					; xmm2 = 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0
	pshufd		xmm0, xmm0, 0aah					; xmm0 = 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0
 %endif
%endif

	pmaddwd		xmm0, xmm4							; xmm0 = ----B2V---- ----B2U---- ----B2Y1--- ----B2Y0---
	pmaddwd		xmm1, xmm5							; xmm1 = ----G2V---- ----G2U---- ----G2Y1--- ----G2Y0---
	pmaddwd		xmm2, xmm6							; xmm2 = ----R2V---- ----R2U---- ----R2Y1--- ----R2Y0---

	paddd		xmm0, xmm7
	paddd		xmm2, xmm1
	paddd		xmm0, xmm2							; xmm0 = -----V----- -----U----- -----Y1---- -----Y0----

	psrld		xmm0, 14							; xmm0 = ---------V0 ---------U0 ---------Y1 ---------Y0
%if ! %$use_ssse3
	packssdw	xmm0, xmm0							; xmm0 = XX XX XX XX XX XX XX XX ---V0 ---U0 ---Y1 ---Y0
	packuswb	xmm0, xmm0							; xmm0 = XX XX XX XX XX XX XX XX XX XX XX XX V0 U0 Y1 Y0
%else
	pshufb		xmm0, xmm8							; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 V0 U0 Y1 Y0
%endif
	movd		eax, xmm0
	mov			[rdi+rcx*2], ax
	shr			eax, 16
	mov			[rbx+rcx], al
	mov			[rdx+rcx], ah

	add			rcx, 1
%if %$rgb32
	add			rsi, 8
%else
	add			rsi, 6
%endif
	cmp			rsi, r8
%if (!%$use_ssse3) || %$rgb32
	jb			.label1
%else
	jb			.label3
%endif

.label2:
	add			rsi, r11
	cmp			rsi, r9
	jne			.label0

	SIMPLE_EPILOGUE

%pop
%endmacro

CONVERT_RGB_TO_ULY2	sse2_ConvertBGRToULY2,   0, bt601coeff, 1, 0
CONVERT_RGB_TO_ULY2	sse2_ConvertBGRXToULY2,  0, bt601coeff, 1, 1
CONVERT_RGB_TO_ULY2	sse2_ConvertRGBToULY2,   0, bt601coeff, 0, 0
CONVERT_RGB_TO_ULY2	sse2_ConvertXRGBToULY2,  0, bt601coeff, 0, 1
CONVERT_RGB_TO_ULY2	sse2_ConvertBGRToULH2,   0, bt709coeff, 1, 0
CONVERT_RGB_TO_ULY2	sse2_ConvertBGRXToULH2,  0, bt709coeff, 1, 1
CONVERT_RGB_TO_ULY2	sse2_ConvertRGBToULH2,   0, bt709coeff, 0, 0
CONVERT_RGB_TO_ULY2	sse2_ConvertXRGBToULH2,  0, bt709coeff, 0, 1

CONVERT_RGB_TO_ULY2	ssse3_ConvertBGRToULY2,  1, bt601coeff, 1, 0
CONVERT_RGB_TO_ULY2	ssse3_ConvertBGRXToULY2, 1, bt601coeff, 1, 1
CONVERT_RGB_TO_ULY2	ssse3_ConvertRGBToULY2,  1, bt601coeff, 0, 0
CONVERT_RGB_TO_ULY2	ssse3_ConvertXRGBToULY2, 1, bt601coeff, 0, 1
CONVERT_RGB_TO_ULY2	ssse3_ConvertBGRToULH2,  1, bt709coeff, 1, 0
CONVERT_RGB_TO_ULY2	ssse3_ConvertBGRXToULH2, 1, bt709coeff, 1, 1
CONVERT_RGB_TO_ULY2	ssse3_ConvertRGBToULH2,  1, bt709coeff, 0, 0
CONVERT_RGB_TO_ULY2	ssse3_ConvertXRGBToULH2, 1, bt709coeff, 0, 1




	align	64
bgr2planarpshufb16	dq	05020f0c09060300h
					dq	0d0a0704010e0b08h
planar2bgrpshufb16	dq	0d02070c01060b00h
					dq	050a0f04090e0308h

bgrx2planarpshufb16	dq	0d0905010c080400h
					dq	0f0b07030e0a0602h
planar2bgrxpshufb16	dq	0d0905010c080400h	; 計算してみたら実は同じだという…
					dq	0f0b07030e0a0602h

xrgb2planarpshufb16	dq	0e0a06020f0b0703h
					dq	0c0804000d090501h
planar2xrgbpshufb16	dq	0105090d0004080ch
					dq	03070b0f02060a0eh

_0x80b				dq	8080808080808080h
					dq	8080808080808080h

%push
global ssse3_ConvertBGRToULRG
ssse3_ConvertBGRToULRG:
	SIMPLE_PROLOGUE	0, pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride

	movdqa		xmm7, [bgr2planarpshufb16]
	movdqa		xmm6, [_0x80b]
	mov			rsi, [rsp + %$pSrcBegin]
	mov			rdi, [rsp + %$pGBegin]
	mov			rbx, [rsp + %$pBBegin]
	mov			rdx, [rsp + %$pRBegin]
	mov			r9,  [rsp + %$pSrcEnd]
	mov			r10, [rsp + %$cbWidth]
	mov			r11, [rsp + %$scbStride]
	sub			r11, r10
	sub			r10, 48 - 3
	xor			rcx, rcx

.label1:
	lea			r8, [rsi+r10]
.label0:
	lddqu		xmm0, [rsi   ]			; xmm0 = B5 R4 G4 B4 R3 G3 B3 R2 G2 B2 R1 G1 B1 R0 G0 B0
	lddqu		xmm1, [rsi+16]			; xmm1 = Ga Ba R9 G9 B9 R8 G8 B8 R7 G7 B7 R6 G6 B6 R5 G5
	lddqu		xmm2, [rsi+32]			; xmm2 = Rf Gf Bf Re Ge Be Rd Gd Bd Rc Gc Bc Rb Gb Bb Ra

	pshufb		xmm0, xmm7				; xmm0 = G4 G3 G2 G1 G0 R4 R3 R2 R1 R0 B5 B4 B3 B2 B1 B0
	pshufb		xmm1, xmm7				; xmm1 = R9 R8 R7 R6 R5 Ba B9 B8 B7 B6 Ga G9 G8 G7 G6 G5
	pshufb		xmm2, xmm7				; xmm2 = Bf Be Bd Bc Bb Gf Ge Gd Gc Gb Rf Re Rd Rc Rb Ra

	movdqa		xmm5, xmm2
	palignr		xmm2, xmm1, 6			; xmm5 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 Ba B9 B8 B7 B6
	palignr		xmm1, xmm0, 6			; xmm4 = Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0 R4 R3 R2 R1 R0
	palignr		xmm0, xmm5, 6			; xmm3 = B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb Gf Ge Gd Gc Gb

	movdqa		xmm3, xmm0
	palignr		xmm0, xmm1, 5			; xmm0 = Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
	palignr		xmm1, xmm2, 5			; xmm1 = R4 R3 R2 R1 R0 Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5
	palignr		xmm2, xmm3, 5			; xmm2 = Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb

	movdqu		[rdi+rcx], xmm0
	psubb		xmm0, xmm6

	palignr		xmm1, xmm1, 11			; xmm1 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 R4 R3 R2 R1 R0
	palignr		xmm2, xmm2, 5			; xmm2 = Bf Be Bd Bc Bb Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0

	psubb		xmm1, xmm0
	psubb		xmm2, xmm0
	movdqu		[rbx+rcx], xmm2
	movdqu		[rdx+rcx], xmm1

	add			rcx, 16
	add			rsi, 48
	cmp			rsi, r8
	jb			.label0

	add			r8, 48 - 3

.label3:
	cmp			rsi, r8
	jae			.label2
	mov			al, [rsi+1]
	mov			[rdi+rcx], al
	sub			al, 80h

	mov			ah, [rsi  ]
	sub			ah, al
	mov			[rbx+rcx], ah

	mov			ah, [rsi+2]
	sub			ah, al
	mov			[rdx+rcx], ah

	add			rcx, 1
	add			rsi, 3
	jmp			.label3

.label2:
	add			rsi, r11
	cmp			rsi, r9
	jne			.label1

	SIMPLE_EPILOGUE
%pop


;dummy0			dq	3121110130201000h
;				dq	3323130332221202h
;dummy1			dq	3525150534241404h
;				dq	3727170736261606h
;dummy2			dq	3929190938281808h
;				dq	3b2b1b0b3a2a1a0ah
;dummy3			dq	3d2d1d0d3c2c1c0ch
;				dq	3f2f1f0f3e2e1e0eh

%macro CONVERT_XRGB_TO_ULRGA 3
%push
	MULTI_CONTEXT_XDEFINE procname, %1, bgrx, %2, havealpha, %3

global %$procname
%$procname:
%if %$havealpha
	SIMPLE_PROLOGUE 0, pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride
%else
	SIMPLE_PROLOGUE 0, pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride
%endif

%if %$bgrx
	movdqa		xmm7, [bgrx2planarpshufb16]
%else
	movdqa		xmm7, [xrgb2planarpshufb16]
%endif
	movdqa		xmm6, [_0x80b]
	mov			rsi, [rsp + %$pSrcBegin]
	mov			rdi, [rsp + %$pGBegin]
	mov			rbx, [rsp + %$pBBegin]
	mov			rdx, [rsp + %$pRBegin]
%if %$havealpha
	mov			rbp, [rsp + %$pABegin]
%endif
	mov			r9,  [rsp + %$pSrcEnd]
	mov			r10, [rsp + %$cbWidth]
	mov			r11, [rsp + %$scbStride]
	sub			r11, r10
	sub			r10, 64 - 4
	xor			rcx, rcx

.label1:
	lea			r8, [rsi+r10]
.label0:
	lddqu		xmm0, [rsi   ]		; xmm0 = A3 R3 G3 B3 A2 R2 G2 B2 A1 R1 G1 B1 A0 R0 G0 B0 (bgrx)
	lddqu		xmm1, [rsi+16]		; xmm1 = A7 R7 G7 B7 A6 R6 G6 B6 A5 R5 G5 B5 A4 R4 G4 B4
	lddqu		xmm2, [rsi+32]		; xmm2 = Ab Rb Gb Bb Aa Ra Ga Ba A9 R9 G9 B9 A8 R8 G8 B8
	lddqu		xmm3, [rsi+48]		; xmm3 = Af Rf Gf Bf Ae Re Ge Be Ad Rd Gd Bd Ac Rc Gc Bc

	pshufb		xmm0, xmm7			; xmm0 = A3 A2 A1 A0 R3 R2 R1 R0 G3 G2 G1 G0 B3 B2 B1 B0
	pshufb		xmm1, xmm7			; xmm1 = A7 A6 A5 A4 R7 R6 R5 R4 G7 G6 G5 G4 B7 B6 B5 B4
	pshufb		xmm2, xmm7			; xmm2 = Ab Aa A9 A8 Rb Ra R9 R8 Gb Ga G9 G8 Bb Ba B9 B8
	pshufb		xmm3, xmm7			; xmm3 = Af Ae Ad Ac Rf Re Rd Rc Gf Ge Gd Gc Bf Be Bd Bc

	movdqa		xmm4, xmm0
	punpckldq	xmm4, xmm1			; xmm4 = G7 G6 G5 G4 G3 G2 G1 G0 B7 B6 B5 B4 B3 B2 B1 B0
	punpckhdq	xmm0, xmm1			; xmm0 = A7 A6 A5 A4 A3 A2 A1 A0 R7 R6 R5 R4 R3 R2 R1 R0
	movdqa		xmm5, xmm2
	punpckldq	xmm5, xmm3			; xmm5 = Gf Ge Gd Gc Gb Ga G9 G8 Bf Be Bd Bc Bb Ba B9 B8
	punpckhdq	xmm2, xmm3			; xmm2 = Af Ae Ad Ac Ab Aa A9 A8 Rf Re Rd Rc Rb Ra R9 R8

	movdqa		xmm1, xmm4
	punpcklqdq	xmm1, xmm5			; xmm1 = Bf Be Bd Bc Bb Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0
	punpckhqdq	xmm4, xmm5			; xmm4 = Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
%if %$havealpha
	movdqa		xmm3, xmm0
	punpckhqdq	xmm3, xmm2			; xmm3 = Af Ae Ad Ac Ab Aa A9 A8 A7 A6 A5 A4 A3 A2 A1 A0
%endif
	punpcklqdq	xmm0, xmm2			; xmm0 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 R4 R3 R2 R1 R0

	movdqu		[rdi+rcx], xmm4
	psubb		xmm4, xmm6
%if %$havealpha
	movdqu		[rbp+rcx], xmm3
%endif

	psubb		xmm1, xmm4
	movdqu		[rbx+rcx], xmm1
	psubb		xmm0, xmm4
	movdqu		[rdx+rcx], xmm0

	add			rcx, 16
	add			rsi, 64
	cmp			rsi, r8
	jb			.label0

	add			r8, 64 - 4

.label3:
	cmp			rsi, r8
	jae			.label2
%if %$bgrx
	mov			al, [rsi+1]
%else
	mov			al, [rsi+2]
%endif
	mov			[rdi+rcx], al
	sub			al, 80h

%if %$havealpha
 %if %$bgrx
	mov			ah, [rsi+3]
 %else
	mov			ah, [rsi  ]
 %endif
	mov			[rbp+rcx], ah
%endif

%if %$bgrx
	mov			ah, [rsi  ]
%else
	mov			al, [rsi+3]
%endif
	sub			ah, al
	mov			[rbx+rcx], ah

%if %$bgrx
	mov			ah, [rsi+2]
%else
	mov			al, [rsi+1]
%endif
	sub			ah, al
	mov			[rdx+rcx], ah

	add			rcx, 1
	add			rsi, 4
	jmp			.label3

.label2:
	add			rsi, r11
	cmp			rsi, r9
	jne			.label1

	SIMPLE_EPILOGUE
%pop
%endmacro

CONVERT_XRGB_TO_ULRGA	ssse3_ConvertBGRXToULRG, 1, 0
CONVERT_XRGB_TO_ULRGA	ssse3_ConvertXRGBToULRG, 0, 0
CONVERT_XRGB_TO_ULRGA	ssse3_ConvertBGRAToULRA, 1, 1
CONVERT_XRGB_TO_ULRGA	ssse3_ConvertARGBToULRA, 0, 1


	align	64
yuyv2planarpshufb16	dq	0e0c0a0806040200h
					dq	0f0b07030d090501h
planar2yuyvpshufb16	dq	0d0309020c010800h
					dq	0f070b060e050a04h

uyvy2planarpshufb16	dq	0f0d0b0907050301h
					dq	0e0a06020c080400h
planar2uyvypshufb16	dq	030d0209010c0008h
					dq	070f060b050e040ah

%macro CONVERT_YUV422_TO_ULY2 2
%push
	MULTI_CONTEXT_XDEFINE procname, %1, yuyv, %2

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd

%if %$yuyv
	movdqa		xmm7, [yuyv2planarpshufb16]
%else
	movdqa		xmm7, [uyvy2planarpshufb16]
%endif
	mov			rsi, [rsp + %$pSrcBegin]
	mov			rdi, [rsp + %$pYBegin]
	mov			rbx, [rsp + %$pUBegin]
	mov			rdx, [rsp + %$pVBegin]
	mov			r9,  [rsp + %$pSrcEnd]
	xor			rcx, rcx

	mov			r8, r9
	sub			r8, 64 - 4
.label0:
	lddqu		xmm0, [rsi   ]		; xmm0 = V06 Y07 U06 Y06 V04 Y05 U04 Y04 V02 Y03 U02 Y02 V00 Y01 U00 Y00 (yuyv)
	lddqu		xmm1, [rsi+16]		; xmm1 = V0e Y0f U0e Y0e V0c Y0d U0c Y0c V0a Y0b U0a Y0a V08 Y09 U08 Y08
	lddqu		xmm2, [rsi+32]		; xmm2 = V16 Y17 U16 Y16 V14 Y15 U14 Y14 V12 Y13 U12 Y12 V10 Y11 U10 Y10
	lddqu		xmm3, [rsi+48]		; xmm3 = V1e Y1f U1e Y1e V1c Y1d U1c Y1c V1a Y1b U1a Y1a V18 Y19 U18 Y18

	pshufb		xmm0, xmm7			; xmm0 = V06 V04 V02 V00 U06 U04 U02 U00 Y07 Y06 Y05 Y04 Y03 Y02 Y01 Y00
	pshufb		xmm1, xmm7			; xmm1 = V0e V0c V0a V08 U0e U0c U0a U08 Y0f Y0e Y0d Y0c Y0b Y0a Y09 Y08
	pshufb		xmm2, xmm7			; xmm2 = V16 V14 V12 V10 U16 U14 U12 U10 Y17 Y16 Y15 Y14 Y13 Y12 Y11 Y10
	pshufb		xmm3, xmm7			; xmm3 = V1e V1c V1a V18 U1e U1c U1a U18 Y1f Y1e Y1d Y1c Y1b Y1a Y19 Y18

	movdqa		xmm4, xmm0
	movdqa		xmm5, xmm2
	punpcklqdq	xmm4, xmm1			; xmm4 = Y0f Y0e Y0d Y0c Y0b Y0a Y09 Y08 Y07 Y06 Y05 Y04 Y03 Y02 Y01 Y00
	punpcklqdq	xmm5, xmm3			; xmm5 = Y1f Y1e Y1d Y1c Y1b Y1a Y19 Y18 Y17 Y16 Y15 Y14 Y13 Y12 Y11 Y10
	movdqu		[rdi+rcx*2   ], xmm4
	movdqu		[rdi+rcx*2+16], xmm5

	punpckhdq	xmm0, xmm1			; xmm0 = V0e V0c V0a V08 V06 V04 V02 V00 U0e U0c U0a U08 U06 U04 U02 U00
	punpckhdq	xmm2, xmm3			; xmm2 = V1e V1c V1a V18 V16 V14 V12 V10 U1e U1c U1a U18 U16 U14 U12 U10

	movdqa		xmm1, xmm0
	punpcklqdq	xmm1, xmm2			; xmm1 = U1e U1c U1a U18 U16 U14 U12 U10 U0e U0c U0a U08 U06 U04 U02 U00
	punpckhqdq	xmm0, xmm2			; xmm0 = V1e V1c V1a V18 V16 V14 V12 V10 V0e V0c V0a V08 V06 V04 V02 V00
	movdqu		[rbx+rcx], xmm1
	movdqu		[rdx+rcx], xmm0

	add			rcx, 16
	add			rsi, 64
	cmp			rsi, r8
	jb			.label0

	add			r8, 64 - 4

.label3:
	cmp			rsi, r8
	jae			.label2

	mov			al, [rsi+1-%$yuyv]
	mov			[rdi+rcx*2  ], al

	mov			al, [esi+3-%$yuyv]
	mov			[rdi+rcx*2+1], al

	mov			al, [esi  +%$yuyv]
	mov			[rbx+rcx], al

	mov			al, [esi+2+%$yuyv]
	mov			[rdx+rcx], al

	add			rcx, 1
	add			rsi, 4
	jmp			.label3

.label2:

	SIMPLE_EPILOGUE
%pop
%endmacro

CONVERT_YUV422_TO_ULY2	ssse3_ConvertYUYVToULY2, 1
CONVERT_YUV422_TO_ULY2	ssse3_ConvertUYVYToULY2, 0



dummyg		dq	0706050403020100h
			dq	0f0e0d0c0b0a0908h
dummyb		dq	9090909090909090h
			dq	9090909090909090h
dummyr		dq	0a0a0a0a0a0a0a0a0h
			dq	0a0a0a0a0a0a0a0a0h
dummya		dq	3736353433323130h
			dq	3f3e3d3c3b3a3938h

%push
global ssse3_ConvertULRGToBGR
ssse3_ConvertULRGToBGR:
	SIMPLE_PROLOGUE	0, pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, cbWidth, scbStride

	movdqa		xmm7, [planar2bgrpshufb16]
	movdqa		xmm6, [_0x80b]
	mov			rdi, [rsp + %$pDstBegin]
	mov			rsi, [rsp + %$pGBegin]
	mov			rbx, [rsp + %$pBBegin]
	mov			rdx, [rsp + %$pRBegin]
	mov			r9,  [rsp + %$pDstEnd]
	mov			r10, [rsp + %$cbWidth]
	mov			r11, [rsp + %$scbStride]
	sub			r11, r10
	sub			r10, 48 - 3
	xor			rcx, rcx

.label1:
	lea			r8, [rdi+r10]
.label0:
	lddqu		xmm0, [rsi+rcx]			; xmm0 = Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
	lddqu		xmm1, [rdx+rcx]
	lddqu		xmm2, [rbx+rcx]

	movdqa		xmm3, xmm0
	paddb		xmm3, xmm6
	paddb		xmm1, xmm3				; xmm1 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 R4 R3 R2 R1 R0
	paddb		xmm2, xmm3				; xmm2 = Bf Be Bd Bc Bb Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0

	palignr		xmm1, xmm1, 5			; xmm1 = R4 R3 R2 R1 R0 Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5
	palignr		xmm2, xmm2, 11			; xmm2 = Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb

	movdqa		xmm5, xmm2
	palignr		xmm2, xmm0, 11			; xmm3 = B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb Gf Ge Gd Gc Gb
	palignr		xmm0, xmm1, 11			; xmm4 = Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0 R4 R3 R2 R1 R0
	palignr		xmm1, xmm5, 11			; xmm5 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 Ba B9 B8 B7 B6

	movdqa		xmm5, xmm2
	palignr		xmm2, xmm1, 10			; xmm2 = Bf Be Bd Bc Bb Gf Ge Gd Gc Gb Rf Re Rd Rc Rb Ra
	palignr		xmm1, xmm0, 10			; xmm1 = R9 R8 R7 R6 R5 Ba B9 B8 B7 B6 Ga G9 G8 G7 G6 G5
	palignr		xmm0, xmm5, 10			; xmm0 = G4 G3 G2 G1 G0 R4 R3 R2 R1 R0 B5 B4 B3 B2 B1 B0

	pshufb		xmm0, xmm7				; xmm0 = B5 R4 G4 B4 R3 G3 B3 R2 G2 B2 R1 G1 B1 R0 G0 B0
	pshufb		xmm1, xmm7				; xmm1 = Ga Ba R9 G9 B9 R8 G8 B8 R7 G7 B7 R6 G6 B6 R5 G5
	pshufb		xmm2, xmm7				; xmm2 = Rf Gf Bf Re Ge Be Rd Gd Bd Rc Gc Bc Rb Gb Bb Ra

	movdqu		[rdi   ], xmm0
	movdqu		[rdi+16], xmm1
	movdqu		[rdi+32], xmm2

	add			rcx, 16
	add			rdi, 48
	cmp			rdi, r8
	jb			.label0

	add			r8, 48 - 3

.label3:
	cmp			rdi, r8
	jae			.label2
	mov			al, [rsi+rcx]
	mov			[rdi+1], al
	add			al, 80h

	mov			ah, [rbx+rcx]
	add			ah, al
	mov			[rdi], ah

	mov			ah, [rdx+rcx]
	add			ah, al
	mov			[rdi+2], ah

	add			rcx, 1
	add			rdi, 3
	jmp			.label3

.label2:
	add			rdi, r11
	cmp			rdi, r9
	jne			.label1

	SIMPLE_EPILOGUE
%pop



%macro CONVERT_ULRGA_TO_XRGB 3
%push
	MULTI_CONTEXT_XDEFINE procname, %1, bgrx, %2, havealpha, %3

global %$procname
%$procname:
%if %$havealpha
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride
%else
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, cbWidth, scbStride
%endif

%if %$bgrx
	movdqa		xmm7, [planar2bgrxpshufb16]
%else
	movdqa		xmm7, [planar2xrgbpshufb16]
%endif
	movdqa		xmm6, [_0x80b]
%if ! %$havealpha
	pcmpeqd		xmm3, xmm3
%endif
	mov			rdi, [rsp + %$pDstBegin]
	mov			rsi, [rsp + %$pGBegin]
	mov			rbx, [rsp + %$pBBegin]
	mov			rdx, [rsp + %$pRBegin]
%if %$havealpha
	mov			rbp, [rsp + %$pABegin]
%endif
	mov			r9,  [rsp + %$pDstEnd]
	mov			r10, [rsp + %$cbWidth]
	mov			r11, [rsp + %$scbStride]
	sub			r11, r10
	sub			r10, 64 - 4
	xor			rcx, rcx

.label1:
	lea			r8, [rdi+r10]
.label0:
	lddqu		xmm0, [rsi+rcx]		; xmm0 = Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
	lddqu		xmm1, [rbx+rcx]
	lddqu		xmm2, [rdx+rcx]
%if %$havealpha
	lddqu		xmm3, [rbp+rcx]		; xmm3 = Af Ae Ad Ac Ab Aa A9 A8 A7 A6 A5 A4 A3 A2 A1 A0
%endif
;	vlddqu		xmm0, [dummyg]
;	vlddqu		xmm1, [dummyb]
;	vlddqu		xmm2, [dummyr]

	movdqa		xmm4, xmm0
	paddb		xmm4, xmm6
	paddb		xmm1, xmm4			; xmm1 = Bf Be Bd Bc Bb Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0
	paddb		xmm2, xmm4			; xmm2 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 R4 R3 R2 R1 R0

	movdqa		xmm4, xmm1
	punpckldq	xmm4, xmm0			; xmm4 = G7 G6 G5 G4 B7 B6 B5 B4 G3 G2 G1 G0 B3 B2 B1 B0
	punpckhdq	xmm1, xmm0			; xmm1 = Gf Ge Gd Gc Bf Be Bd Bc Gb Ga G9 G8 Bb Ba B9 B8
	movdqa		xmm5, xmm2
	punpckldq	xmm5, xmm3			; xmm5 = A7 A6 A5 A4 R7 R6 R5 R4 A3 A2 A1 A0 R3 R2 R1 R0
	punpckhdq	xmm2, xmm3			; xmm2 = Af Ae Ad Ac Rf Re Rd Rc Ab Aa A9 A8 Rb Ra R9 R8

	movdqa		xmm0, xmm4
	punpcklqdq	xmm0, xmm5			; xmm0 = A3 A2 A1 A0 R3 R2 R1 R0 G3 G2 G1 G0 B3 B2 B1 B0
	punpckhqdq	xmm4, xmm5			; xmm4 = A7 A6 A5 A4 R7 R6 R5 R4 G7 G6 G5 G4 B7 B6 B5 B4
	movdqa		xmm5, xmm1
	punpcklqdq	xmm5, xmm2			; xmm5 = Ab Aa A9 A8 Rb Ra R9 R8 Gb Ga G9 G8 Bb Ba B9 B8
	punpckhqdq	xmm1, xmm2			; xmm1 = Af Ae Ad Ac Rf Re Rd Rc Gf Ge Gd Gc Bf Be Bd Bc

	pshufb		xmm0, xmm7			; xmm0 = A3 R3 G3 B3 A2 R2 G2 B2 A1 R1 G1 B1 A0 R0 G0 B0
	pshufb		xmm4, xmm7			; xmm4 = A7 R7 G7 B7 A6 R6 G6 B6 A5 R5 G5 B5 A4 R4 G4 B4
	pshufb		xmm5, xmm7			; xmm5 = Ab Rb Gb Bb Aa Ra Ga Ba A9 R9 G9 B9 A8 R8 G8 B8
	pshufb		xmm1, xmm7			; xmm1 = Af Rf Gf Bf Ae Re Ge Be Ad Rd Gd Bd Ac Rc Gc Bc

	movdqu		[rdi   ], xmm0
	movdqu		[rdi+16], xmm4
	movdqu		[rdi+32], xmm5
	movdqu		[rdi+48], xmm1

	add			rcx, 16
	add			rdi, 64
	cmp			rdi, r8
	jb			.label0

	add			r8, 64 - 4

.label3:
	cmp			rdi, r8
	jae			.label2
	mov			al, [rsi+rcx]
%if %$bgrx
	mov			[rdi+1], al
%else
	mov			[rdi+2], al
%endif
	sub			al, 80h

%if %$havealpha
	mov			ah, [rbp+rcx]
 %if %$bgrx
	mov			[rdi+3], ah
 %else
	mov			[rdi  ], ah
 %endif
%else
 %if %$bgrx
	mov			byte [rdi+3], 0ffh
 %else
	mov			byte [rdi  ], 0ffh
 %endif
%endif

	mov			ah, [rbx+rcx]
	add			ah, al
%if %$bgrx
	mov			[rdi  ], ah
%else
	mov			[rdi+3], ah
%endif

	mov			ah, [rdx+rcx]
	add			ah, al
%if %$bgrx
	mov			[rdi+2], ah
%else
	mov			[rdi+1], ah
%endif

	add			rcx, 1
	add			rdi, 4
	jmp			.label3

.label2:
	add			rdi, r11
	cmp			rdi, r9
	jne			.label1

	SIMPLE_EPILOGUE
%pop
%endmacro

CONVERT_ULRGA_TO_XRGB	ssse3_ConvertULRGToBGRX, 1, 0
CONVERT_ULRGA_TO_XRGB	ssse3_ConvertULRGToXRGB, 0, 0
CONVERT_ULRGA_TO_XRGB	ssse3_ConvertULRAToBGRA, 1, 1
CONVERT_ULRGA_TO_XRGB	ssse3_ConvertULRAToARGB, 0, 1



%macro CONVERT_ULY2_TO_YUV422 2
%push
	MULTI_CONTEXT_XDEFINE procname, %1, yuyv, %2

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin

%if %$yuyv
	movdqa		xmm7, [planar2yuyvpshufb16]
%else
	movdqa		xmm7, [planar2uyvypshufb16]
%endif
	mov			rdi, [rsp + %$pDstBegin]
	mov			rsi, [rsp + %$pYBegin]
	mov			rbx, [rsp + %$pUBegin]
	mov			rdx, [rsp + %$pVBegin]
	mov			r9,  [rsp + %$pDstEnd]
	xor			rcx, rcx

	mov			r8, r9
	sub			r8, 64 - 4
.label0:
	lddqu		xmm2, [rbx+rcx]			; xmm2 = U1e U1c U1a U18 U16 U14 U12 U10 U0e U0c U0a U08 U06 U04 U02 U00
	lddqu		xmm3, [rdx+rcx]			; xmm3 = V1e V1c V1a V18 V16 V14 V12 V10 V0e V0c V0a V08 V06 V04 V02 V00
	lddqu		xmm0, [rsi+rcx*2   ]	; xmm0 = Y0f Y0e Y0d Y0c Y0b Y0a Y09 Y08 Y07 Y06 Y05 Y04 Y03 Y02 Y01 Y00
	lddqu		xmm1, [rsi+rcx*2+16]	; xmm1 = Y1f Y1e Y1d Y1c Y1b Y1a Y19 Y18 Y17 Y16 Y15 Y14 Y13 Y12 Y11 Y10

	movdqa		xmm4, xmm2
	punpckldq	xmm4, xmm3				; xmm4 = V0e V0c V0a V08 U0e U0c U0a U08 V06 V04 V02 V00 U06 U04 U02 U00
	punpckhdq	xmm2, xmm3				; xmm2 = V1e V1c V1a V18 U1e U1c U1a U18 V16 V14 V12 V10 U16 U14 U12 U10

	movdqa		xmm5, xmm0
	punpcklqdq	xmm5, xmm4				; xmm5 = V06 V04 V02 V00 U06 U04 U02 U00 Y07 Y06 Y05 Y04 Y03 Y02 Y01 Y00
	punpckhqdq	xmm0, xmm4				; xmm0 = V0e V0c V0a V08 U0e U0c U0a U08 Y0f Y0e Y0d Y0c Y0b Y0a Y09 Y08
	movdqa		xmm4, xmm1
	punpcklqdq	xmm4, xmm2				; xmm4 = V16 V14 V12 V10 U16 U14 U12 U10 Y17 Y16 Y15 Y14 Y13 Y12 Y11 Y10
	punpckhqdq	xmm1, xmm2				; xmm1 = V1e V1c V1a V18 U1e U1c U1a U18 Y1f Y1e Y1d Y1c Y1b Y1a Y19 Y18

	pshufb		xmm5, xmm7				; xmm5 = V06 Y07 U06 Y06 V04 Y05 U04 Y04 V02 Y03 U02 Y02 V00 Y01 U00 Y00
	pshufb		xmm0, xmm7				; xmm0 = V0e Y0f U0e Y0e V0c Y0d U0c Y0c V0a Y0b U0a Y0a V08 Y09 U08 Y08
	pshufb		xmm4, xmm7				; xmm4 = V16 Y17 U16 Y16 V14 Y15 U14 Y14 V12 Y13 U12 Y12 V10 Y11 U10 Y10
	pshufb		xmm1, xmm7				; xmm1 = V1e Y1f U1e Y1e V1c Y1d U1c Y1c V1a Y1b U1a Y1a V18 Y19 U18 Y18

	movdqu		[rdi   ], xmm5
	movdqu		[rdi+16], xmm0
	movdqu		[rdi+32], xmm4
	movdqu		[rdi+48], xmm1

	add			rcx, 16
	add			rdi, 64
	cmp			rdi, r8
	jb			.label0

	add			r8, 64 - 4

.label3:
	cmp			rdi, r8
	jae			.label2

	mov			al, [rsi+rcx*2  ]
	mov			[rdi+1-%$yuyv],al

	mov			al, [rsi+rcx*2+1]
	mov			[rdi+3-%$yuyv], al

	mov			al, [rbx+rcx]
	mov			[rdi  +%$yuyv], al

	mov			al, [rdx+rcx]
	mov			[rdi+2+%$yuyv], al

	add			rcx, 1
	add			rdi, 4
	jmp			.label3

.label2:

	SIMPLE_EPILOGUE
%pop
%endmacro

CONVERT_ULY2_TO_YUV422	ssse3_ConvertULY2ToYUYV, 1
CONVERT_ULY2_TO_YUV422	ssse3_ConvertULY2ToUYVY, 0



align	64
encorrelate_pshufb_bgrx		dq	0ff05ff05ff01ff01h, 0ff0dff0dff09ff09h
encorrelate_add_bgrx		dq	00080008000800080h, 00080008000800080h
encorrelate_dummyalpha_bgrx	dq	0ff000000ff000000h, 0ff000000ff000000h

encorrelate_pshufb_xrgb		dq	006ff06ff02ff02ffh, 00eff0eff0aff0affh
encorrelate_add_xrgb		dq	08000800080008000h, 08000800080008000h
encorrelate_dummyalpha_xrgb	dq	0000000ff000000ffh, 0000000ff000000ffh

%macro ENCORRELATE_INPLACE_XRGB 3
%push
	MULTI_CONTEXT_XDEFINE procname, %1, bgrx, %2, havealpha, %3

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pBegin, pEnd, cbWidth, scbStride

%if %$bgrx
	movdqa		xmm2, [encorrelate_pshufb_bgrx]
	movdqa		xmm3, [encorrelate_add_bgrx]
 %if ! %$havealpha
	movdqa		xmm4, [encorrelate_dummyalpha_bgrx]
 %endif
%else
	movdqa		xmm2, [encorrelate_pshufb_xrgb]
	movdqa		xmm3, [encorrelate_add_xrgb]
 %if ! %$havealpha
	movdqa		xmm4, [encorrelate_dummyalpha_xrgb]
 %endif
%endif
	mov			rsi, [rsp + %$pBegin]
	mov			rax, [rsp + %$pEnd]
	mov			rbx, [rsp + %$cbWidth]
	mov			rcx, [rsp + %$scbStride]
	sub			rcx, rbx
	sub			rbx, 16 - 4

.label1:
	lea			rdx, [rsi+rbx]
.label0:
	lddqu		xmm0, [rsi]		; xmm0 = A3 R3 G3 B3 A2 R2 G2 B2 A1 R1 G1 B1 A0 R0 G0 B0 (bgrx)
	movdqa		xmm1, xmm0
	pshufb		xmm1, xmm2
	paddb		xmm0, xmm3
	paddb		xmm0, xmm1
%if ! %$havealpha
	por			xmm0, xmm4
%endif
	movdqu		[rsi], xmm0

	add			rsi, 16
	cmp			rsi, rdx
	jb			.label0

	add			rdx, 16 - 4

.label3:
	cmp			rsi, rdx
	jae			.label2

	movd		xmm0, [rsi]
	movdqa		xmm1, xmm0
	pshufb		xmm1, xmm2
	paddb		xmm0, xmm3
	paddb		xmm0, xmm1
%if ! %$havealpha
	por			xmm0, xmm4
%endif
	movd		[rsi], xmm0

	add			rsi, 4
	jmp			.label3

.label2:
	add			rsi, rcx
	cmp			rsi, rax
	jne			.label1

	SIMPLE_EPILOGUE
%pop
%endmacro

ENCORRELATE_INPLACE_XRGB	ssse3_EncorrelateInplaceBGRX, 1, 0
ENCORRELATE_INPLACE_XRGB	ssse3_EncorrelateInplaceXRGB, 0, 0
ENCORRELATE_INPLACE_XRGB	ssse3_EncorrelateInplaceBGRA, 1, 1
ENCORRELATE_INPLACE_XRGB	ssse3_EncorrelateInplaceARGB, 0, 1
