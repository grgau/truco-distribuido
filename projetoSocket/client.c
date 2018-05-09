#include "data.h"

int main(int argc, char *argv[ ]) {

	//Estrutura pra separar substrings das mensagens enviadas e recebidas
	subs *message = NULL;

	//Abrindo conexão com servidor
    int servidor = createSocket("127.0.0.1",14000);

    //vetor de players
    player players[4];

    while(1) {
    	//Recebendo mensagem
        message = receiveData(servidor,message);

        //Printando mensagem desestruturada mesmo
        displayMessage(message);

        //Switch de tipos de mensagens
        if(strcmp(message->info,"MESA") == 0) {
        	receivePlayers(message->prox,players);
        	displayPlayers(players);
        	message = destroyMessage(message);

        }
        else if(strcmp(message->info,"CARTA") == 0) {
        	//armazenar cartas
        }
        else if(strcmp(message->info,"VIRA") == 0) {
        	//armazenar mania
        }
        else if(strcmp(message->info,"FR") == 0) {
        	//lógica de fim de rodada
        }

        //e por assim vai

        //tá enviando dados sozinho pra manter recebendo
        sendData(servidor);

        sleep(100);
    }

    return 0;
}