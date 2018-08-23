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
	char winnerAtMoment[10]="";
	int valorWinnerAtMoment=0;

	//variavel para controlar qual a jogada da rodada, para controlar quem é o vencedor até o momento
	//e decidir quando q o jogador deve jogar a carta.
	int jogadaDaRodada=0;
	int rodada = 1; // rodadas 1,2,3

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
	card cartaJogada;
	int valorCartaJogada;

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
     if(strcmp(message->info,"CARTA") == 0) {
      receiveCards(message->prox,cards);
      displayCards(cards);
    }
     if(strcmp(message->info,"VIRA") == 0) {
      vira = receiveVira(message->prox,vira,IDStart);
      displayVira(vira, IDStart);
			printf("Cartas ordenadas:\n");
			organizeCards(cards, vira);
			displayCards(cards);
    }

		if(strcmp(message->info,"MAO") == 0) {
			printf("RECEBEU MAO\n");
			printf("Esse é o message->prox->info: %s\n", message->prox->info);
			strcpy(IDStart,message->prox->info);
			if(strcmp(IDStart, myPlayer.ID) == 0){
				printf("Ora ora, parece que eu começo desta vez!");
			}
			jogadaDaRodada = 0;
			rodada++;
		}

		if(strcmp(IDStart,myPlayer.ID)==0 && jogadaDaRodada==0){
		 printf("EU COMECO PORRA\n");
		 //enviando carta para os demais players
		 // sendData("MC",cards,pegarMaiorCarta(cards,3),servidor);
		 // sendData("MC",cards,pegarMaiorCarta(cards,3),sockPlayers[0]);
		 // sendData("MC",cards,pegarMaiorCarta(cards,3),sockPlayers[1]);
		 // sendData("MC",cards,pegarMaiorCarta(cards,3),sockPlayers[2]);
		 jogaCarta(cards, pegarMaiorCarta(cards,3), rodada, servidor, sockPlayers);
	 }


		if(strcmp(message->info,"OK")==0){
			cartaJogada = receiveCard(message->prox,cartaJogada);

			//atualizar o atual vencedor

			//primeira jogada
			if(strcmp(winnerAtMoment,"")==0){
				strcpy(winnerAtMoment,IDStart);
				valorWinnerAtMoment = valorCarta(&cartaJogada);
			}
			else{
				//vencedor mudou, atualizar as informacoes
				if(valorWinnerAtMoment< valorCarta(&cartaJogada)){
					strcpy(winnerAtMoment,players[jogadaDaRodada].ID);
					valorWinnerAtMoment = valorCarta(&cartaJogada);
				}
			}
			jogadaDaRodada++;
			switch(jogadaDaRodada){
				case 1:{
					if(strcmp(players[1].ID,myPlayer.ID)==0){
						int indiceCarta = whichCardSend(jogadaDaRodada,0,winnerAtMoment, valorWinnerAtMoment,myPlayer.ID,myPartner.ID,cards,0);
					printf("Pediram Pra mandar: %d\n",indiceCarta );
						printf("MINHA VEZ GALERA 2º jogador a jogar\n");
						// sendData("MC",cards,indiceCarta,servidor);
						// sendData("MC",cards,indiceCarta,sockPlayers[0]);
						// sendData("MC",cards,indiceCarta,sockPlayers[1]);
						// sendData("MC",cards,indiceCarta,sockPlayers[2]);
						jogaCarta(cards, indiceCarta, rodada, servidor, sockPlayers);
					}
					break;
				}

				case 2:{
					if(strcmp(players[2].ID,myPlayer.ID)==0){
						printf("MINHA VEZ GALERA 3º jogador a jogar\n");
						// sendData("MC",cards,pegarMaiorCarta(cards,3),servidor);
						// sendData("MC",cards,pegarMaiorCarta(cards,3),sockPlayers[0]);
						// sendData("MC",cards,pegarMaiorCarta(cards,3),sockPlayers[1]);
						// sendData("MC",cards,pegarMaiorCarta(cards,3),sockPlayers[2]);
						jogaCarta(cards, pegarMaiorCarta(cards,3), rodada, servidor, sockPlayers);
					}
					break;
				}

				case 3:{
					if(strcmp(players[3].ID,myPlayer.ID)==0){
						printf("MINHA VEZ GALERA 4º jogador a jogar\n");
						// sendData("MC",cards,pegarMaiorCarta(cards,3),servidor);
						// sendData("MC",cards,pegarMaiorCarta(cards,3),sockPlayers[0]);
						// sendData("MC",cards,pegarMaiorCarta(cards,3),sockPlayers[1]);
						// sendData("MC",cards,pegarMaiorCarta(cards,3),sockPlayers[2]);
						jogaCarta(cards, pegarMaiorCarta(cards,3), rodada, servidor, sockPlayers);
					}

					if(strcmp(message->info,"OK")==0){
						printf("QUEM VENCEU A PRIMEIRA RODADA FOI: %s",winnerAtMoment );
					}
					break;
				}

			}


		}
    /*else if(strcmp(message->info,"FR") == 0) {
      //lógica de fim de rodada
    }*/
     if(strcmp(message->info,"MC") == 0) { // Receber "MC", precisa verificar de quem que foi
		 	printf("SOU EU AGR MANO\n");
  	}
		//Vindas de jogadores
     if(strcmp(message->info,"EC") == 0) {
      //lógica de armazenar carta vazia no jogador
    }
     if(strcmp(message->info,"TRUCO") == 0) {
      //lógica de aceitar ou não o truco
    }
     if(strcmp(message->info,"DESCE") == 0) {
      //lógica de ir pro truco
    }
     if(strcmp(message->info,"FORA") == 0) {
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
