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


void printMatrizArquivo(float *matrix, int tam, FILE *arq){
    sds string = sdsempty();
    sds temp = sdsempty();
    
    string = sdscat(string, temp);

    for(int i = 0; i < tam; i++){
        for(int j = 0; j < tam; j++){
            sprintf(temp,"%5.2f ",get_m_value(matrix,tam,i,j));
            string = sdscat(string, temp);
        }
        string = sdscat(string, "\n");
    }
    string = sdscat(string, "\n");
    fprintf(arq, "%s", string);
}

float *leMatrizDoArquivo(char *arquivo, int *N){

    FILE *arq = fopen(arquivo,"r");
    
    if (arq == NULL)
        return NULL;
    
    int n;
    float *matrix;

    fscanf(arq, "%d\n", &n);                        // Lê o tamanho n da matriz
    matrix = (float*) malloc(n*n*sizeof(float));

   
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if (!fscanf(arq, "%f",  &(get_m_value(matrix,n,i,j)))) 
                break;            
        }
    }
    fclose(arq);

    *N = n;
    return matrix;
}
 
