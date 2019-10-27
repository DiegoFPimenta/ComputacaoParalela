#ifndef SDS
    #define SDS
    #include "sds/sds.h"
    #include "sds/sds.c"
#endif
#include <string.h>
#ifndef DEFINES
    #define DEFINES
    #define TRUE 1
    #define FALSE 0
    #define MIN(a, b) ((a) < (b)) ? (a) : (b)
    #define get_m_value(vector, size, a, b) vector[a*size + b]
#endif


void printMatriz(float *matrix, int tam, int rank){
    sds string = sdsempty();
    sds temp = sdsempty();
    
    sprintf(temp,"Meu_rank %d\n",rank);
    string = sdscat(string, temp);

    for(int i = 0; i < tam; i++){
        for(int j = 0; j < tam; j++){
            sprintf(temp,"%5.2f ",get_m_value(matrix,tam,i,j));
            string = sdscat(string, temp);
        }
        string = sdscat(string, "\n");
    }
    string = sdscat(string, "\n");
    printf("%s", string);
}

void printMatrizFOX(sds title, float *matrix, int tam, int rank, int iteracao){
    sds string = sdsempty();
    sds temp = sdsempty();
    
    sprintf(temp,"iteracao %d ", iteracao);
    string = sdscat(string, temp);
    sprintf(temp,"Meu_rank %d ",rank);
    string = sdscat(string, temp);
    string = sdscat(string, title);
    
    for(int i = 0; i < tam; i++){
        for(int j = 0; j < tam; j++){
            sprintf(temp,"%5.2f ",get_m_value(matrix,tam,i,j));
            string = sdscat(string, temp);
        }
        string = sdscat(string, "\n");
    }
    string = sdscat(string, "\n");
    printf("%s", string);
}

float *leMatrizDoArquivo(char *arquivo, int *N){

    sds linha = sdsempty();
    sds *valores;
    FILE *arq = fopen(arquivo,"r");
    
    if (arq == NULL)
        return NULL;
    
    int count;
    int n;
    float *matrix;

    fscanf(arq, "%d\n", &n);                        // Lê o tamanho n da matriz
    matrix = (float*) malloc(n*n*sizeof(float));

    for (int i = 0; i < n; i++){                    
        fgets(linha,10000, arq);
        valores = sdssplitargs(linha, &count);        // Split no espaço

        for (int j = 0; j < count; j++){                            // Atribui os valores à matriz
            get_m_value(matrix,n,i,j) = atof(valores[j]);
        }
    }

    fclose(arq);

    *N = n;
    return matrix;
}
 