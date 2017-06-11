/* •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

	uint8_t *ret;
	void* clobber;
#ifdef __GNUC__
	asm volatile (
	R"(
	.intel_syntax noprefix

.macro HUFFMAN_DECODE_8 bmi2

	# rdi = pDstBegin
	# rax = pDstEnd
	# rsi = pSrcBegin
	# rbx = pDecodeTable

	mov			r8, rax

	mov			cl, 32
	mov			edx, dword ptr [rsi]
	add			rsi, 4

.irp perbyte, 0, 1
.if !\perbyte
	sub			r8, 4
.else
	add			r8, 4
.endif

	.balign		64
1:
	cmp			rdi, r8
	jae			3f

	cmp			cl, 32
	jb			4f
	sub			cl, 32
	shl			rdx, 32
	mov			r9d, dword ptr [rsi]
	or			rdx, r9
	add			rsi, 4

4:
.if \bmi2
	shlx		rax, rdx, rcx
.else
	mov			rax, rdx
	shl			rax, cl
.endif
	shr			rax, 20+32
	mov			r11d, dword ptr [rbx + 8192 + rax*4]						# pDecodeTable->MultiSpeedTable_sym[rax]
	movzx		eax, word ptr [rbx + rax*2]									# pDecodeTable->MultiSpeedTable_cs[rax]
	cmp			ah, 255
	jne			0f

.if \bmi2
	shlx		rax, rdx, rcx
.else
	mov			rax, rdx
	shl			rax, cl
	mov			r15d, ecx
.endif
	shr			rax, 32
	or			eax, 1
	bsr			r10, rax
.if \bmi2
	movzx		r13d, byte ptr [rbx + 8192 + 16384 + r10]					# pDecodeTable->nCodeShift[r10]
	shrx		eax, eax, r13d
.else
	mov			cl, byte ptr [rbx + 8192 + 16384 + r10]						# pDecodeTable->nCodeShift[r10]
	shr			eax, cl
	mov			ecx, r15d
.endif
	mov			r10d, dword ptr [rbx + 8192 + 16384 + 32 + r10*4]			# pDecodeTable->dwSymbolBase[r10]
	add			r10, rax
	mov			eax, dword ptr [rbx + 8192 + 16384 + 32 + 4*32 + r10*2]		# pDecodeTable->SymbolAndCodeLength[r10]
	mov			r11d, eax
	mov			al, 1

0:
	add			cl, ah

.if !\perbyte
	mov			dword ptr [rdi], r11d
	movzx		rax, al
	add			rdi, rax
	jmp			1b
.else
5:
	mov			byte ptr [rdi], r11b
	shr			r11d, 8
	add			rdi, 1
	cmp			rdi, r8
	jae			3f
	sub			al, 1
	ja			5b
	jmp			1b
.endif

3:
.endr

	mov			rax, rdi
.endm
	HUFFMAN_DECODE_8 )" ARGS R"(
.purgem HUFFMAN_DECODE_8
	)"
		: "=a"(ret), "=D"(clobber), "=S"(clobber), "=b"(clobber)
		: "D"(pDstBegin), "a"(pDstEnd), "S"(pSrcBegin), "b"(pDecodeTable)
		: "rcx", "rdx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15");
#endif
	return ret;

#undef ARGS
