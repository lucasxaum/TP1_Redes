#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_functions.c"

int main(int argc, char *argv[]) {	
	if (argc == 3) {
		int reservedPort = createSocket(atoi(argv[1]));
		printf("Server ON!\n");
		switch (atoi(argv[2])) {
			case 1:
				printf("Modo: Iterativo\n");
				iterative(reservedPort);
				break;
			case 2:
				printf("Modo: Threads\n");
				threads(reservedPort);
				break;
			case 3:
				printf("Modo: Produtor-Consumidor\n");
				producerConsumer(reservedPort);
				break;
			case 4:
				printf("Modo: Concorrente\n");
				competitor(reservedPort);
				break;
			default:
				printf("\nERRO: Parâmetros não informados corretamente!\n");
		}
		close(reservedPort);
	} else {
		printf("\nERRO: Parâmetros não informados corretamente!\n");
	}	
	return 0;
}