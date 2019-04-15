#ifndef server_functions_h
#define server_functions_h

#define IP "127.0.0.1"
#define CONSUMER_TAM 256
#define MAX_LEN 4096
#define NUM_THREADS 8

struct sockaddr_in client;
sem_t hasSpace;
sem_t hasClient;
int clientID;

int createSocket(int port);
char *verifyExtension(char *file);
void responseOK(int client_socket, char *file);
void response(int type, int client_socket, char *file);
int fileExists(const char *filename);
int acceptConnection(int reservedPort, int size);
void readRequest(int client_socket);
void iterative(int listenSocket);
void threads(int listenSocket);
int findId(int count);
void *consumer(void * arg);
void producerConsumer(int listenSocket);
void competitor(int listenSocket);

#endif