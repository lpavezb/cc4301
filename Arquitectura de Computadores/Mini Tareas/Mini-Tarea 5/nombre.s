@Lineas de rigor
.text
.global %main
.type main, %function

main:
  stmfd sp!, {lr}   @Hace la inicializacion del stack
  mov r1, #37       @Guarda el largo de la palabra a escribir
  ldr r0, palabra   @Carga la palabra a escribir en r0
  bl printf         @Imprime la palabra llamando a la subrutina printf
  mov r0, #0        @Coloca un 0 en r0, para retornarlo
  ldmfd sp!, {pc}   @Hace pop del stack

nombre:
  @El ascii de la palabra a escribir
  .ascii “Hello, Gabriel Andres Azocar Carcamo”

palabra:
  @Una forma de cargar la palabra en nombre
  .word nombre