.data
string: 
	.ascii "%d\n"
.text
.global main
main: 
	stmfd sp!, {fp,lr}
	mov r0, #3
	stmfd r13!, {r0}
	mov r1, #8
	stmfd r13!, {r1}
	ldmfd r13!, {r1}
	ldmfd r13!, {r0}
	add r0, r1, r0
	stmfd r13!, {r0}
	mov r1, #6
	stmfd r13!, {r1}
	ldmfd r13!, {r1}
	ldmfd r13!, {r0}
	add r0, r1, r0
	stmfd r13!, {r0}
	ldmfd r13!, {r1}
	ldr r0, =string
	bl printf
	ldmfd sp!, {fp, pc}
