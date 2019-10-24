#include <string.h>
#include "matrix.h"
#include "sds/sds.h"
#include "sds/sds.c"

#define get_m_value(vector, size, a, b) vector[a*size + b]


void printMatriz(float *matrix, int tam){

    printf("Dimensao: %d\n",tam);

    for(int i = 0; i < tam; i++){
        for(int j = 0; j < tam; j++){
            printf("%5.2f ",get_m_value(matrix,tam,i,j));
        }
        printf("\n");
    }


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
