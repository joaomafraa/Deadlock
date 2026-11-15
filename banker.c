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
    FILE *arq=fopen("customer.txt", "r");
    if(arq == NULL){
        fprintf(stderr,"Erro ao abrir customer.txt\n");
        return 0;
    }
    char linha[256];
    while(fgets(linha,sizeof(linha),arq)!=NULL){
        linha[strcspn(linha, "\r\n")] = '\0';
        char* parte=strtok(linha,",");
        int coluna=0;
        while (parte!=NULL)
        {
            printf("coluna %d %s ",coluna,parte);
            coluna++;
            parte=strtok(NULL,",");
        }
        printf("\n");
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
            fprintf(stderr,"entrada invalida: %s\n", argv[i]);
            return 1;
        }
        printf("%d\n", vetor[i-1]);
    }
    if(ler_clientes()==0){
        free(vetor);
        return 1;
    }
    free(vetor);
    return 0;

}