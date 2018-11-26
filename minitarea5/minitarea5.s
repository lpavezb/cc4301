.data @aqui se declaran variables

string:
    .ascii      "Hello, Lukas Dario Pavez Bahamondes\n" @string a imprimir en pantalla
len = . - string @largo del string

.text @desde aqui se va a ejecutar

.globl main @funcion global main
main: 
    mov     %r0, #1       @se va a escribir en stdout
    ldr     %r1, = string @string se guarda en el registro R1
    ldr     %r2, = len    @largo del string se guarda en registro R2
    mov     %r7, #4       @indica que se va a llamar a funcion write(R0,R1,R2) 
    swi     #0            @se llama a write
    mov     %r0, #0       @se va a llamar a exit(R0), por lo que R0 = 0 (exit(0))
    mov     %r7, #1       @indica que se va a llamar a exit
    swi     #0            @se llama a exit(0)
