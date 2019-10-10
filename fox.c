#include "matrix.h"
#include <mpi.h>

int main(int argc, char *argv[]){

    int i, my_rank, n_procs;
    int dimension_matrix = 0;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

    //le a matriz do arquivo
    int **matrix;
    matrix = leMatrizDoArquivo("matrix.txt",&dimension_matrix);

    if( my_rank == 0 ){
        //faz o calculo de quantas partes vao ter
        //distribui pro pares
    }

    printMatriz(matrix,dimension_matrix);
}
