#include "biblioteca.h"

int main(int argc, char *argv[ ]) {
    
    int sock = createSocket(argv[1]);

    while(1) {
        //receiveData(sock);
        sendData(sock);
    }

    return 0;
}