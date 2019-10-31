all: floyd geraMatriz
	
geraMatriz: geraMatriz.c
	gcc geraMatriz.c -o geraMatriz

floyd: main.c 
	mpicc main.c -o floyd -lm

clean: 
	rm floyd
	rm geraMatriz
	rm -rf entrada
	rm -rf saida
	