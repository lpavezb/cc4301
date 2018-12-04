/**

SFSEXP: Small, Fast S-Expression Library version 1.3
Written by Matthew Sottile (mjsottile@gmail.com)

Copyright (2003-2006). The Regents of the University of California. This
material was produced under U.S. Government contract W-7405-ENG-36 for Los
Alamos National Laboratory, which is operated by the University of
California for the U.S. Department of Energy. The U.S. Government has rights
to use, reproduce, and distribute this software. NEITHER THE GOVERNMENT NOR
THE UNIVERSITY MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified to produce
derivative works, such modified software should be clearly marked, so as not
to confuse it with the version available from LANL.

Additionally, this library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 2.1 of the
License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, U SA

LA-CC-04-094

**/

/*
 * Simple interpreter of s-expressions
 */
#include <stdio.h>
#include <sexp.h>
#include <sexp_ops.h>
#include <string.h>


/******************
   eval function
*******************/
int fun_num = 0;
int reg_num = 0;
int label = 0;
void eval(sexp_t *exp, FILE *out) {
    char *v;

    /**
    * values understood here: 
    *   INT_CONST
    *   ADD
    *   SUB
    *   FUN
    *   RETURN
    *   APPLY
    *   IF0
    */
    if (exp->ty == SEXP_LIST) {
        if (exp->list->ty == SEXP_VALUE)
            v = exp->list->val;
        else return;
    } else return;
    
    if (strcmp(v,"INT_CONST") == 0) {
        char *s = exp->list->next->val; // valor de la nueva constante
        char reg[4];
        sprintf(reg, "r%d", reg_num); // registro a utilizar (r{reg_num})
        printf("\tmov %s, #%s\n", reg, s); // comando mov
        printf("\tstmfd r13!, {%s}\n", reg); //comando stmdf
        reg_num++; // se suma 1 a los registros utilizados

    } else if (strcmp(v,"ADD") == 0) {
        reg_num--; // se vuelve al ultimo registro utilizado
        if(reg_num < 1){
            printf("Error: Debe haber al menos 2 valores apilados\n");
            exit(0);
        }
        char reg1[4];
        char reg2[4];
        sprintf(reg1, "r%d", reg_num);
        sprintf(reg2, "r%d", reg_num-1);
        printf("\tldmfd r13!, {%s}\n",reg1);
        printf("\tldmfd r13!, {%s}\n",reg2);
        printf("\tadd %s, %s, %s\n", reg2, reg1, reg2);
        printf("\tstmfd r13!, {%s}\n",reg2);
        // al final reg_num queda 1 mas que los registros utilizados
        // por ejemplo si solo se esta usando r0, entonces reg_num = 1

    } else if (strcmp(v,"SUB") == 0) {
        reg_num--; // se vuelve al ultimo registro utilizado
        if(reg_num < 1){
            printf("Error: Debe haber al menos 2 valores apilados\n");
            exit(0);
        }
        char reg1[4];
        char reg2[4];
        sprintf(reg1, "r%d", reg_num);
        sprintf(reg2, "r%d", reg_num-1);
        printf("\tldmfd r13!, {%s}\n",reg1);
        printf("\tldmfd r13!, {%s}\n",reg2);
        printf("\tsub %s, %s, %s\n", reg2, reg1, reg2);
        printf("\tstmfd r13!, {%s}\n",reg2);
        // al final reg_num queda 1 mas que los registros utilizados
        // por ejemplo si solo se esta usando r0, entonces reg_num = 1
    } else if (strcmp(v,"IF0") == 0) {
        reg_num--; // se vuelve al ultimo registro utilizado
        if(reg_num < 0){
            printf("Error: Debe haber al menos 1 valor en la pila\n");
            exit(0);
        }
        char reg[4];
        sprintf(reg, "r%d", reg_num); // registro a utilizar (r{reg_num})
        printf("\tldmfd r13!, {%s}\n",reg); // desapila elemento
        printf("\tcmp %s, #0\n", reg); // compara registro con 0
        printf("\tBEQ .L%d\n", label); // salta a .L{label} cuando r{reg_num} == 0
        sexp_t *lista1 = exp->list->next->next->list;
        while(lista1 != NULL){ 		//
            eval(lista1, out);      // ejecuta lista de instrucciones cuando r{reg_num} != 0
            lista1 = lista1->next;  //
        }
        printf("\tB .L%d\n", label+1); // salta a final del IF despues de ejecutar las instrucciones
        printf(".L%d:\n", label); // crea etiqueta .L{label}
        reg_num--; // se vuelve al ultimo registro utilizado
        label++;
        sexp_t *lista2 = exp->list->next->list;
        while(lista2 != NULL){		//
            eval(lista2, out);      // ejecuta lista de instrucciones cuando r{reg_num} == 0
            lista2 = lista2->next;	//
        }
        printf(".L%d:\n", label);	// crea etiqueta .L{label+1}
        label++;
    } else if (strcmp(v,"FUN") == 0) {
        reg_num--; // se vuelve al ultimo registro utilizado
        if(reg_num < 0){
            printf("Error: Debe haber al menos 1 valor en la pila\n"); // argumento de la funcion
            exit(0);
        }
        char reg[4];
        sprintf(reg, "r%d", reg_num); // registro a utilizar (r{reg_num})
        printf("\tldr %s, =.FUN%d\n",reg, fun_num); // desapila elemento
    } else { // if (strcmp(v,"RETURN") == 0) {
        printf("RETURN\n");
    }

}

void evalFun(sexp_t *exp, FILE *out) {
    char *v;

    /**
    * values understood here: 
    *   FUN
    */
    if (exp->ty == SEXP_LIST) {
        if (exp->list->ty == SEXP_VALUE)
            v = exp->list->val;
        else return;
    } else return;

    if (strcmp(v,"FUN") == 0) {
        reg_num++; // argumento
        printf(".FUN%d:\n", fun_num); // escribe etiqueta para funcion
        fun_num++;
        sexp_t *lista2 = exp->list->next->list;
        while(lista2 != NULL){     //
            v = lista2->list->val;
            if(strcmp(v,"FUN") == 0 or strcmp(v,"RETURN") == 0)
                evalFun(lista2, out);
            else
                eval(lista2, out);    // ejecuta lista de instrucciones cuando r{reg_num} == 0
            //printf("%s\n", lista2->list->val);
            lista2 = lista2->next;  //
        }
    } else { //if (strcmp(v,"RETURN") == 0) {
        printf("\tjump to main\n");
    }
}

/****
 * main
 ****/
int main(int argc, char **argv) {
    char linebuf[BUFSIZ];
    FILE *fp, *out;
    char *status;
    sexp_t *sx;

    if (argc == 2)
        fp = fopen(argv[1],"r+");
    else
        fp = fopen("test2.in","r+");
    out = fopen("arm.s","w+");

    printf(".data\n");
    printf("string: \n");
    printf("%s","\t.ascii \"%d\\n\"\n");
    printf(".text\n");
    printf(".global main\n");
    
    while (1) {
        status = fgets(linebuf,BUFSIZ,fp);

        if (feof(fp) != 0) break;

        /* if not EOF and status was NULL, something bad happened. */
        if (status != linebuf) {
            printf("Error encountered on fgets.\n");
            exit(EXIT_FAILURE);
        }

        sx = parse_sexp(linebuf,BUFSIZ);
        print_sexp(linebuf,BUFSIZ,sx);

        evalFun(sx, out);

        destroy_sexp(sx);

        fflush(stderr);
    }

    if (argc == 2)
        fp = fopen(argv[1],"r+");
    else
        fp = fopen("test2.in","r+");

    printf("main: \n");
    printf("\tstmfd sp!, {fp,lr}\n");
    reg_num = 0;
    fun_num = 0;
    while (1) {
        status = fgets(linebuf,BUFSIZ,fp);

        if (feof(fp) != 0) break;

        /* if not EOF and status was NULL, something bad happened. */
        if (status != linebuf) {
            printf("Error encountered on fgets.\n");
            exit(EXIT_FAILURE);
        }

        sx = parse_sexp(linebuf,BUFSIZ);
        print_sexp(linebuf,BUFSIZ,sx);

        eval(sx, out);

        destroy_sexp(sx);

        fflush(stderr);
    }

    //final del programa
    printf("\tldmfd r13!, {r1}\n");     // desapilar ultimo elemento
    printf("\tldr r0, =string\n");      //
    printf("\tbl printf\n");            // mostrar elemento en pantalla
    printf("\tldmfd sp!, {fp, pc}\n");  //

    sexp_cleanup();

    fclose(fp);

    exit(EXIT_SUCCESS);
}
