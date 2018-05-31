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

#define NUM_BYTES 1024

int newSocket;
struct hostent *host;
struct sockaddr_in server_addr;
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
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(server_addr.sin_zero),8); 
    
    if (connect(newSocket, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1) {
        perror("Connect");
        exit(1);
    }

    return newSocket;
}

//Função que trata dados recebidos
subs* receiveData(int sock,subs *message) {

	bytes_recv=recv(sock,recv_data,NUM_BYTES,0);
    recv_data[bytes_recv] = '\0';

    message = structureMessage(message,recv_data,bytes_recv);

    return message;

    /* CODIGO INICIAL SLIDES
    if (strcmp(recv_data,"q") == 0 || strcmp(recv_data,"Q") == 0) {
        close(sock);
        exit(1);
    }
    else
        printf("\nReceived data = %s " , recv_data);
    */
}

//Função que trata dados a serem enviados (VAI MUDAR MUITO AINDA) 
void sendData(char *comando, card carta,int sock) {

    char message[11] = comando;
    int len = (int) strlen(comando);

    //strcpy(message,comando);
    strcat(message,",");
    strcat(message,carta.suit);
    strcat(message,carta.card);

	strcpy(send_data,message);
	send(sock,send_data,strlen(send_data), 0);

	/*
	printf("\nSEND (q or Q to quit) : ");
    gets(send_data);
        
	if (strcmp(send_data,"q") != 0 && strcmp(send_data,"Q") != 0)
		send(sock,send_data,strlen(send_data), 0); 
	else {
		send(sock,send_data,strlen(send_data), 0);   
		close(sock);
		exit(1);
	}
	*/
}

