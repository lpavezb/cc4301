
	pushq	%rbp
	movq	%rsp, %rbp
	xorl	%eax, %eax
	movb	$49, -1(%rbp)
	popq	%rbp
	retq
