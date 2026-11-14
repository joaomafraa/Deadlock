#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>


int validar_arg(char *entrada,int *resultado){
    char*final;
    long valor;
    errno=0;

    valor=strtol(entrada,&final,10);
                                        //ULTRAPASSOU O LONG
    if(final==entrada || *final!='\0' || errno == ERANGE || valor<0 || valor >INT_MAX){
        return 0;
    }
    *resultado=(int)valor;
    return 1;
}
int main(int argc,char **argv){
    if (argc < 2){
        fprintf(stderr,"INFORME PELO MENOS UM TIPO DE RECURSO\n");
        return 1;
    }
    int resultado=0;
    for(int i=1;i<argc;i++){
        if(validar_arg(argv[i],resultado) == 0){
            fprintf(stderr,"entrada invalida: %s\n", argv[1]);
            return 1;
        }
    }
}