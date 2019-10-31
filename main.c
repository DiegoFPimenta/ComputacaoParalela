
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

#ifndef GRID
    #define GRID
    typedef struct grid_t {
        MPI_Comm comm;
        MPI_Comm row_comm;
        MPI_Comm col_comm;
        int coordenadas[2];
        int my_rank;
        int my_row;
        int my_col;
        int size;
    }grid_t;
#endif

#ifndef FOX
    #define FOX
    #include "fox.c"
#endif

#ifndef MATRIX
    #define MATRIX
#include "matrix.c"
#endif

void finalizaProgramaComErro (char *message) {
    printf("%s\n",message);
    int satisfaz = 0;
    MPI_Bcast (&satisfaz, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Finalize();
    exit(0);
}

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

grid_t* setupgrid(int q){
    grid_t *grid = (grid_t*) malloc(sizeof(grid_t));
    int dimensoes[2] = { q, q };
    int circular[2] = { TRUE, TRUE };
    
    int reordenar = TRUE;    
    
    MPI_Cart_create(MPI_COMM_WORLD, 2, dimensoes, circular, reordenar, &grid->comm);
    
    int  varying_coords[2] = {0, 1};
    MPI_Cart_sub(grid->comm, varying_coords, &(grid->row_comm));
    
    varying_coords[0] = 1; varying_coords[1] = 0;
    MPI_Cart_sub(grid->comm, varying_coords, &grid->col_comm);
    
    int coordenadas[2];
    MPI_Comm_rank(grid->comm, &(grid->my_rank));
    MPI_Cart_coords(grid->comm, grid->my_rank, 2, coordenadas);
    grid->my_row = coordenadas[0];
    grid->my_col = coordenadas[1];
    grid->size = q;
    return grid;
}

int main(int argc, char *argv[]){

    int  my_rank, n_procs, q;
    int  N = 0;
    float *matrix;

    int satisfaz = 1;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

    q = (int) sqrt(n_procs);
    // Cria a topologia cartesiana
    
    float *dist;
    //le a matriz do arquivo
    if( my_rank == 0 ){
        if (argc < 2){
            finalizaProgramaComErro("Usage ./fox matrix-file.txt");
        }

        matrix = leMatrizDoArquivo(argv[1], &N);
        if (matrix == NULL){
            finalizaProgramaComErro("Could not load Matrix file!");
        }
        //printMatriz(matrix, N, my_rank);

        // Verifica se os parâmetros satisfazem a condição inicial do algoritmo de Fox
        satisfaz = isPerfectSquare(n_procs) && N % q == 0;
        if (!satisfaz){
            finalizaProgramaComErro("A matriz não satisfaz as condições para o algoritmo de Fox. Encerrando...");
        }
        MPI_Bcast (&satisfaz, 1, MPI_INT, 0, MPI_COMM_WORLD);        // Broadcast sucesso
        MPI_Bcast (&N, 1, MPI_INT, 0, MPI_COMM_WORLD);        // Broadcast tamanho da matriz
        //-----------------------------------------------------------------

        dist = inicializa_dist (matrix, N);
    }
    else{
        MPI_Bcast (&satisfaz, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (!satisfaz){
            MPI_Finalize();
            return 0;
        }
        MPI_Bcast (&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }


    MPI_Datatype blocktype;
    MPI_Datatype blocktype2;
    int block_dimm = N/q;  // numero de linhas em cada bloco
    int blocksize = block_dimm*block_dimm;

    
    // ------------ Divisão da matriz dist em blocos de tamanho block_dimmxblock_dimm
    MPI_Type_vector(block_dimm, block_dimm, N, MPI_FLOAT, &blocktype2);
    MPI_Type_create_resized( blocktype2, 0, sizeof(float), &blocktype);
    MPI_Type_commit(&blocktype);                  

    int disps[n_procs];
    int counts[n_procs];
    for (int i=0; i<q; i++) {
        for (int j=0; j<q; j++) {
            get_m_value(disps, q, i, j) = i*N*block_dimm+j*block_dimm;
            get_m_value(counts, q, i, j) = 1;
        }
    }    
    
    float *localdist = (float*) malloc(blocksize*sizeof(float));
    float *localresult = (float*) calloc(blocksize,sizeof(float));
    
    grid_t *grid = setupgrid(q);

    MPI_Scatterv(dist, counts, disps, blocktype, localdist, blocksize, MPI_FLOAT, 0, MPI_COMM_WORLD);
    

    for (int i = 0; i < block_dimm; i++)
        for (int j = 0; j < block_dimm; j++)
                get_m_value(localresult, block_dimm, i, j) = INFINITY;
    int g;
    

    
    double t1, t2;
    char *arquivo_saida = (char*) malloc (64);
    
    if (my_rank == 0) system("mkdir saida");
    FILE *fout;
    int aux;
    for (aux = 0; aux < strlen(argv[1]); aux++) 
        if (argv[1][aux] =='/') break;
    
    sprintf(arquivo_saida, "saida/n%d:%s", n_procs, argv[1]+aux+1);
    fout = fopen(arquivo_saida, "w");
    if (my_rank == 0) t1 = MPI_Wtime();
    
    for ( g = 1; g < N; g *=2){
        fox(grid, localdist, block_dimm, localresult);
        memcpy(localdist, localresult, blocksize*sizeof(float));
    }
    
    if (my_rank == 0) {
        t2 = MPI_Wtime();
        fprintf(fout, "tempo: %lf\n", t2 -t1);
    }
    
    float *resultado = junta_resultado(grid, localresult, block_dimm, N);
    if (my_rank == 0){
        printMatrizArquivo(resultado, N, fout);
        fclose(fout);
    }

    MPI_Finalize();
    return 0;
}

