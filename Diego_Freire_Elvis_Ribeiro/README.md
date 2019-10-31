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

A pasta também conta com alguns shell scripts:

geraMatriz.sh:
   Compila e executa o .c homônimo


batchexec.sh n_procs pasta_com_arquivos/
   Compila e executa o código MPI com n_procs e com os arquivos da pasta passada no argumento