#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <semaphore.h>
#include "server_functions.h"

int createSocket(int port) {
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0) {
		printf("Erro ao abrir o socket");
		exit(1);
	}
	struct sockaddr_in server;
	//monta o endereço
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(IP);
	memset(server.sin_zero, 0, 8);
	//associa o endereço ao socket
	if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) < 0){
		printf("Erro ao associar endereço ao socket\n");
		exit(1);
	}
	listen(server_socket, CONSUMER_TAM);
	return server_socket;
}

char *verifyExtension(char *file) {
	char *contentType = strrchr(file, '.');
	if (contentType == ".png") {
		return "Content-Type: image/png\r\n\r\n";
	}
	return "Content-Type: text/html\r\n\r\n";
}

void responseOK(int client_socket, char *file) {
	int fragment, fd = open(file, O_RDONLY);
	char *header = (char *) malloc(MAX_LEN*sizeof(char));
	void *body = malloc(MAX_LEN*sizeof(char));
	char *contentType = verifyExtension(file);
	strcpy(header, "HTTP/1.0 200 OK\r\nConnection: close\r\n");			
	strcat(header, contentType);
	send(client_socket, header, strlen(header), 0);
	while ((fragment = read(fd, body, MAX_LEN)) > 0) {
		write (client_socket, body, fragment);
	}
    close(fd);
    free(header);
    free(body);
}

void response(int type, int client_socket, char *file) {
	char *msg;
	switch (type) {
		case 400:
			printf("ERRO 400: Bad Request\n");
			msg = ("HTTP/1.0 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<html><body><h1>ERRO 400: Bad Request</h1></body></html>\r\n");
			break;
		case 404:
			printf("ERRO 404: Not Found\n");
			msg = ("HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>ERRO 404: Not Found</h1</body></html>\r\n");
			break;
		default:
			printf("200 OK\n");
			responseOK(client_socket, file);
			return;
	}
	send(client_socket, msg, strlen(msg), 0);
}

int fileExists(const char *filename){
	FILE *file;
	if (file = fopen(filename, "r")) {
		fclose(file);
		return 1;
	}
	return 0;
}

int acceptConnection(int reservedPort, int size) {
	int client_socket = accept(reservedPort, (struct sockaddr*)&client, (socklen_t *)&size);
	if (client_socket < 0) {
		printf("Erro ao aceitar conexao!\n");
		exit(1);
	}
	printf("\nCliente: %s - Porta: %d\n", (char *)inet_ntoa(client.sin_addr), ntohs(client.sin_port));
	return client_socket;
}

void readRequest(int client_socket) {
	char *buffer = (char *) calloc(MAX_LEN, sizeof(char));
	if (recv(client_socket, buffer, MAX_LEN, 0) > 0) {
		char *marker = strchr(buffer, ' ');
		*marker = '\0';
		if (strcmp(buffer, "GET") == 0) {
			strcpy(buffer, "www");
			strcat(buffer, marker+1);
			marker = strchr(buffer, ' ');
			*marker = '\0';
			if (strcmp(buffer, "www/") == 0) {
				strcpy(buffer, "www/index.htm");
			}
			if (fileExists(buffer)) {
				response(200, client_socket, buffer);
			}else{
				response(404, client_socket, NULL);
			}
		} else {
			response(400, client_socket, NULL);
		}
	}
	close(client_socket);
	free(buffer);
}

void iterative(int reservedPort) {
	int client_socket, size = sizeof(client);
	for (;;) {
		readRequest(acceptConnection(reservedPort, size));
	}
}

void threads(int reservedPort) {
	int client_socket, size = sizeof(client), i = -1;
	pthread_t thread_id[NUM_THREADS];
	for (;;) {
		i == NUM_THREADS ? i = 0 : i++;
		client_socket = acceptConnection(reservedPort, size);
		pthread_create(&thread_id[i], NULL, readRequest, *(int*)&client_socket);
	}
	pthread_exit(NULL);
}

int findId(int count) {
	if (count < CONSUMER_TAM-1) {
		return count++;
	}
	return 0;
}

void *consumer(void * arg) {
	int id, * fila = (int *) arg;
	for (;;) {
		sem_wait(&hasClient);
		id = findId(clientID);
		sem_post(&hasSpace);
		readRequest(fila[id]);
	}
}

void producerConsumer(int reservedPort) {
	pthread_t threads[CONSUMER_TAM];
	sem_init(&hasSpace, 0, CONSUMER_TAM);
	sem_init(&hasClient, 0, 0);
	int i, client_socket, fila[CONSUMER_TAM], id = 0, size = sizeof(client);
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_create(&(threads[i]), NULL, consumer, (void *)&fila);
	}
	for (;;) {
		client_socket = acceptConnection(reservedPort, size);
		sem_wait(&hasSpace);
		fila[findId(id)] = client_socket;
		sem_post(&hasClient);
	}
}

void competitor(int reservedPort) {
	int client_socket, i, size = sizeof(client);
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(reservedPort, &readfds);
	for (;;) {
		if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) > 0) {
			for (i = 0; i < FD_SETSIZE; i++) {
				if (FD_ISSET(i, &readfds)) {
					if (i == reservedPort) {
						client_socket = acceptConnection(reservedPort, size);
						FD_SET(client_socket, &readfds);
					} else {
						readRequest(client_socket);
						FD_CLR(i, &readfds);
					}
				}
			}
		}
	}
}