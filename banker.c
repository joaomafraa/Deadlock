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
int ler_clientes(int qnt_recursos, int **matriz, int *qnt_clientes_saida){
    FILE *arq=fopen("customer.txt", "r");
    if(arq == NULL){
        fprintf(stderr,"Erro ao abrir customer.txt\n");
        return 0;
    }
    char linha[256];
    int qnt_Clientes=0;
    while(fgets(linha,sizeof(linha),arq)!=NULL){
        if(strchr(linha, '\n') == NULL && fgetc(arq) != EOF){
            fprintf(stderr, "Linha muito longa em customer.txt\n");
            fclose(arq);
            return 0;
        }
        linha[strcspn(linha, "\r\n")] = '\0';
        size_t tamanho = strlen(linha);
        if(tamanho == 0 || linha[0] == ',' || linha[tamanho - 1] == ','|| strstr(linha, ",,")!= NULL){
            fprintf(stderr, "Formato invalido em customer.txt\n");
            fclose(arq);
            return 0;
        }
        char* parte=strtok(linha,",");
        int coluna=0;
        while (parte!=NULL)
        {   
            int valor;
            if(validar_arg(parte, &valor) == 0){
                fprintf(stderr, "Valor invalido em customer.txt: %s\n", parte);
                fclose(arq);
                return 0;
            }
            printf("coluna %d %d ",coluna,valor);
            coluna++;
            parte=strtok(NULL,",");
        }
        if(coluna!=qnt_recursos){
            fprintf(stderr,"quantidade de recursos errados em customer.txt \n");
            fclose(arq);
            return 0;
        }
        qnt_Clientes++;
        printf("\n");
    }
    printf("Quantidade de clientes: %d\n", qnt_Clientes);
    if(ferror(arq)){
        fprintf(stderr, "Erro ao ler customer.txt\n");
        fclose(arq);
        return 0;
    }
    rewind(arq);//volta ao inicio do aqrquivo 
    int *maximo = malloc((size_t)qnt_Clientes * qnt_recursos * sizeof(int));
    if(maximo == NULL){
        fprintf(stderr,"erro ao alocar maximo");
        fclose(arq);
        return 0;
    }
    int cliente = 0;

    while(fgets(linha, sizeof(linha), arq) != NULL){
        if(cliente >= qnt_Clientes){
            fprintf(stderr, "Quantidade de clientes mudou durante a leitura\n");
            free(maximo);
            fclose(arq);
            return 0;
        }

        linha[strcspn(linha, "\r\n")] = '\0';
        char *parte = strtok(linha, ",");
        int coluna = 0;
        while(parte!=NULL && coluna<qnt_recursos){
            int valor;
            if(validar_arg(parte, &valor) == 0){
                fprintf(stderr, "Erro ao reler customer.txt\n");
                free(maximo);
                fclose(arq);
                return 0;
            }

            maximo[cliente * qnt_recursos + coluna]=valor;

            coluna++;
            parte = strtok(NULL, ",");
        }

        if (parte != NULL || coluna != qnt_recursos) {
            fprintf(stderr, "Quantidade de recursos incorreta\n");
            free(maximo);
            fclose(arq);
            return 0;
        } 
        cliente++;
    }
    if(ferror(arq) || cliente != qnt_Clientes){
        fprintf(stderr, "Erro ao reler customer.txt\n");
        free(maximo);
        fclose(arq);
        
    }

    *matriz = maximo;
    *qnt_clientes_saida = qnt_Clientes;

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
    int *maximo = NULL;
    int qnt_clientes = 0;
    if(ler_clientes(argc - 1, &maximo, &qnt_clientes) == 0){
    free(vetor);
    return 1;
    }
    int qnt_recursos = argc - 1;
    size_t total = (size_t)qnt_clientes * qnt_recursos;

    int *alocacao = calloc(total, sizeof(int));
    int *necessidade = malloc(total * sizeof(int));

    if (alocacao == NULL || necessidade == NULL) {
        fprintf(stderr, "Erro ao alocar as matrizes\n");
        free(alocacao);
        free(necessidade);
        free(maximo);
        free(vetor);
        return 1;
    }

    for (size_t i = 0; i < total; i++) {
        necessidade[i] = maximo[i];
    }
        free(maximo);
        free(vetor);
        return 0;

}