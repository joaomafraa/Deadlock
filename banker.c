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
        return 0;
    }

    *matriz = maximo;
    *qnt_clientes_saida = qnt_Clientes;

    fclose(arq);
    return 1;
}
int estado_seguro(int qnt_clientes, int qnt_recursos,int *disponivel, int *alocacao, int *necessidade) {
    int *trabalho = malloc((size_t)qnt_recursos * sizeof(int));
    int *terminou = calloc((size_t)qnt_clientes, sizeof(int));

    if(trabalho == NULL || terminou == NULL){
        fprintf(stderr, "Erro ao verificar estado seguro\n");
        free(trabalho);
        free(terminou);
        return -1;
    }

    for(int i=0;i<=qnt_recursos;i++){
        trabalho[i] = disponivel[i];
    }

    int concluidos=0;
    int avancou;

    do{
        avancou = 0;
        for(int cliente=0;cliente<qnt_clientes;cliente++){
            if (terminou[cliente]){
                continue;
            }

            int pode_terminar=1;

            for(int recurso=0;recurso<qnt_recursos;recurso++){
                int posicao=cliente * qnt_recursos + recurso;

                if(necessidade[posicao]>trabalho[recurso]){
                    pode_terminar=0;
                    break;
                }
            }

            if(pode_terminar){
                for (int recurso = 0; recurso < qnt_recursos; recurso++) {
                int posicao = cliente * qnt_recursos + recurso;

                trabalho[recurso] += alocacao[posicao];
            }
                terminou[cliente] = 1;
                concluidos++;
                avancou = 1;
            }
        }
    }while(avancou && concluidos<qnt_clientes);
    int seguro = (concluidos == qnt_clientes);
    free(trabalho);
    free(terminou);
    return seguro;
}
int solicitar_recursos(int cliente, int qnt_clientes,int qnt_recursos,int *pedido, int *necessidade,int* disponivel,int*alocacao){
    for(int recurso=0;recurso<qnt_recursos;recurso++){
        int posicao = cliente * qnt_recursos + recurso;
        if(pedido[recurso]>necessidade[posicao]){
            return 0;
        } 
        if(pedido[recurso]>disponivel[recurso]){
            return 0;
        }
    }
    for (int recurso = 0; recurso < qnt_recursos; recurso++) {
        int posicao = cliente * qnt_recursos + recurso;

        disponivel[recurso] -= pedido[recurso];
        alocacao[posicao] += pedido[recurso];
        necessidade[posicao] -= pedido[recurso];
    }
    int resultado = estado_seguro(qnt_clientes, qnt_recursos,disponivel, alocacao, necessidade);
    if (resultado != 1){
    for (int recurso = 0; recurso < qnt_recursos; recurso++) {
        int posicao = cliente * qnt_recursos + recurso;

        disponivel[recurso] += pedido[recurso];
        alocacao[posicao] -= pedido[recurso];
        necessidade[posicao] += pedido[recurso];
        }
    }
    return resultado;
    
}
int ler_comandos(int qnt_clientes,int qnt_recursos,int *disponivel,int *alocacao,int *necessidade,FILE *resultado){
    FILE *arq = fopen("commands.txt", "r");
    if(arq == NULL){
        fprintf(stderr, "Erro ao abrir commands.txt\n");
        return 0;
    }
    char linha[256];
    while(fgets(linha, sizeof(linha), arq) != NULL){
        if(strchr(linha, '\n') == NULL && fgetc(arq) != EOF){
            fprintf(stderr, "Linha muito longa em commands.txt\n");
            fclose(arq);
            return 0;
        }
        char *comando = strtok(linha, " \t\r\n");
        if(comando == NULL){
            fprintf(stderr, "Linha vazia em commands.txt\n");
            fclose(arq);
            return 0;
        }
        int cliente;
        if(strcmp(comando, "RQ") == 0 || strcmp(comando, "RL") == 0){
            char *parte_cliente = strtok(NULL, " \t\r\n");
            if(parte_cliente == NULL || validar_arg(parte_cliente, &cliente) == 0 || cliente >= qnt_clientes){
                fprintf(stderr, "Cliente invalido em commands.txt\n");
                fclose(arq);
                return 0;
            }
            int *valores = malloc((size_t)qnt_recursos * sizeof(int));
            if(valores == NULL){
                fprintf(stderr, "Erro ao alocar valores do comando\n");
                fclose(arq);
                return 0;
            }
            for(int i=0;i<qnt_recursos;i++){
                char *parte = strtok(NULL, " \t\r\n");
                if(parte == NULL || validar_arg(parte, &valores[i]) == 0){
                    fprintf(stderr, "Valor de recurso invalido em commands.txt\n");
                    free(valores);
                    fclose(arq);
                    return 0;
                }
            }
            if(strtok(NULL, " \t\r\n") != NULL){
                fprintf(stderr, "Valores a mais em commands.txt\n");
                free(valores);
                fclose(arq);
                return 0;
            }

            int situacao;

            if(strcmp(comando, "RQ") == 0){
                situacao = solicitar_recursos(cliente, qnt_clientes, qnt_recursos,valores, necessidade, disponivel, alocacao);
                if(situacao == -1){
                    free(valores);
                    fclose(arq);
                    return 0;
                }
                if(situacao == 1){
                    fprintf(resultado, "Allocate to customer %d the resources", cliente);
                    for(int i=0;i<qnt_recursos;i++){
                        fprintf(resultado, " %d", valores[i]);
                    }
                    fprintf(resultado, "\n");
                } else{
                    fprintf(resultado, "Request denied for customer %d\n", cliente);
                }
            } else{
                situacao = liberar_recursos(cliente, qnt_recursos, valores,disponivel, alocacao, necessidade);
                if(situacao == 1){
                    fprintf(resultado, "Release from customer %d the resources", cliente);
                    for(int i=0;i<qnt_recursos;i++){
                        fprintf(resultado, " %d", valores[i]);
                    }
                    fprintf(resultado, "\n");
                } else{
                    fprintf(resultado, "Release denied for customer %d\n", cliente);
                }
            }
            free(valores);
        } else if(strcmp(comando, "*") == 0){
            if(strtok(NULL, " \t\r\n") != NULL){
                fprintf(stderr, "Valores a mais apos * em commands.txt\n");
                fclose(arq);
                return 0;
            }

        } else{
            fprintf(stderr, "Comando invalido: %s\n", comando);
            fclose(arq);
            return 0;
        }
    }

    if(ferror(arq)){
        fprintf(stderr, "Erro ao ler commands.txt\n");
        fclose(arq);
        return 0;
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
    for(size_t i=0;i<total;i++){
        necessidade[i] = maximo[i];
    }
    FILE *resultado = fopen("result.txt", "w");
    if(resultado == NULL){
        fprintf(stderr, "Erro ao criar result.txt\n");
        free(necessidade);
        free(alocacao);
        free(maximo);
        free(vetor);
        return 1;
    }
    if(ler_comandos(qnt_clientes, qnt_recursos,vetor, alocacao, necessidade, resultado) == 0){
        fclose(resultado);
        free(necessidade);
        free(alocacao);
        free(maximo);
        free(vetor);
        return 1;
    }
    fclose(resultado);
    free(necessidade);
    free(alocacao);
    free(maximo);
    free(vetor);
    return 0;
}