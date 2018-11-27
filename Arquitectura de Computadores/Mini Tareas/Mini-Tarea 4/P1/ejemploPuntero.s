
	pushq	%rbp

	movq	%rsp, %rbp

	subq	$32, %rsp
	leaq	L_.str(%rip), %rdi
	leaq	-4(%rbp), %rax
	movl	$1, -4(%rbp)
	movq	%rax, -16(%rbp)
	movl	-4(%rbp), %esi
	movb	$0, %al
	callq	_printf
	xorl	%esi, %esi
	movl	%eax, -20(%rbp)         ## 4-byte Spill
	movl	%esi, %eax
	addq	$32, %rsp
	popq	%rbp
	retq
	.cfi_endproc

	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"%i\n"


.subsections_via_symbols
