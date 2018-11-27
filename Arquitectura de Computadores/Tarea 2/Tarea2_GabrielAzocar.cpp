#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "/Users/GabrielAzocar/sexpr-1.3/src/sexp.h"
#include "/Users/GabrielAzocar/sexpr-1.3/src/sexp_ops.h"
#include <string.h>

int actualRegistro = 0;
int actualCond = 1;
int actualFun = 2;
int regreso = 1;
bool buscandoReturn = false;
char* archivo = "IF0_Prueba1.txt"; //Nombre del archivo de texto a leer

//Funcion que escribe el programa principal (main)
void escribir(elt *elt, unsigned int level, FILE *fo)
{
    if (elt == NULL)
        return;
    if (elt->val != NULL) //elt->val es INT_CONST, ADD, SUB o un INT
    {
      //Rutina que escribe el push de un INT en el stack
      if (strncmp(elt->val,"INT_CONST", 10) == 0 && !buscandoReturn) {
        actualRegistro++; //Seteamos el numero del registro que usaremos
        elt = elt->next; //Vamos al valor del numero, que es a continuacion de INT_CONST
        std::string valor = elt->val; //Obtenemos el valor
        
        //Generamos las lineas de ARM
        //Esto esta mejor explicado en el pdf
        fprintf(fo, "        %s%i%s%s\n", "mov r",actualRegistro,", #", valor.c_str());
        fprintf(fo, "        %s%i%s\n", "stmfd r13!, {r", actualRegistro, "}");
      }

      //Funcion que hace push de la funcion indicada
      else if (strncmp(elt->val, "FUN", 4) == 0) {
        fprintf(fo, "        %s%i%s%i\n", "ldr r", actualRegistro, ", = .L", actualFun);
        fprintf(fo, "        %s%i%s\n", "stmfd r13!, {r", actualRegistro, "}");
        buscandoReturn = true;
      }

      //Funcion que crea saltos condicionales
      else if (strncmp(elt->val, "IF0", 4) == 0) {
        fprintf(fo, "        %s%i%s\n", "ldmfd r13!, {r", actualRegistro, "}");
        fprintf(fo, "        %s%i%s\n", "cmp r", actualRegistro, ", #0");
        fprintf(fo, "        %s%i\n", "beq .cond", actualCond);
        fprintf(fo, "        %s%i\n", "bne .cond", actualCond + 1);
        fprintf(fo,"%s%i%s\n", ".cond", actualCond, ":");
        elt = elt->next;
        escribir(elt->list, level + 1, fo);
        fprintf(fo,"        %s%i\n", "bl .regreso", regreso);
        fprintf(fo,"%s%i%s\n", ".cond", actualCond + 1, ":");
        elt = elt->next;
        escribir(elt->list, level + 1, fo);
        fprintf(fo,"        %s%i\n", "bl .regreso", regreso);
        actualCond += 2;
        fprintf(fo,"%s%i%s\n", ".regreso", regreso, ":");
        elt = elt->list->list;
      }

      else if (strncmp(elt->val, "RETURN", 7) == 0) {
        buscandoReturn = false;
      }

      else if (strncmp(elt->val, "APPLY", 6) == 0 && !buscandoReturn) {
        fprintf(fo, "        %s%i%s%i%s\n", "ldmfd r13!, {r", actualRegistro, ", r", actualRegistro + 1, "}");
        fprintf(fo, "        %s%i%s\n", "stmfd r13!, {r", actualRegistro, "}");
        fprintf(fo, "        %s%i\n", "bx r", actualRegistro + 1);
        fprintf(fo, "%s%i%s\n", ".regreso", regreso, ":");
        regreso++;
      }
            
      //Rutina que escribe la operacion ADD entre los dos primeros valores del stack
      else if (strncmp(elt->val, "ADD", 4) == 0 && !buscandoReturn) {
        fprintf(fo, "        %s%i%s\n", "ldmfd r13!, {r", actualRegistro,"}"); //Hacemos un pop para obtener el elemento que esta primero (en el tope) del stack
        fprintf(fo, "        %s%i%s\n", "ldmfd r13!, {r", actualRegistro - 1, "}"); //Repetimos para encontrar el segundo
        fprintf(fo, "        %s%i%s%i%s%i\n", "add r", actualRegistro - 1, ", r", actualRegistro, ", r", actualRegistro - 1); //Producimos la suma y la guardamos en el registro con numero menor
        fprintf(fo, "        %s%i%s\n", "stmfd r13!, {r", actualRegistro - 1, "}"); //Hacemos push del resultado
        actualRegistro--; //Bajamos el indice actual, ya que de dos registros pasamos a solo uno
      }
      
      
      //Rutina que escribe la operacion SUB entre los dos primeros valores del stack
      else if (strncmp(elt->val, "SUB", 4) == 0 && !buscandoReturn) {
        fprintf(fo, "        %s%i%s\n", "ldmfd r13!, {r", actualRegistro,"}"); //Hacemos un pop para obtener el elemento que esta primero (en el tope) del stack
        fprintf(fo, "        %s%i%s\n", "ldmfd r13!, {r", actualRegistro - 1, "}"); //Repetimos para encontrar el segundo
        fprintf(fo, "        %s%i%s%i%s%i\n", "sub r", actualRegistro - 1, ", r", actualRegistro, ", r", actualRegistro - 1); //Producimos la resta y la guardamos en el registro con numero menor
        fprintf(fo, "        %s%i%s\n", "stmfd r13!, {r", actualRegistro - 1, "}"); //Hacemos push del resultado
        actualRegistro--; //Bajamos el indice actual, ya que de dos registros pasamos a solo uno
      }

    }

    //Seguimos leyendo las instrucciones que siguen
    escribir(elt->list, level + 1, fo);
    escribir(elt->next, level, fo);
}

//Funcion que define las funciones antes de la compilación principal
void escribirFun(elt *elt, unsigned int level, FILE *fo) 
{
    if (elt == NULL) {
        return; 
    }
    if(elt->val != NULL) {
        //Si nos encontramos con la palabra FUN, creamos una nueva etiqueta para la funcion
        //Y colocamos que estamos en busca de un RETURN
        if (strncmp(elt->val, "FUN", 4) == 0) {
            buscandoReturn = true;
            fprintf(fo, "%s%i%s\n", ".L", actualFun, ":");
        }

        //Si encontramos un return, terminamos la declaracion de la funcion y colocamos que ya 
        //no estamos buscando un RETURN
        else if (strncmp(elt->val, "RETURN", 7) == 0) {
            fprintf(fo, "        %s\n", "ldmfd r13!, {r0}");
            fprintf(fo, "        %s%i\n", "b .regreso", regreso);
            regreso++; 
            buscandoReturn = false;
        }

        //Si se esta buscando un RETURN, entonces se crea el cuerpo de la funcion
        if(buscandoReturn) {
            //Rutina que escribe el push de un INT en el stack
            if (strncmp(elt->val,"INT_CONST", 10) == 0) {
                actualRegistro++; //Seteamos el numero del registro que usaremos
                elt = elt->next; //Vamos al valor del numero, que es a continuacion de INT_CONST
                std::string valor = elt->val; //Obtenemos el valor

                //Generamos las lineas de ARM
                //Esto esta mejor explicado en el pdf
                fprintf(fo, "        %s%i%s%s\n", "mov r",actualRegistro," ,#", valor.c_str());
                fprintf(fo, "        %s%i%s\n", "stmfd r13!, {r", actualRegistro, "}");
            }

            //Rutina que escribe la operacion ADD entre los dos primeros valores del stack
            else if (strncmp(elt->val, "ADD", 4) == 0) {
                fprintf(fo, "        %s%i%s\n", "ldmfd r13!, {r", actualRegistro,"}"); //Hacemos un pop para obtener el elemento que esta primero (en el tope) del stack
                fprintf(fo, "        %s%i%s\n", "ldmfd r13!, {r", actualRegistro - 1, "}"); //Repetimos para encontrar el segundo
                fprintf(fo, "        %s%i%s%i%s%i\n", "add r", actualRegistro - 1, ", r", actualRegistro, ", r", actualRegistro - 1); //Producimos la suma y la guardamos en el registro con numero menor
                fprintf(fo, "        %s%i%s\n", "stmfd r13!, {r", actualRegistro - 1, "}"); //Hacemos push del resultado
                actualRegistro--; //Bajamos el indice actual, ya que de dos registros pasamos a solo uno
            }


            //Rutina que escribe la operacion SUB entre los dos primeros valores del stack
            else if (strncmp(elt->val, "SUB", 4) == 0) {
                fprintf(fo, "        %s%i%s\n", "ldmfd r13!, {r", actualRegistro,"}"); //Hacemos un pop para obtener el elemento que esta primero (en el tope) del stack
                fprintf(fo, "        %s%i%s\n", "ldmfd r13!, {r", actualRegistro - 1, "}"); //Repetimos para encontrar el segundo
                fprintf(fo, "        %s%i%s%i%s%i\n", "sub r", actualRegistro - 1, ", r", actualRegistro, ", r", actualRegistro - 1); //Producimos la resta y la guardamos en el registro con numero menor
                fprintf(fo, "        %s%i%s\n", "stmfd r13!, {r", actualRegistro - 1, "}"); //Hacemos push del resultado
                actualRegistro--; //Bajamos el indice actual, ya que de dos registros pasamos a solo uno
            }

            //Funcion que crea saltos condicionales
            else if (strncmp(elt->val, "IF0", 4) == 0) {
                fprintf(fo, "        %s%i%s\n", "ldmfd r13!, {r", actualRegistro, "}");
                fprintf(fo, "        %s%i%s\n", "cmp r", actualRegistro, ", #0");
                fprintf(fo, "        %s%i\n", "beq .cond", actualCond);
                fprintf(fo, "        %s%i\n", "bne .cond", actualCond + 1);
                fprintf(fo,"%s%i%s\n", ".cond", actualCond, ":");
                elt = elt->next;
                escribir(elt->list, level + 1, fo);
                fprintf(fo,"        %s%i\n", "bl .regreso", regreso);
                fprintf(fo,"%s%i%s\n", ".cond", actualCond + 1, ":");
                elt = elt->next;
                escribir(elt->list, level + 1, fo);
                fprintf(fo,"        %s%i\n", "bl .regreso", regreso);
                actualCond += 2;
                fprintf(fo,"%s%i%s\n", ".regreso", regreso, ":");
                elt = elt->list->list;
            }
        }
    }
    escribirFun(elt->list, level + 1, fo);
    escribirFun(elt->next, level, fo);
}

int main(int argc, const char * argv[])
{
    char linebuf[BUFSIZ];
    FILE *fp;
    FILE *fo;
    char *status;
    sexp_t *sx;
    
    fp = fopen(archivo, "r+");
    fo = fopen("armTraduccion.s", "w+");
  
    //Inicio, lineas del principio
    fprintf(fo, ".LC0:\n");
    fprintf(fo, "        %s\n", ".string \"%u\\n\""); //Linea para imprimir el numero resultante
    fprintf(fo, "        %s\n", ".text");
    fprintf(fo, "        %s\n", ".global main");
    fprintf(fo, "        %s\n", ".type main, %function");

    //Aqui definimos las funciones, para luego solo invocarlas por su nombre
     while (1) {
        status = fgets(linebuf, BUFSIZ, fp);
        
        /* if not EOF and status was NULL, something bad happened. */
        if (status != linebuf) {
            printf("Error encountered on fgets.\n");
            exit(EXIT_FAILURE);
        }
        
        sx = parse_sexp(linebuf, BUFSIZ);
        escribirFun(sx->list, 1, fo);
        destroy_sexp(sx);
        
        if (feof(fp) != 0) break;
    }

    //Reiniciamos los contadores
     actualRegistro = 0;
     actualCond = 1;
     actualFun = 2;
     regreso = 1;
     buscandoReturn = false;
     fp = fopen(archivo, "r+"); 
     fprintf(fo, "%s\n", "main:");
     fprintf(fo, "        %s\n", "stmfd sp!, {lr}"); //Inicializacion del stack

     //Esta parte corresponde a la creacion del programa principal
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
    fprintf(fo, "        %s\n", "mov r1, r0");
    fprintf(fo, "        %s\n", "ldr r0, .L1");
    fprintf(fo, "        %s\n", "bl printf");
    fprintf(fo, "        %s\n", "ldmfd sp!, {pc}");
  
    //Label que ayuda a imprimir en pantalla
    fprintf(fo, ".L1:\n");
    fprintf(fo, "        %s\n", ".word .LC0");
  

    return 0;
}
