	pushq	%rbp

	movq	%rsp, %rbp

	movl	$0, -4(%rbp)
	movl	$1, -8(%rbp)
	movl	$0, -12(%rbp)

LBB0_1:                                 ## =>This Inner Loop Header: Depth=1
	cmpl	$10, -12(%rbp)
	jge	LBB0_4
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	LBB0_1

LBB0_4:
	movl	-4(%rbp), %eax
	popq	%rbp
	retq

