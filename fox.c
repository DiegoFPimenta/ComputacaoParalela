#include "matrix.h"
#include "matrix.c"
#include <mpi.h>
#include <math.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0

#define MIN(a, b) ((a) < (b)) ? (a) : (b)


void finalizaProgramaComErro (char *message) {
    printf("%s\n",message);
    int satisfaz = 0;
    MPI_Bcast (&satisfaz, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Finalize();
    exit(0);
}

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

    int  my_rank, n_procs;
    int  N = 0;
    MPI_Status status;

    int satisfaz = 1;

    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

    //le a matriz do arquivo

    if( my_rank == 0 ){
        if (argc < 2){
            finalizaProgramaComErro("Usage ./fox matrix-file.txt");
        }

        float *matrix;
        matrix = leMatrizDoArquivo(argv[1], &N);
        
        if (matrix == NULL){
            finalizaProgramaComErro("Could not load Matrix file!");
        }

        // Verifica se os parâmetros satisfazem a condição inicial do algoritmo de Fox

        satisfaz = isPerfectSquare(n_procs) && N % (int) sqrt(n_procs) == 0;
        
        if (!satisfaz){
            finalizaProgramaComErro("A matriz não satisfaz as condições para o algoritmo de Fox. Encerrando...");
        }
        MPI_Bcast (&satisfaz, 1, MPI_INT, 0, MPI_COMM_WORLD);        // Broadcast sucesso

        //-----------------------------------------------------------------

        float *dist;
        
        dist = inicializa_dist (matrix, N);
             

        //faz o calculo de quantas partes vao ter
        //distribui pro pares
        printMatriz(matrix,N);
        printf("\n\n===============DIST================\n");
        printMatriz(dist,N);
    }
    else{
        MPI_Bcast (&satisfaz, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (satisfaz){
            
        }
    }

    MPI_Finalize();
}
