
	pushq	%rbp
	movq	%rsp, %rbp
	xorl	%eax, %eax
	movss	LCPI0_0(%rip), %xmm0
	movss	%xmm0, -4(%rbp)
	popq	%rbp
	retq

