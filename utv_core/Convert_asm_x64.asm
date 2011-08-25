; 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ
; $Id$

include Common_asm_x64.inc

_TEXT_ASM	SEGMENT	page 'CODE'

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

CONVERT_ULY2_TO_BOTTOMUP_RGB	macro	procname, rgb32

; procname(uint8_t *pDstBegin, uint8_t *pDstEnd, const uint8_t *pYBegin, const uint8_t *pUBegin, const uint8_t *pVBegin, DWORD dwStride, DWORD dwDataStride);
public	&procname
&procname	proc

	STD_PROLOG	0
$pDstBegin    = argsoffset +  0
$pDstEnd      = argsoffset +  8
$pYBegin      = argsoffset + 16
$pUBegin      = argsoffset + 24
$pVBegin      = argsoffset + 32
$dwStride     = argsoffset + 40
$dwDataStride = argsoffset + 48

	mov			rbp, qword ptr [rsp + $pYBegin]
	mov			rbx, qword ptr [rsp + $pUBegin]
	mov			rcx, qword ptr [rsp + $pVBegin]
	mov			rdx, qword ptr [rsp + $dwStride]
	mov			rsi, qword ptr [rsp + $pDstEnd]			; esi なのに dst のポインタを保持するのは気持ちが悪いが。
	sub			rsi, rdx
	add			rsi, qword ptr [rsp + $dwDataStride]

	align	64
label0:
	mov			rdi, rsi
	sub			rdi, qword ptr [rsp + $dwDataStride]

	; align	64	; さすがに入れすぎな気がするのでコメントアウト。
label1:
	pxor		xmm3, xmm3				; xmm3 = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

	movd		xmm0, dword ptr [rbp]	; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 Y3 Y2 Y1 Y0
	punpcklbw	xmm0, xmm3				; xmm0 = 00 00 00 00 00 00 00 00 00 Y3 00 Y2 00 Y1 00 Y0
	psubw		xmm0, oword ptr [yoff]	; xmm0 = 00 00 00 00 00 00 00 00 ---Y3 ---Y2 ---Y1 ---Y0 (de-offset)
	punpcklwd	xmm0, xmm3				; xmm0 = 00 00 ---Y3 00 00 ---Y2 00 00 ---Y1 00 00 ---Y0 (de-offset)

	movd		xmm1, dword ptr [rbx]	; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 U6 U4 U2 U0
	movd		xmm2, dword ptr [rcx]	; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 V6 V4 V2 V0
	punpcklbw	xmm1, xmm2				; xmm1 = 00 00 00 00 00 00 00 00 V6 U6 V4 U4 V2 U2 V0 U0
	punpcklbw	xmm1, xmm3				; xmm1 = 00 V6 00 U6 00 V4 00 U4 00 V2 00 U2 00 V0 00 U0
	psubw		xmm1, oword ptr [uvoff]	; xmm1 = 00 V6 00 U6 00 V4 00 U4 ---V2 ---U2 ---V0 ---U0 (de-offset)
	punpckldq	xmm1, xmm1				; xmm1 = ---V2 ---U2 ---V2 ---U2 ---V0 ---U0 ---V0 ---U0 (de-offset)
	paddw		xmm1, xmm1

	pmaddwd		xmm0, oword ptr [y2rgb]

	movdqa		xmm3, xmm1
	pmaddwd		xmm3, oword ptr [uv2r]
	paddd		xmm3, xmm0				; xmm3 = -R3-------- -R2-------- -R1-------- -R0--------
	psrad		xmm3, 14				; xmm3 = ---------R3 ---------R2 ---------R1 ---------R0

	movdqa		xmm2, xmm1
	pmaddwd		xmm2, oword ptr [uv2b]
	paddd		xmm2, xmm0				; xmm2 = -B3-------- -B2-------- -B1-------- -B0--------
	psrad		xmm2, 14				; xmm2 = ---------B3 ---------B2 ---------B1 ---------B0

	pmaddwd		xmm1, oword ptr [uv2g]
	paddd		xmm1, xmm0				; xmm1 = -G3-------- -G2-------- -G1-------- -G0--------
	psrad		xmm1, 14				; xmm1 = ---------G3 ---------G2 ---------G1 ---------G0

	pxor		xmm0, xmm0				; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	packssdw	xmm1, xmm3				; xmm1 = ---R3 ---R2 ---R1 ---R0 ---G3 ---G2 ---G1 ---G0
	packssdw	xmm2, xmm0				; xmm2 = 00 00 00 00 00 00 00 00 ---B3 ---B2 ---B1 ---B0
	pmaxsw		xmm1, xmm0				; 計算過程でマイナスになることがあるので、
	pmaxsw		xmm2, xmm0				; ここの pmaxsw xmmN, xmm0 は必要。
	punpcklwd	xmm2, xmm1				; xmm2 = ---G3 ---B3 ---G2 ---B2 ---G1 ---B1 ---G0 ---B0
	punpckhwd	xmm1, xmm0				; xmm1 = 00 00 ---R3 00 00 ---R2 00 00 ---R1 00 00 ---R0
if &rgb32
	pcmpeqb		xmm0, xmm0				; xmm0 = ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
	pslld		xmm0, 24				; xmm0 = ff 00 00 00 ff 00 00 00 ff 00 00 00 ff 00 00 00
endif
	packuswb	xmm2, xmm2				; xmm2 = XX XX XX XX XX XX XX XX G3 B3 G2 B2 G1 B1 G0 B0
	packuswb	xmm1, xmm1				; xmm2 = XX XX XX XX XX XX XX XX 00 R3 00 R2 00 R1 00 R0
	punpcklwd	xmm2, xmm1				; xmm2 = 00 R3 G3 B3 00 R2 G2 B2 00 R1 G1 B1 00 R0 G0 B0
if &rgb32
	por			xmm2, xmm0				; xmm2 = ff R3 G3 B3 ff R2 G2 B2 ff R1 G1 B1 ff R0 G0 B0
endif

if &rgb32
	add			rdi, 16
	add			rbp, 4
	add			rbx, 2
	add			rcx, 2
	cmp			rdi, rsi
	ja			label2
	movdqu		oword ptr [rdi-16], xmm2
	jne			label1
	jmp			label3

label2:
	sub			rdi, 16
	sub			rbp, 4
	sub			rbx, 2
	sub			rcx, 2
	test		rdx, 8
	jz			label4
	movq		qword ptr [rdi], xmm2
	psrldq		xmm2, 8
	add			rdi, 8
	add			rbp, 2
	add			rbx, 1
	add			rcx, 1
label4:
	test		rdx, 4
	jz			label3
	movd		dword ptr [rdi], xmm2
	add			rdi, 4
	add			rbp, 2
	add			rbx, 1
	add			rcx, 1
else
	movd		eax, xmm2
	psrldq		xmm2, 4
	mov			word ptr [rdi], ax
	shr			eax, 16
	mov			byte ptr [rdi+2], al
	add			rdi, 3
	add			rbp, 2
	add			rbx, 1
	add			rcx, 1
	cmp			rdi, rsi
	jae			label3
	movd		eax, xmm2
	psrldq		xmm2, 4
	mov			word ptr [rdi], ax
	shr			eax, 16
	mov			byte ptr [rdi+2], al
	add			rdi, 3
	cmp			rdi, rsi
	jae			label3
	movd		eax, xmm2
	psrldq		xmm2, 4
	mov			word ptr [rdi], ax
	shr			eax, 16
	mov			byte ptr [rdi+2], al
	add			rdi, 3
	add			rbp, 2
	add			rbx, 1
	add			rcx, 1
	cmp			rdi, rsi
	jae			label3
	movd		eax, xmm2
	mov			word ptr [rdi], ax
	shr			eax, 16
	mov			byte ptr [rdi+2], al
	add			rdi, 3
	cmp			rdi, rsi
	jb			label1
endif

label3:
	sub			rsi, rdx
	cmp			rsi, qword ptr [rsp + $pDstBegin]
	ja			label0

	STD_EPILOG
	ret

&procname	endp

	endm

CONVERT_ULY2_TO_BOTTOMUP_RGB	x64_sse2_ConvertULY2ToBottomupRGB24, 0
CONVERT_ULY2_TO_BOTTOMUP_RGB	x64_sse2_ConvertULY2ToBottomupRGB32, 1




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

CONVERT_BOTTOMUP_RGB_TO_ULY2	macro	procname, rgb32

; procname(uint8_t *pYBegin, uint8_t *pUBegin, uint8_t *pVBegin, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, DWORD dwStride, DWORD dwDataStride);
public	&procname
&procname	proc

	STD_PROLOG	0
$pYBegin      = argsoffset +  0
$pUBegin      = argsoffset +  8
$pVBegin      = argsoffset + 16
$pSrcBegin    = argsoffset + 24
$pSrcEnd      = argsoffset + 32
$dwStride     = argsoffset + 40
$dwDataStride = argsoffset + 48

	mov			rdi, qword ptr [rsp + $pYBegin]
	mov			rbx, qword ptr [rsp + $pUBegin]
	mov			rcx, qword ptr [rsp + $pVBegin]
	mov			rbp, qword ptr [rsp + $pSrcEnd]
	sub			rbp, qword ptr [rsp + $dwStride]
	add			rbp, qword ptr [rsp + $dwDataStride]

	align	64
label0:
	mov			rsi, rbp
	sub			rsi, qword ptr [rsp + $dwDataStride]
if &rgb32
	add			rsi, 4
else
	add			rsi, 3
endif

	;align	64
label1:
if &rgb32
	movd		xmm0, dword ptr [rsi-4]				; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0
	movd		xmm1, dword ptr [rsi]				; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R1 G1 B1
else
	movd		xmm0, dword ptr [rsi-3]				; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0
	movd		xmm1, dword ptr [rsi-1]				; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 R1 G1 B1 XX
	psrld		xmm1, 8								; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 00 R1 G1 B1
endif

	punpcklbw	xmm0, xmm1							; xmm0 = 00 00 00 00 00 00 00 00 00 XX R1 R0 G1 G0 B1 B0
label3:
	pxor		xmm1, xmm1
	punpcklbw	xmm0, xmm1							; xmm0 = 00 00 00 XX 00 R1 00 R0 00 G1 00 G0 00 B1 00 B0

	pshufd		xmm1, xmm0, 055h					; xmm1 = 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0
	pshufd		xmm2, xmm0, 0aah					; xmm2 = 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0
	pshufd		xmm0, xmm0, 000h					; xmm0 = 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0

	pmaddwd		xmm0, oword ptr [b2yuv]				; xmm0 = ----B2V---- ----B2U---- ----B2Y1--- ----B2Y0---
	pmaddwd		xmm1, oword ptr [g2yuv]				; xmm1 = ----G2V---- ----G2U---- ----G2Y1--- ----G2Y0---
	pmaddwd		xmm2, oword ptr [r2yuv]				; xmm2 = ----R2V---- ----R2U---- ----R2Y1--- ----R2Y0---

	paddd		xmm0, oword ptr [yuvoff]
	paddd		xmm2, xmm1
	paddd		xmm0, xmm2							; xmm0 = -----V----- -----U----- -----Y1---- -----Y0----

	psrld		xmm0, 14							; xmm0 = ---------V0 ---------U0 ---------Y1 ---------Y0
	packssdw	xmm0, xmm0							; xmm0 = XX XX XX XX XX XX XX XX ---V0 ---U0 ---Y1 ---Y0
	packuswb	xmm0, xmm0							; xmm0 = XX XX XX XX XX XX XX XX XX XX XX XX V0 U0 Y1 Y0
	movd		eax, xmm0
	mov			word ptr [rdi], ax
	shr			eax, 16
	mov			byte ptr [rbx], al
	mov			byte ptr [rcx], ah

	add			rdi, 2
	add			rbx, 1
	add			rcx, 1
if &rgb32
	add			rsi, 8
else
	add			rsi, 6
endif
	cmp			rsi, rbp
	jb			label1
	ja			label2

if &rgb32
	movd		xmm0, dword ptr [rsi-4]				; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0
else
	movd		xmm0, dword ptr [rsi-3]				; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0
endif

	punpcklbw	xmm0, xmm0							; xmm0 = 00 00 00 00 00 00 00 00 XX XX R0 R0 G0 G0 B0 B0
	jmp			label3

label2:
	sub			rbp, qword ptr [rsp + $dwStride]
	cmp			rbp, qword ptr [rsp + $pSrcBegin]
	ja			label0

	STD_EPILOG
	ret

&procname	endp

	endm

CONVERT_BOTTOMUP_RGB_TO_ULY2	x64_sse2_ConvertBottomupRGB24ToULY2, 0
CONVERT_BOTTOMUP_RGB_TO_ULY2	x64_sse2_ConvertBottomupRGB32ToULY2, 1

end
