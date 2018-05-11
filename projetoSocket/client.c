#include "data.h"
#include "card.h"
#include "player.h"

int main(int argc, char *argv[ ]) {

	//Estrutura pra separar substrings das mensagens enviadas e recebidas
	subs *message = NULL;

	//Abrindo conexão com servidor
    int servidor = createSocket("127.0.0.1",14000);

    //vetor de players [0]-[2] e [1]-[3] parceiros
    player players[4];

    //vetor de cartas [O,C,E,P]-[1,2,3,4,5,6,7,8,9,10]
    card cards[3];

    card vira;

    while(1) {
    	//Recebendo mensagem
        message = receiveData(servidor,message);

        //Printando mensagem desestruturada mesmo
        displayMessage(message);

        //Switch de tipos de mensagens
        //Vindas do servidor
        if(strcmp(message->info,"MESA") == 0) {
        	receivePlayers(message->prox,players);
        	displayPlayers(players);

        }
        else if(strcmp(message->info,"CARTA") == 0) {
        	receiveCards(message->prox,cards,3);
        	displayCards(cards);
        }
        else if(strcmp(message->info,"VIRA") == 0) {
        	receiveCards(message->prox,vira,1);
        	displayVira(vira);
        }
        else if(strcmp(message->info,"FR") == 0) {
        	//lógica de fim de rodada
        }
        else if(strcmp(message->info,"MC") == 0) {
        	//lógica de armazenar carta no jogador
        }

        //Vindas de jogadores
        else if(strcmp(message->info,"EC") == 0) {
        	//lógica de armazenar carta vazia no jogador
        }
        else if(strcmp(message->info,"TRUCO") == 0) {
        	//lógica de aceitar ou não o truco
        }
        else if(strcmp(message->info,"DESCE") == 0) {
        	//lógica de ir pro truco
        }
        else if(strcmp(message->info,"FORA") == 0) {
        	//lógica de fim de rodada
        }

        //e por assim vai

        //Destruindo mensagem recebida após cada interação
        message = destroyMessage(message);
        //tá enviando dados sozinho pra manter recebendo
        sendData(servidor);

        sleep(10);
    }

    return 0;
}