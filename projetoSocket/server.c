#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>  // defines IP standard protocols
#include <arpa/inet.h>  // to convert host addresses
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // defines POSIX constants
#include <errno.h>   // used for error messages

int main(int argc, char  *argv[ ]) {

    int sock, connected, bytes_recv, true = 1;  
    char send_data [1024] , recv_data[1024];       
    struct sockaddr_in server_addr,  client_addr;    
    int sin_size;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket");
        exit(1);
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,&true,sizeof(int)) == -1) {
        perror("Setsockopt");
        exit(1);
    }

    server_addr.sin_family = AF_INET;         
    server_addr.sin_port = htons(14000);     
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

    printf("\nTCPServer Waiting for client on port 5000");
    fflush(stdout);
    
    while(1) {
        sin_size = sizeof(struct sockaddr_in);
        connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);
        printf("\nGot connection from (%s , %d)\n", inet_ntoa(client_addr.sin_addr), 
        ntohs(client_addr.sin_port));
        while (1) {

            //gets(send_data);
            strcpy(send_data,"MESA, Paim, 1.1.1.1, 1, Azei, 1.1.1.2, 2, Greg, 1.1.1.3, 3, Gerv, 1.1.1.4, 4");
            send(connected, send_data, strlen(send_data), 0);

            bytes_recv = recv(connected, recv_data,1024,0);
            recv_data[bytes_recv] = '\0';

            //printf("\nClient:%s" , recv_data);

            fflush(stdout);

            /*
            printf ("\n SEND (q or Q to quit) : ");
            gets(send_data);
            if (strcmp(send_data,"q") ==0 || strcmp(send_data,"Q") ==0) {
                send(connected, send_data, strlen(send_data), 0); 
                close(connected);
                break;
            }
            else
                send(connected, send_data, strlen(send_data), 0);

            bytes_recv = recv(connected, recv_data,1024,0);
            recv_data[bytes_recv] = '\0';
            
            if (strcmp(recv_data,"q") ==0 || strcmp(recv_data,"Q") ==0) { 
                close(connected);
                break;
            }
            else 
                printf("\nClient:%s " , recv_data);
            
            fflush(stdout);

            */
            
        }
    }

    close(sock);
    return 0;
} 