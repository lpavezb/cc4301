#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "sexp.h"
#include "sexp_ops.h"
#include <string.h>

int actual = -1;

void escribir(elt *elt, unsigned int level, FILE *fo)
{
    if (elt == NULL)
        return;
    if (elt->val != NULL) //elt->val es INT_CONST, ADD, SUB o un INT
    {
      //Rutina que escribe el push de un INT en el stack
      if (strncmp(elt->val,"INT_CONST", 10) == 0) {
        actual++; //Seteamos el numero del registro que usaremos
        elt = elt->next; //Vamos al valor del numero, que es a continuacion de INT_CONST
        std::string valor = elt->val; //Obtenemos el valor
        
        //Generamos las lineas de ARM
        //Esto esta mejor explicado en el pdf
        fprintf(fo, "        %s%i%s%s\n", "mov r",actual," ,#", valor.c_str());
        fprintf(fo, "        %s%i%s\n", "stmfd r13!, {r", actual,"}");
      }
      
      
      //Rutina que escribe la operacion ADD entre los dos primeros valores del stack
      else if (strncmp(elt->val, "ADD", 4) == 0) {
        fprintf(fo, "        %s%i%s\n", "ldmfd r13!, {r", actual,"}"); //Hacemos un pop para obtener el elemento que esta primero (en el tope) del stack
        fprintf(fo, "        %s%i%s\n", "ldmfd r13!, {r", actual - 1, "}"); //Repetimos para encontrar el segundo
        fprintf(fo, "        %s%i%s%i%s%i\n", "add r", actual - 1, ", r", actual, ", r", actual - 1); //Producimos la suma y la guardamos en el registro con numero menor
        fprintf(fo, "        %s%i%s\n", "stmfd r13!, {r", actual - 1, "}"); //Hacemos push del resultado
        actual--; //Bajamos el indice actual, ya que de dos registros pasamos a solo uno
      }
      
      
      //Rutina que escribe la operacion SUB entre los dos primeros valores del stack
      else if (strncmp(elt->val, "SUB", 4) == 0) {
        fprintf(fo, "        %s%i%s\n", "ldmfd r13!, {r", actual,"}"); //Hacemos un pop para obtener el elemento que esta primero (en el tope) del stack
        fprintf(fo, "        %s%i%s\n", "ldmfd r13!, {r", actual - 1, "}"); //Repetimos para encontrar el segundo
        fprintf(fo, "        %s%i%s%i%s%i\n", "sub r", actual - 1, ", r", actual, ", r", actual - 1); //Producimos la resta y la guardamos en el registro con numero menor
        fprintf(fo, "        %s%i%s\n", "stmfd r13!, {r", actual - 1, "}"); //Hacemos push del resultado
        actual--; //Bajamos el indice actual, ya que de dos registros pasamos a solo uno
      }
    }
  
    //Seguimos leyendo las instrucciones que siguen
    escribir(elt->list, level+1, fo);
    escribir(elt->next, level, fo);
}

int main(int argc, const char * argv[])
{
    char linebuf[BUFSIZ];
    FILE *fp;
    FILE *fo;
    char *status;
    sexp_t *sx;
    
    fp = fopen("instrucciones.txt","r+");
    fo = fopen("armTraduccion.s", "w+");
  
    //Inicio, lineas del principio
    fprintf(fo, ".LC0:\n");
    fprintf(fo, "        %s\n", ".string \"%u\\n\""); //Linea para imprimir el numero resultante
    fprintf(fo, "        %s\n", ".text");
    fprintf(fo, "        %s\n", ".global main");
    fprintf(fo, "        %s\n", ".type main, %function");
    fprintf(fo, "%s\n", "main:");
    fprintf(fo, "       %s\n", "stmfd sp!, {lr}"); //Inicializacion del stack

     while (1) {
        status = fgets(linebuf, BUFSIZ, fp);
        
        /* if not EOF and status was NULL, something bad happened. */
        if (status != linebuf) {
            printf("Error encountered on fgets.\n");
            exit(EXIT_FAILURE);
        }
        
        sx = parse_sexp(linebuf, BUFSIZ);
        escribir(sx->list, 1, fo);
        destroy_sexp(sx);
        
        if (feof(fp) != 0) break;
    }
    
    fclose(fp);
  
    //Final del programa (cierre y mostrar en pantalla)
    fprintf(fo, "        %s\n", "ldmfd r13!, {r0}");
    fprintf(fo, "        %s\n", "mov r1 , r0");
    fprintf(fo, "        %s\n", "ldr r0, .L1");
    fprintf(fo, "        %s\n", "bl printf");
    fprintf(fo, "        %s\n", "ldmfd sp!, {pc}");
  
    //Label que ayuda a imprimir en pantalla
    fprintf(fo, ".L1:\n");
    fprintf(fo, "        %s\n", ".word .LC0");
  

    return 0;
}
