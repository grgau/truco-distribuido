#include "data.h"

#define IP_SERVER "127.0.0.1"
#define PORT_SERVER 14000

int main(int argc, char *argv[ ]) {
	int port_listening, i ;
	int sockPlayers[3]; // Vetor de sockets com players, cada indice é um jogador conectado

	//vetor de players [0]-[2] e [1]-[3] parceiros
	player players[4];
	player myPlayer;
	player myPartner;

	//Estrutura pra separar substrings das mensagens enviadas e recebidas
	subs *message = NULL;

	//Abrindo conexão com servidor do Aleardo
  int servidor = createSocket(IP_SERVER, PORT_SERVER);

	// Cria servidor de conexão para jogadores, retornando porta que esta ouvindo
	port_listening = createPlayersListen();

	// Mensagem inicial de entrada do jogador na partida ≪ "ID", endereçoIP, porta ≫, retorna quem sou
	myPlayer = startGame(servidor, port_listening, myPlayer);

  //vetor de cartas [O,C,E,P]-[1,2,3,4,5,6,7,8,9,10]
  card cards[3];
	card vira;

	char IDStart[10];

  while(1) {
    //Recebendo mensagem
    message = receiveData(servidor,message);

    //Printando mensagem
    displayMessage(message);

    //Switch de tipos de mensagens
    //Vindas do servidor
    if(strcmp(message->info,"MESA") == 0) {
      receivePlayers(message->prox, players);
      displayPlayers(players);
			printf("\nmyID: %s", myPlayer.ID);
			myPartner = whoIsMyPartner(players, myPlayer); // Retorna ID do parceiro
			printf("\nmyPartner: %s", myPartner.ID);

			// Cria 3 sockets, um com cada jogador
			for (i=0; i < 4; i++)
				if (players[i].ID != myPlayer.ID) // Para não criar socket consigo mesmo
					sockPlayers[i] = createSocket(players[i].ip,atoi(players[i].port));
    }
    else if(strcmp(message->info,"CARTA") == 0) {
      receiveCards(message->prox,cards);
      displayCards(cards);
    }
    else if(strcmp(message->info,"VIRA") == 0) {
      vira = receiveVira(message->prox,vira,IDStart);
      displayVira(vira, IDStart);
    }
    /*else if(strcmp(message->info,"FR") == 0) {
      //lógica de fim de rodada
    }*/
    else if(strcmp(message->info,"MC") == 0) { // Receber "MC", precisa verificar de quem que foi

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

    //sendData("FORA",NULL,0,servidor);
    //tá enviando dados sozinho pra manter recebendo
  }

  return 0;
}
