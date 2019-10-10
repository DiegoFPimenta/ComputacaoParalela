#include "matrix.h"

void printMatriz(int **matrix, int tam){

    int k,i;

    printf("Dimensao: %d\n",tam);

    for(k = 0; k < tam; k++){
        for(i = 0; i < tam; i++){
            printf("%d ",matrix[k][i]);
        }
        printf("\n");
    }


}

int **leMatrizDoArquivo(char *arquivo, int *tam){

    FILE *arq;
    int k,tamf,i,j,x;
    char lixo;
    int **matrix;

    arq = fopen("matrix.txt","r");
    
    //le o tamanho da matriz
    fscanf(arq,"%d",tam);

    //aloca a matriz
    matrix = (int**) malloc(*tam*sizeof(int*));
    for(k = 0; k < *tam; k++){
        matrix[k] = (int*) malloc(*tam*sizeof(int));
    }

    //le o \n
    fscanf(arq,"%c",&lixo);

    // o numero de espaços é igual ao numero de numeros - 1.(tam + tam -1)...Porem existe o \n e o \0 por isso o tamanho é tam + tam + 1
    tamf = *tam + (*tam + 1);
    char linha[tamf];
    
    x = 0;
    // le linha por linha do arquivo ate acabar e atribui na matriz alocada
    while(fgets(linha, sizeof(linha), arq) != NULL) {
        j = 0;
        for(i = 0; i <= tamf - 2; i++){ // tamf -2 pq tira o \0 e o \n.
            if( i%2 == 0 ){
                matrix[x][j] = atoi(&linha[i]);
                j++;
            }
        }
        x++;
    }

    fclose(arq);


    return matrix;



}
