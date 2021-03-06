#ifndef DATA_H

//Defines das constantes necessárias.
#define DATA_H DATA_H
#define NUM_BYTES 1024

//Header das funções criadas.
int createPlayersListen();
int createSocket(char *, int);
subs* receiveData(int, subs*);
player startGame(int, int, player);
void sendData(char *, card *, int, int);

//Includes necessários.
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "subs.h"
#include "player.h"
#include "card.h"


//Variáveis globais.
int newSocket;
struct hostent *host;
struct sockaddr_in server_addr, my_addr;
int bytes_recv;
char send_data[NUM_BYTES],recv_data[NUM_BYTES];


//Funções criadas para esta biblioteca.
//Cria um socket com o host hostname na porta port.
int createSocket(char *hostname, int port) {
	host = gethostbyname(hostname);

  if ((newSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("Socket");
      exit(1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr = *((struct in_addr *) host -> h_addr);
  bzero(&(server_addr.sin_zero),8);

  if (connect(newSocket, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1) {
      perror("Connect");
      exit(1);
  }

  return newSocket;
}

//Função que trata dados recebidos
subs* receiveData(int sock, subs *message) {
	bytes_recv=recv(sock,recv_data,NUM_BYTES,0);
  recv_data[bytes_recv] = '\0';

  message = structureMessage(message,recv_data,bytes_recv);
  return message;
}

player startGame(int sock, int port_listening, player myPlayer) {
	char startMessage[35];
	char id[10];
	char myIP[16];
	char myPort[6];

	// Cria ID aleatória de até 9 dígitos para jogador
	srand(time(NULL));
	int r = rand() % 999999999;
	snprintf(id, 9, "%d", r);

	// Pega endereço IP local e porta (transformando-a para char) do cliente
	bzero(&my_addr, sizeof(my_addr));
	int len = sizeof(my_addr);
	getsockname(sock, (struct sockaddr *) &my_addr, &len);
	inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
	snprintf(myPort, 6, "%d", port_listening);

	// Constrói a mensagem de envio
	strcpy(startMessage, id);
	strcat(startMessage, ",");
	strcat(startMessage, myIP);
	strcat(startMessage, ",");
	strcat(startMessage, myPort);
	strcpy(send_data, startMessage);

	// Envia mensagem construída para servidor
	send(sock,send_data,strlen(send_data), 0);

	// Atualiza myPlayer
	strcpy(myPlayer.ID,id);
	strcpy(myPlayer.ip,myIP);
	strcpy(myPlayer.port,myPort);

	return myPlayer;
}

//Função que trata dados a serem enviados.
void sendData(char *comando, card *cards, int index, int sock) {
	int i;
    char message[15];
    char suit[2];
    char card[3];

    strcpy(message,comando);
    strcat(message,",");

    if(cards != NULL) {
				suit[0] = cards[index].suit;
				suit[1] = '\0';

        cardString(card, cards[index].card);

        strcat(message,suit);
        strcat(message,card);
    }
    else {
        strcat(message,"NULL");
    }

	strcpy(send_data,message);
	printf("Mensagem enviada: %s\n", message);
	send(sock,send_data,strlen(send_data), 0);
}

//Função que cria o socket para receber dados dos outros jogadores.
int createPlayersListen () {
	int sock, connected, bytes_recv, true = 1;
	struct sockaddr_in server_addr, client_addr;
	int sin_size, port_listen;
	char myIP[16];

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("Socket");
			exit(1);
	}

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,&true,sizeof(int)) == -1) {
			perror("Setsockopt");
			exit(1);
	}

	// Decide porta aletaroria entre 1025 e 65535 para ouvir os outros jogadores
	srand(time(NULL));
	port_listen = rand() % 64511 + 1025;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_listen);	// Deixa que o sistema operacional escolha uma porta livre
	server_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_addr.sin_zero),8);

	if (bind(sock, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1) {
		perror("Tente novamente.\n");
		exit(1);
	}

	if (listen(sock, 5) == -1) {
			perror("Listen");
			exit(1);
	}

	printf("\nTCPServer Waiting for client on port %d\n", port_listen);
	fflush(stdout);

	return port_listen;
}

#endif
