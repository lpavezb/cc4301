.LC0:
        .string "%u\n"
        .text
        .global main
        .type main, %function
.L2:
        mov r1 ,#1
        stmfd r13!, {r1}
        mov r2 ,#2
        stmfd r13!, {r2}
        ldmfd r13!, {r2}
        ldmfd r13!, {r1}
        add r1, r2, r1
        stmfd r13!, {r1}
        ldmfd r13!, {r0}
        b .regreso1
main:
        stmfd sp!, {lr}
        ldr r0, = .L2
        stmfd r13!, {r0}
        ldmfd r13!, {r0, r1}
        stmfd r13!, {r0}
        bx r1
.regreso1:
        ldmfd r13!, {r0}
        mov r1, r0
        ldr r0, .L1
        bl printf
        ldmfd sp!, {pc}
.L1:
        .word .LC0
