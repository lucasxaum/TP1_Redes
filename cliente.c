#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>          /* hostent struct, gethostbyname() */
#include <arpa/inet.h>      /* inet_ntoa() to format IP address */
#include <string.h>
#define PORT 80

//Leitura do terminal

//Leitura da URL

//Leitura do arquivo
  //Se não tiver na URL, será index.html
  //Se o index.html não existir, retornar erro

//Criação da pasta

//Criação do arquivo

int main(int argc, char const *argv[]) {

  //Criar o Socket
  struct sockaddr_in address;
	struct sockaddr_in serv_addr;

	int sock = 0;
  int valread;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Erro na criação do socket \n");
		return -1;
	}

  memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

  struct hostent *host;     /* host information */
  struct in_addr h_addr;    /* internet address */

  if (argc != 2) {
    printf("ERRO DE PASSAGEM PELO TERMINAL:\n USO: ./cliente [OPCAO]\n  -- OPCAO: ENDERECO IP OU ENDERECO URL\n");
    exit(1);
  }

  printf("teste1\n");

  if ((host = gethostbyname(argv[1])) == NULL) {
    printf("Falha na recuperação do endereço IP de '%s'\n", argv[1]);
    exit(1);
  }

  h_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);
  char *endereco = inet_ntoa(h_addr);
  printf("%s\n", endereco);

  if(inet_pton(AF_INET, endereco, &serv_addr.sin_addr)<=0) {
		printf("\nEndereço inválido \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nFalha na conexão \n");
		return -1;
	}

  struct sockaddr_in server;
	char *message , server_reply[600000];

  //Send some data
  //Send some data
	message = "GET /index.html HTTP/1.1\r\nHost: \r\n\r\n";
	if( send(sock, message , strlen(message) , 0) < 0)
	{
		puts("Falha no envio");
		return 1;
	}
	puts("Dados enviados\n");

	//Receive a reply from the server
	if( recv(sock, server_reply , 600000 , 0) < 0)
	{
		puts("Falha na recuperação de dados");
	}
	puts("Dados recebidos\n");
	puts(server_reply);

  FILE *ptrArq = fopen("index.html", "w");
  fprintf(ptrArq, "%s\n", server_reply);

  system("firefox index.html");
	//char *hello = "Hello from client";
	//char buffer[1024] = {0};

  //Pesquisar pelo endereço

  return 0;
}
