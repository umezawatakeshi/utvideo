; ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe
; $Id$


%include "Common_asm_x64.mac"


section .text


%push
global cpuid
cpuid:
	SIMPLE_PROLOGUE 0, result, leaf, subleaf

	mov			eax, dword [rsp + %$leaf]
	mov			ecx, dword [rsp + %$subleaf]
	mov			rdi, qword [rsp + %$result]
	cpuid
	mov			dword [rdi   ], eax
	mov			dword [rdi+ 4], ebx
	mov			dword [rdi+ 8], ecx
	mov			dword [rdi+12], edx

	SIMPLE_EPILOGUE
%pop


%push
global xgetbv
xgetbv:
	SIMPLE_PROLOGUE 0, result, idx

	mov			ecx, dword [rsp + %$idx]
	mov			rdi, qword [rsp + %$result]
	xgetbv
	mov			dword [rdi   ], eax
	mov			dword [rdi+ 4], edx

	SIMPLE_EPILOGUE
%pop
