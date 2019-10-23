#include "matrix.h"
#include "matrix.c"
#include <mpi.h>
#include <math.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0

int isPerfectSquare(int number)
{
    int iVar;
    float fVar;
 
    fVar=sqrt((double)number);
    iVar=fVar;
 
    if(iVar==fVar)
        return 1;
    else
        return 0;
}

float* inicializa_dist (float *matrix, int n){
    float *dist = (float*) malloc(n*n*sizeof(float));

    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            if (get_m_value(matrix, n, i, j) != 0){
                get_m_value(dist, n, i, j) = get_m_value(matrix, n, i, j);   // vertices já existentes
            } else if (i != j) {
                get_m_value(dist, n, i, j) = INFINITY;                       // Distancia entre um vertice e outro nao conectados
            } else { 
                get_m_value(dist, n, i, j) = 0;                              //Distancia de um vertice com ele mesmo
            }
            //printf("%5.2f ", get_m_value(dist, n, i, j));
        }
            //printf("\n");
    }
    return dist;
}

int main(int argc, char *argv[]){

    int  i, my_rank, n_procs;
    int  dimension_matrix = 0;
    MPI_Status status;

    int satisfaz = 1;

    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

    //le a matriz do arquivo

    if( my_rank == 0 ){
        float *matrix;
        matrix = leMatrizDoArquivo("matrix.txt", &dimension_matrix);
        
        // Verifica se os parâmetros satisfazem a condição inicial do algoritmo de Fox

        satisfaz = isPerfectSquare(n_procs) && dimension_matrix % (int) sqrt(n_procs) == 0;
        MPI_Bcast (&satisfaz, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        if (!satisfaz){
            printf("A matriz não satisfaz as condições para o algoritmo de Fox. Encerrando...\n");
            MPI_Finalize();
            exit(0);
        }
        

        float *dist;
        
        dist = inicializa_dist (matrix, dimension_matrix);
        //faz o calculo de quantas partes vao ter
        //distribui pro pares
        printMatriz(matrix,dimension_matrix);
        printf("\n\n===============DIST================\n");
        printMatriz(dist,dimension_matrix);
    }
    else{
        MPI_Bcast (&satisfaz, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (satisfaz){
            
        }
    }

    MPI_Finalize();
}
