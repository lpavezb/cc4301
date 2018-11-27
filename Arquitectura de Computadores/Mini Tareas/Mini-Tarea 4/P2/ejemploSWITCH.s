	pushq	%rbp

	movq	%rsp, %rbp

	subq	$16, %rsp
	movl	$0, -4(%rbp)
	movb	$66, -5(%rbp)
	movb	$1, %al
	testb	%al, %al
	jne	LBB0_2
	jmp	LBB0_1
LBB0_1:
	leaq	L_.str(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -12(%rbp)         ## 4-byte Spill
LBB0_2:
	leaq	L_.str.1(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -16(%rbp)         ## 4-byte Spill

	movl	-4(%rbp), %eax
	addq	$16, %rsp
	popq	%rbp
	retq

