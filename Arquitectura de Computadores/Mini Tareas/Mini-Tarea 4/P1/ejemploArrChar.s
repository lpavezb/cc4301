  pushq	%rbp
	movq	%rsp, %rbp
	xorl	%eax, %eax
	movb	$104, -2(%rbp)
	movb	$111, -1(%rbp)
	popq	%rbp
	retq
