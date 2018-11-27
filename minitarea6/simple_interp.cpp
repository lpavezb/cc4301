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
int reg_num = 0;
void eval(sexp_t *exp, FILE *out) {
    char *v;
    /**
     * values understood here: 
     *   INT_CONST
     *   ADD
     *   SUB
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
        fprintf(out, "\tmov %s, #%s\n", reg, s); // comando mov
        fprintf(out, "\tstmfd r13!, {%s}\n", reg); //comando stmdf
        reg_num++; // se suma 1 a los registros utilizados

    } else if (strcmp(v,"ADD") == 0) {
        reg_num--; // se vuelve al ultimo registro utilizado
        if(reg_num < 1){
            fprintf(out, "Error: Debe haber al menos 2 valores apilados\n");
            exit(0);
        }
        char reg1[4];
        char reg2[4];
        sprintf(reg1, "r%d", reg_num);
        sprintf(reg2, "r%d", reg_num-1);
        fprintf(out, "\tldmfd r13!, {%s}\n",reg1);
        fprintf(out, "\tldmfd r13!, {%s}\n",reg2);
        fprintf(out, "\tadd %s, %s, %s\n", reg2, reg1, reg2);
        fprintf(out, "\tstmfd r13!, {%s}\n",reg2);
        // al final reg_num queda 1 mas que los registros utilizados
        // por ejemplo si solo se esta usando r0, entonces reg_num = 1

    } else { //if (strcmp(v,"SUB") == 0) {
        reg_num--; // se vuelve al ultimo registro utilizado
        if(reg_num < 1){
            printf("Error: Debe haber al menos 2 valores apilados\n");
            exit(0);
        }
        char reg1[4];
        char reg2[4];
        sprintf(reg1, "r%d", reg_num);
        sprintf(reg2, "r%d", reg_num-1);
        fprintf(out, "\tldmfd r13!, {%s}\n",reg1);
        fprintf(out, "\tldmfd r13!, {%s}\n",reg2);
        fprintf(out, "\tsub %s, %s, %s\n", reg2, reg1, reg2);
        fprintf(out, "\tstmfd r13!, {%s}\n",reg2);
        // al final reg_num queda 1 mas que los registros utilizados
        // por ejemplo si solo se esta usando r0, entonces reg_num = 1
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
        fp = fopen("test1.in","r+");
    out = fopen("arm.s","w+");

    fprintf(out, ".data\n");
    fprintf(out, "string: \n");
    fprintf(out, "%s","\t.ascii \"%d\\n\"\n");
    fprintf(out, ".text\n");
    fprintf(out, ".global main\n");
    fprintf(out, "main: \n");
    fprintf(out, "\tstmfd sp!, {fp,lr}\n");

    while (1) {
        status = fgets(linebuf,BUFSIZ,fp);


        /* if not EOF and status was NULL, something bad happened. */
        if (status != linebuf) {
            fprintf(out, "Error encountered on fgets.\n");
            exit(EXIT_FAILURE);
        }

        sx = parse_sexp(linebuf,BUFSIZ);
        print_sexp(linebuf,BUFSIZ,sx);

        eval(sx, out);

        destroy_sexp(sx);

        fflush(stderr);
        if (feof(fp) != 0) break;
    }
    //Final del programa (cierre y mostrar en pantalla)
    fprintf(out, "\tldmfd r13!, {r1}\n");
    fprintf(out, "\tldr r0, =string\n");
    fprintf(out, "\tbl printf\n");
    fprintf(out, "\tldmfd sp!, {fp, pc}\n");
  
    sexp_cleanup();

    fclose(fp);

    exit(EXIT_SUCCESS);
}
