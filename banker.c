#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

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
int ler_clientes(){
    FILE *arq=fopen("cosumer.txt", "r");
    if(arq == NULL){
        fprintf(stderr,"Erro ao abrir custumer.txt\n");
        return 0;
    }
    char linha[256];
    while(fgets(linha,sizeof(linha),arq)!=NULL){
        
    }

    fclose(arq);
    return 1;
}
int main(int argc,char **argv){
    if (argc < 2){
        fprintf(stderr,"INFORME PELO MENOS UM TIPO DE RECURSO\n");
        return 1;
    }
    int *vetor=malloc(argc-1 *sizeof(int));
    if(vetor == NULL){
        fprintf(stderr,"erro na alocacao");
        return 1;
    }
    for(int i=1;i<argc;i++){
        if(validar_arg(argv[i],&vetor[i-1]) == 0){
            free(vetor);
            fprintf(stderr,"entrada invalida: %s\n", argv[1]);
            return 1;
        }
        printf("%d\n", vetor[i-1]);
    }
}