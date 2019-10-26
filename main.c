#include "matrix.h"
#include "matrix.c"
#include <mpi.h>
#include <math.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0

#define MIN(a, b) ((a) < (b)) ? (a) : (b)

void solucaoSimples (float *dist, int N){
    for (int k = 0; k < N; k++) {
        for (int i = 0; i < N; i++){
            for (int j = 0; j < N; j++){
                get_m_value(dist, N, i, j) = MIN(get_m_value(dist, N, i, j),
                                                    get_m_value(dist, N, i, k) + get_m_value(dist, N, k, j));
            }
        }
    }
}


void finalizaProgramaComErro (char *message) {
    printf("%s\n",message);
    int satisfaz = 0;
    MPI_Bcast (&satisfaz, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Finalize();
    exit(0);
}

int isPerfectSquare(int number) {
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

    int  my_rank, n_procs, q;
    int  N = 0;
    MPI_Status status;

    int satisfaz = 1;

    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

    q = (int) sqrt(n_procs);
    // Cria a topologia cartesiana
    MPI_Comm grid_comm, row_comm, col_comm;

    int dimensoes[2] = { q, q };
    int circular[2] = { TRUE, TRUE };

    int reordenar = TRUE;    
    
    MPI_Cart_create(MPI_COMM_WORLD, 2, dimensoes, circular, reordenar, &grid_comm);
    
    int  varying_coords[2] = {0, 1};
    MPI_Cart_sub(grid_comm, varying_coords, &row_comm);
    varying_coords[2] = {1, 0};
    MPI_Cart_sub(grid_comm, varying_coords, &col_comm);

    int coordenadas[2];
    int my_grid_rank;

    MPI_Comm_rank(grid_comm, &my_grid_rank);
    MPI_Cart_coords(grid_comm, my_grid_rank, 2, coordenadas);
    
    float *dist, *c;
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
        MPI_Bcast (&N, 1, MPI_INT, 0, MPI_COMM_WORLD);        // Broadcast tamanho da matriz

        // Verifica se os parâmetros satisfazem a condição inicial do algoritmo de Fox
        satisfaz = isPerfectSquare(n_procs) && N % q == 0;
        if (!satisfaz){
            finalizaProgramaComErro("A matriz não satisfaz as condições para o algoritmo de Fox. Encerrando...");
        }
        MPI_Bcast (&satisfaz, 1, MPI_INT, 0, MPI_COMM_WORLD);        // Broadcast sucesso

        //-----------------------------------------------------------------

        dist = inicializa_dist (matrix, N);
        
        //distribui pro pares

        //faz o calculo de quantas partes vao ter
        printMatriz(matrix,N);
    }
    else{
        MPI_Bcast (&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast (&satisfaz, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (!satisfaz){
            MPI_Finalize();
            return 0;
        }
    }

    MPI_Datatype blocktype;
    MPI_Datatype blocktype2;
    int BLOCKSIZE = N/q;  // numero de linhas em cada bloco

    float *matrizlocal = (float*) malloc(BLOCKSIZE*BLOCKSIZE*sizeof(float));
    
    // ------------ Divisão da matriz dist em blocos de tamanho BLOCKSIZExBLOCKSIZE
    MPI_Type_vector(BLOCKSIZE, BLOCKSIZE, N, MPI_FLOAT, &blocktype2);
    MPI_Type_create_resized( blocktype2, 0, sizeof(float), &blocktype);
    MPI_Type_commit(&blocktype);                  

    int disps[n_procs];
    int counts[n_procs];
    for (int i=0; i<q; i++) {
        for (int j=0; j<q; j++) {
            get_m_value(disps, q, i, j) = i*N*BLOCKSIZE+j*BLOCKSIZE;
            get_m_value(counts, q, i, j) = 1;
        }
    }    
    
    MPI_Scatterv(dist, counts, disps, blocktype, matrizlocal, BLOCKSIZE*BLOCKSIZE, MPI_FLOAT, 0, MPI_COMM_WORLD);
    

    for (int i=0; i<BLOCKSIZE; i++) {
        for (int j=0; j<BLOCKSIZE; j++) {
            printf("%3.0f ", get_m_value(matrizlocal, BLOCKSIZE, i, j));
        }
        printf("\n");
    }
    

    MPI_Finalize();
    return 0;
}
