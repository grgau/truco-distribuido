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
#include "card.h"
#include "player.h"

#define NUM_BYTES 1024

int newSocket;
struct hostent *host;
struct sockaddr_in server_addr, my_addr;
int bytes_recv;
char send_data[NUM_BYTES],recv_data[NUM_BYTES];

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

/*	if (message != NULL) {
		printf("%s", message -> info);
	}
*/
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

//Função que trata dados a serem enviados (VAI MUDAR MUITO AINDA)
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
		perror("Unable to bind");
		exit(1);
	}

	if (listen(sock, 5) == -1) {
			perror("Listen");
			exit(1);
	}

	// Pega a porta que o sistema operacional atribuiu e poe na variavel port_listen
	/*bzero(&my_addr, sizeof(my_addr));
	int len = sizeof(my_addr);
	getsockname(sock, (struct sockaddr *) &my_addr, &len);
	inet_ntop(AF_INET, &server_addr.sin_addr, myIP, sizeof(myIP));
	port_listen = ntohs(my_addr.sin_port);*/

	printf("\nTCPServer Waiting for client on port %d\n", port_listen);
	fflush(stdout);

	return port_listen;
}



/////////////////////////////////////PRECISO VER ONDE COLOCAR ISSO depois
//Função que joga a carta na posição pos da mão, levando em consideração o numero de jogadas já feitas.
void jogaCarta(card *cards, int indiceCartaJogada, int rodada, int servidor, int *sockPlayers){
	int i;

	//Envia a carta escolhida para os demais jogadores e para o servidor.

	sendData("MC", cards, indiceCartaJogada, servidor);
	sendData("MC", cards, indiceCartaJogada, sockPlayers[0]);
	sendData("MC", cards, indiceCartaJogada, sockPlayers[1]);
	sendData("MC", cards, indiceCartaJogada, sockPlayers[2]);

	//"Limpa" a mão.
	/*if(indiceCartaJogada != 2)
		for(i = indiceCartaJogada; i < 3-rodada; i++)
			cards[i] = cards[i+1];*/

	cards[indiceCartaJogada].suit = '-';
	cards[indiceCartaJogada].card = 0;
	cards[indiceCartaJogada].potencia = -1;
}

int findJogada(char *IDStart, player myPlayer, player myPartner, player *players) {
	int i;

	if (strcmp(IDStart, myPlayer.ID) == 0) {	// Se eu for quem começa
		return 0;
	}
	else {
		for (i=0; i<4; i++) {
			if (strcmp(IDStart, players[i].ID) == 0) { // Achei quem começa
				switch (i) {
					case 0: {
						if (strcmp(IDStart, myPartner.ID) == 0)
							return 2;
						else if (strcmp(players[i+1].ID, myPlayer.ID) == 0)
							return 1;
						else
							return 3;
					break;
					}
					case 1: {
						if (strcmp(IDStart, myPartner.ID) == 0)
							return 2;
						else if (strcmp(players[i+1].ID, myPlayer.ID) == 0)
							return 1;
						else
							return 3;
					break;
					}
					case 2: {
						if (strcmp(IDStart, myPartner.ID) == 0)
							return 2;
						else if (strcmp(players[i+1].ID, myPlayer.ID) == 0)
							return 1;
						else
							return 3;
					break;
					}
					case 3: {
						if (strcmp(IDStart, myPartner.ID) == 0)
							return 2;
						else if (strcmp(players[i-1].ID, myPlayer.ID) == 0)
							return 3;
						else
							return 1;
					}
				}
			}






				/*if (strcmp(IDStart, myPartner.ID) != 0 && strcmp(IDStart, players[3].ID) != 0) { // Se eu nao for parceiro dele
					if (strcmp(players[i+1].ID, myPlayer.ID) == 0) {	// Se eu sou depois dele
						return 1;
						//jogadaDaRodada--;
					}
					else {	// Se eu sou antes dele
						return 3;
						//jogadaDaRodada--;
					}
					if (strcmp(IDStart, players[3].ID) == 0) {
						if (strcmp(players[i-1].ID, myPlayer.ID) == 0) {
							return 3;
							//jogadaDaRodada--;
						}
						else {
							return 1;
							//jogadaDaRodada--;
						}
					}
				}
				else { // Sou parceiro de quem começa
					if (i == 0)
						return 2;
					else
						return 0;
				}
			}*/
		}
	}
}
