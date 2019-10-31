#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	unsigned short i, j, n;
	int matrix_sizes[5] = {12, 1266, 1896, 2502, 642};
	FILE *arq;
	char *dir = (char*) malloc(50);
	char *name = (char*) malloc(50);
	
	system("mkdir entrada");
	for (int k = 0; k < 5; k++){
		sprintf(name, "entrada/m%s%d.txt", dir, matrix_sizes[k]);
		printf("gerando %s\n", name);
		arq = fopen(name, "w");
		fprintf(arq, "%d\n", matrix_sizes[k]);
	  	for(i = 0; i < matrix_sizes[k]; i++) {
    		for(j = 0; j < matrix_sizes[k]; j++) {
    			if (i != j)
    				fprintf(arq, "%d ", rand()%100);
    			else
    				fprintf(arq, "0 ");

    		}
    		fprintf(arq, "\n");
    	}
    	fclose(arq);
	}
	return 0;
}