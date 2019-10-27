#ifndef STD
    #define STD
    #include <stdio.h>
    #include <stdlib.h>
    #include <mpi.h>
    #include <math.h>
    #include <limits.h>
#endif

#ifndef DEFINES
    #define DEFINES
    #define TRUE 1
    #define FALSE 0
    #define MIN(a, b) ((a) < (b)) ? (a) : (b)
    #define get_m_value(vector, size, a, b) vector[a*size + b]
#endif

#ifndef MATRIX
    #define MATRIX
#include "matrix.c"
#endif


#ifndef GRID
    #define GRID
    typedef struct grid_t {
        MPI_Comm comm;
        MPI_Comm row_comm;
        MPI_Comm col_comm;
        int coordenadas[2];
        int my_rank;
        int size;
    }grid_t;
#endif

float* inicializa_dist (float *matrix, int n){
    float *dist = (float*) malloc(n*n*sizeof(float));

    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            if (i == j){
                get_m_value(dist, n, i, j) = 0;
            } else if (get_m_value(matrix, n, i, j) == 0) {
                get_m_value(dist, n, i, j) = INFINITY;
            } else {
                get_m_value(dist, n, i, j) = get_m_value(matrix, n, i, j);
            }
        }
            //printf("\n");
    }
    return dist;
}

float* print_resultado(grid_t *grid, float *localresult, int localsize, int globalsize){   
    float *matrizona = (float *) malloc (globalsize*globalsize*sizeof(float));
    int        source;
    float*     temp;
    MPI_Status status;
    temp = (float*) malloc(localsize*sizeof(float));

    int linha, col_global;
    for (int i = 0; i < globalsize; i++){
        linha = (int)i/localsize;
        if (grid->my_rank == 0){
            for (int j = 0; j < grid->size; j++){
                source = linha*grid->size + j;
                col_global = (source%grid->size)*localsize;
                if (source != 0) {
                    MPI_Recv(temp, localsize, MPI_FLOAT, source, 0, grid->comm, &status);
                    memcpy(&(get_m_value(matrizona, globalsize, i, col_global)), temp, localsize*sizeof(float));
                }else{

                    memcpy(&(get_m_value(matrizona, globalsize, i, col_global)), localresult+(i%localsize*localsize), localsize*sizeof(float));
                }
            }
        }else{
            if (grid->my_row == linha){
                MPI_Send(&get_m_value(localresult, localsize, i%localsize, 0), localsize, MPI_FLOAT, 0, 0, grid->comm);
                
            }
        }
    }
    return matrizona;
}

void multiplica(float *A, float *B, float *res, int n){
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                get_m_value(res, n, i, j) = MIN(get_m_value(res, n, i, j), get_m_value(A, n, i, k) + get_m_value(B, n, k, j));
}

void fox(grid_t *grid, float *localdist, int N, float *localresult){
    //printMatriz(localdist, N, grid->my_rank);
    int blocksize = N*N;

    float *local_A;
    float *local_B;
    int bcast_root;
    int source;
    int dest;
    MPI_Status status;

    local_A = localdist;
    local_B = (float*) malloc(blocksize*sizeof(float));
    memcpy(local_B, localdist, blocksize*sizeof(float));

    /* Calculate addresses for circular shift of B */  
    source = (grid->my_row + 1) % grid->size;
    dest = (grid->my_row + grid->size - 1) % grid->size;

    /* Set aside storage for the broadcast block of A */
    float *temp_A = (float*) malloc(blocksize*sizeof(float));

    for (int stage = 0; stage < grid->size; stage++) {
        bcast_root = (grid->my_row + stage) % grid->size;

        if (bcast_root == grid->my_col) {
            MPI_Bcast(local_A, blocksize, MPI_FLOAT, bcast_root, grid->row_comm);
            multiplica(local_A, local_B, localresult, N);
        } else {
            MPI_Bcast(temp_A, blocksize, MPI_FLOAT, bcast_root, grid->row_comm);
            multiplica(temp_A, local_B, localresult, N);
        }
        MPI_Sendrecv_replace(local_B, blocksize, MPI_FLOAT, dest, 0, source, 0, grid->col_comm, &status);
    } 

}
