/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

	typedef typename std::remove_reference<decltype(*pDecodeTable)>::type decodetable_t;
	typedef typename std::remove_reference<decltype(*pDstBegin)>::type sym_t;
	sym_t *ret;
	void* clobber;
#if defined(__GNUC__) || defined(__clang__)
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

.irp persym, 0, 1
.if !\persym
	sub			r8, %c[sizeof_combined_t]
.else
	add			r8, %c[sizeof_combined_t]
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
	shr			rax, 64 - %c[lookup_bits]
.if %c[sizeof_combined_t] == 4
	mov			r11d, dword ptr [rbx + %c[offsetof_multispeedtable_sym] + rax*4]
.else
.err
.endif
	movzx		eax, word ptr [rbx + %c[offsetof_multispeedtable_cs] + rax*2]
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
	movzx		r13d, byte ptr [rbx + %c[offsetof_codeshift] + r10]
	shrx		eax, eax, r13d
.else
	mov			cl, byte ptr [rbx + %c[offsetof_codeshift] + r10]
	shr			eax, cl
	mov			ecx, r15d
.endif
	mov			r10d, dword ptr [rbx + %c[offsetof_symbolbase] + r10*4]
	add			r10, rax
.if %c[sizeof_combined_t] == 4
.if %c[sizeof_sym_t] == 1
	mov			eax, dword ptr [rbx + %c[offsetof_symbolandcodelength] + r10*2]
	mov			r11d, eax
.elseif %c[sizeof_sym_t] == 2
	mov			eax, dword ptr [rbx + %c[offsetof_symbolandcodelength] + r10*4]
	mov			r11d, eax
	shr			eax, 8
.else
.err
.endif
.else
.err
.endif
	mov			al, 1

0:
	add			cl, ah

.if !\persym
.if %c[sizeof_combined_t] == 4
	mov			dword ptr [rdi], r11d
.else
.err
.endif
	movzx		rax, al
.if %c[sizeof_sym_t] == 1
	add			rdi, rax
.elseif %c[sizeof_sym_t] == 2
	lea			rdi, [rdi + rax * 2]
.else
.err
.endif
	jmp			1b
.else
5:
.if %c[sizeof_sym_t] == 1
	mov			byte ptr [rdi], r11b
.elseif %c[sizeof_sym_t] == 2
	mov			word ptr [rdi], r11w
.else
.err
.endif
	shr			r11, 8 * %c[sizeof_sym_t]
	add			rdi, %c[sizeof_sym_t]
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
		: "D"(pDstBegin), "a"(pDstEnd), "S"(pSrcBegin), "b"(pDecodeTable),
		  [lookup_bits]"i"(decodetable_t::LOOKUP_BITS),
		  [offsetof_multispeedtable_cs]"i"(offsetof(decodetable_t, MultiSpeedTable_cs)),
		  [offsetof_multispeedtable_sym]"i"(offsetof(decodetable_t, MultiSpeedTable_sym)),
		  [offsetof_codeshift]"i"(offsetof(decodetable_t, nCodeShift)),
		  [offsetof_symbolbase]"i"(offsetof(decodetable_t, dwSymbolBase)),
		  [offsetof_symbolandcodelength]"i"(offsetof(decodetable_t, SymbolAndCodeLength)),
		  [sizeof_sym_t]"i"(sizeof(sym_t)),
		  [sizeof_combined_t]"i"(sizeof(decodetable_t::combined_t))
		: "rcx", "rdx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15");
#endif
	return ret;

#undef ARGS
