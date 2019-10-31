# Implementação do Algoritmo de Floyd-Warshall usando o algoritmo de Fox

Para compilar o programa basta digitar 

```Shell
$ mpicc main.c -o main -lm
```
A execução é feita usando o mpirun e passando o nome do arquivo como argumento para o programa

```Shell
$ mpirun -n n_procs main file_name 
```
Os resultados são armazenados na pasta "saida/"
