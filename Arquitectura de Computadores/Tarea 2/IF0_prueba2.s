.LC0:
        .string "%u\n"
        .text
        .global main
        .type main, %function
main:
        stmfd sp!, {lr}
        mov r1, #1
        stmfd r13!, {r1}
        ldmfd r13!, {r1}
        cmp r1, #0
        beq .cond1
        bne .cond2
.cond1:
        mov r2, #5
        stmfd r13!, {r2}
        bl .regreso1
.cond2:
        mov r3, #6
        stmfd r13!, {r3}
        mov r4, #7
        stmfd r13!, {r4}
        ldmfd r13!, {r4}
        ldmfd r13!, {r3}
        add r3, r4, r3
        stmfd r13!, {r3}
        bl .regreso1
.regreso1:
        mov r4, #10
        stmfd r13!, {r4}
        ldmfd r13!, {r4}
        ldmfd r13!, {r3}
        add r3, r4, r3
        stmfd r13!, {r3}
        ldmfd r13!, {r0}
        mov r1, r0
        ldr r0, .L1
        bl printf
        ldmfd sp!, {pc}
.L1:
        .word .LC0
