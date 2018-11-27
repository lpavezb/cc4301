.LC0:
        .string "%u\n"
        .text
        .global main
        .type main, %function
main:
       stmfd sp!, {lr}
        mov r0 ,#3
        stmfd r13!, {r0}
        mov r1 ,#8
        stmfd r13!, {r1}
        mov r2 ,#6
        stmfd r13!, {r2}
        ldmfd r13!, {r2}
        ldmfd r13!, {r1}
        add r1, r2, r1
        stmfd r13!, {r1}
        ldmfd r13!, {r1}
        ldmfd r13!, {r0}
        sub r0, r1, r0
        stmfd r13!, {r0}
        ldmfd r13!, {r0}
        mov r1 , r0
        ldr r0, .L1
        bl printf
        ldmfd sp!, {pc}
.L1:
        .word .LC0
