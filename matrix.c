#include "matrix.h"

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

float *leMatrizDoArquivo(char *arquivo, int *tam){

    FILE *arq;
    int k,tamf,i,j,x;
    char lixo;
    float *matrix;

    arq = fopen("matrix.txt","r");
    
    //le o tamanho da matriz
    fscanf(arq,"%d\n",tam);

    //aloca a matriz
    matrix = (float*) malloc((*tam)*(*tam)*sizeof(float*));
    

    //le o \n
    //fscanf(arq,"%c",&lixo);

    // o numero de espaços é igual ao numero de numeros - 1.(tam + tam -1)...Porem existe o \n e o \0 por isso o tamanho é tam + tam + 1
    tamf = *tam + (*tam + 1);
    char linha[tamf];
    
    x = 0;
    // le linha por linha do arquivo ate acabar e atribui na matriz alocada
    while(fgets(linha, sizeof(linha), arq) != NULL) {
        j = 0;
        for(i = 0; i <= tamf - 2; i++){ // tamf -2 pq tira o \0 e o \n.
            if( i%2 == 0 ){
                get_m_value(matrix,*tam,x,j) = atof(&linha[i]);
                j++;
            }
        }
        x++;
    }

    fclose(arq);


    return matrix;



}
