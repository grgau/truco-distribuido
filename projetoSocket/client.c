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

	// Conta o placar de pontos, de cada rodada e da partida
	placar contadorRodada, contadorPartida;
	contadorRodada.meuTime = 0;
	contadorRodada.timeInimigo = 0;
	contadorPartida.meuTime = 0;
	contadorPartida.timeInimigo = 0;

	char winnerAtMoment[10]="";
	int valorWinnerAtMoment=0;
	int indiceCartaJogada;
	int contJogadas = 0 ;

	//variavel para controlar qual a jogada da rodada, para controlar quem é o vencedor até o momento
	//e decidir quando q o jogador deve jogar a carta.
	int jogadaDaRodada = -1;
	int rodada = 1; // rodadas 1,2,3
	int joguei = 0;
	int indiceCarta;

	//Estrutura pra separar substrings das mensagens enviadas e recebidas
	subs *message = NULL;

	// Cria servidor de conexão para jogadores, retornando porta que esta ouvindo
	port_listening = createPlayersListen();

	//Abrindo conexão com servidor do Aleardo
  int servidor = createSocket(IP_SERVER, PORT_SERVER);


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

		// Mensagem de MESA vinda do servidor, define quem são os jogadores (ID, IP, Porta)
    if(strcmp(message->info,"MESA") == 0) {
      receivePlayers(message->prox, players);
      displayPlayers(players);
			myPartner = whoIsMyPartner(players, myPlayer); // Retorna ID do parceiro
			printf("myID: %s\nmyPartner: %s\n\n", myPlayer.ID, myPartner.ID);

			// Cria 3 sockets, um com cada jogador
			for (i=0; i < 4; i++)
				if (players[i].ID != myPlayer.ID) // Para não criar socket consigo mesmo
					sockPlayers[i] = createSocket(players[i].ip, atoi(players[i].port));
    }

		// Mensagem de CARTA vinda do servidor, define as cartas da mão recebidas
    if(strcmp(message->info,"CARTA") == 0) {
    	receiveCards(message->prox,cards);
    	displayCards(cards);
    }

		// Mensagem de VIRA vinda do servidor, define qual a carta de vira
    if(strcmp(message->info,"VIRA") == 0) {
    	vira = receiveVira(message->prox,vira,IDStart);
    	displayVira(vira, IDStart);
			printf("Cartas ordenadas:");
			organizeCards(cards, vira);
			displayCards(cards);

			jogadaDaRodada = findJogada(IDStart, myPlayer, myPartner, players);
			printf("\n\n -- jogadaDaRodada: %d -- \n\n", jogadaDaRodada);

			strcpy(winnerAtMoment,"");

			// Pra quando começa uma nova partida
			contJogadas = 0 ;

			joguei = 0;
    }

		// Mensagem de MAO vinda do servidor, define quem começa a proxima rodada
		if(strcmp(message->info,"MAO") == 0) {

			// Verifica quem ganhou e conta no placar da rodada
			if ((strcmp(message->prox->info, myPlayer.ID) == 0) || (strcmp(message->prox->info, myPartner.ID) == 0))
				contadorRodada.meuTime++;
			else
				contadorRodada.timeInimigo++;
			printf("\nPlacar da rodada: meuTime: %d - timeInimigo: %d\n", contadorRodada.meuTime, contadorRodada.timeInimigo);

			printf("RECEBEU MAO\n");
			printf("Jogador que começa a rodada: %s\n", message->prox->info);
			strcpy(IDStart,message->prox->info);

			//if(strcmp(message->prox->info, myPlayer.ID) == 0)
			if(strcmp(IDStart, myPlayer.ID) == 0)
				printf("\n\nOra ora, parece que eu começo desta vez!\n\n");

			strcpy(winnerAtMoment,"");
			valorWinnerAtMoment=0;
			contJogadas = 0;
			rodada++;
			displayCards(cards);

			jogadaDaRodada = findJogada(IDStart, myPlayer, myPartner, players);
			printf("\n\n -- jogadaDaRodada: %d -- \n\n", jogadaDaRodada);

			joguei = 0;
		}

		// Estratégia da primeira jogada da rodada
		// Joga a maior carta da mão
		if(/*strcmp(IDStart, myPlayer.ID)==0 &&*/ jogadaDaRodada == 0 && /*strcmp(winnerAtMoment,myPlayer.ID) != 0*/ contJogadas == jogadaDaRodada && joguei == 0) {
			indiceCarta = pegarMaiorCarta(cards);
			printf("jogada: %d, contJogadas: %d\n", jogadaDaRodada, contJogadas);
			// enviando carta para os demais players
			/*sendData("MC",cards,indiceCartaJogada,servidor);
			sendData("MC",cards,indiceCartaJogada,sockPlayers[0]);
			sendData("MC",cards,indiceCartaJogada,sockPlayers[1]);
			sendData("MC",cards,indiceCartaJogada,sockPlayers[2]);*/
			//strcpy(winnerAtMoment,IDStart);
			//valorWinnerAtMoment = getPotencia(cards[indiceCarta], vira);

			/*if(cards[indiceCartaJogada].card==0)
				printf("IRIA MANDAR DNOVO\n" );
			else*/
			jogaCarta(cards, indiceCarta, rodada, servidor, sockPlayers);
			//jogadaDaRodada++;
			//displayCards(cards);
			joguei = 1;
		}

		// Mensagem de OK vinda do servidor, confirma recebimento de carta jogada por um jogador
		if(strcmp(message->info, "OK") == 0) {
			contJogadas++;
			cartaJogada = receiveCard(message->prox,cartaJogada);

			// Se eu comecei a rodada, guarda eu como winnerAtMoment e potência da carta que joguei
			if(strcmp(winnerAtMoment, "") == 0){
				strcpy(winnerAtMoment,IDStart);
				valorWinnerAtMoment = getPotencia(cartaJogada, vira);	// Atribui as potencias pra cada carta
			}

			// Se eu não comecei a rodada, atualiza quem está ganhando e com qual potência de carta
			else {
				if(valorWinnerAtMoment < getPotencia(cartaJogada, vira)) {
					strcpy(winnerAtMoment,players[jogadaDaRodada].ID);
					valorWinnerAtMoment = getPotencia(cartaJogada, vira);
				}
			}
			//if (rodada == 1)
			//jogadaDaRodada++;

			printf("\nwinnerAtMoment: %s - valorWinnerAtMoment: %d\n", winnerAtMoment, valorWinnerAtMoment);
			printf("jogada: %d, contJogadas: %d\n", jogadaDaRodada, contJogadas);
			if (jogadaDaRodada == contJogadas) {
				printf("\n\nentrou if\n\n");
			switch(jogadaDaRodada){
				case 1: {
					//if(strcmp(players[1].ID,myPlayer.ID)==0/* && strcmp(winnerAtMoment,myPlayer.ID)!=0*/){
						printf("MINHA VEZ GALERA 2º jogador a jogar\n");
						indiceCarta = whichCardSend(jogadaDaRodada, rodada, winnerAtMoment, valorWinnerAtMoment,myPlayer.ID,myPartner.ID,cards, contadorRodada);
						printf("Pediram Pra mandar: %d\n",indiceCarta );
						/* sendData("MC",cards,indiceCarta,servidor);
						sendData("MC",cards,indiceCarta,sockPlayers[0]);
						sendData("MC",cards,indiceCarta,sockPlayers[1]);
						sendData("MC",cards,indiceCarta,sockPlayers[2]); */
						jogaCarta(cards, indiceCarta, rodada, servidor, sockPlayers);
					//}
					//jogadaDaRodada++;
					printf("%d %d \n",strcmp(players[1].ID,myPlayer.ID)==0 ,strcmp(winnerAtMoment,myPlayer.ID)!=0 );
					break;
				}

				case 2:{
					//if(strcmp(players[2].ID,myPlayer.ID)==0/* && strcmp(winnerAtMoment,myPlayer.ID)!=0*/){
						printf("MINHA VEZ GALERA 3º jogador a jogar\n");
						indiceCarta = whichCardSend(jogadaDaRodada, rodada, winnerAtMoment, valorWinnerAtMoment,myPlayer.ID,myPartner.ID,cards, contadorRodada);

						/*sendData("MC",cards,indiceCarta,servidor);
						sendData("MC",cards,indiceCarta,sockPlayers[0]);
						sendData("MC",cards,indiceCarta,sockPlayers[1]);
						sendData("MC",cards,indiceCarta,sockPlayers[2]);
						if(cards[indiceCarta].card != 0)*/

						jogaCarta(cards, indiceCarta, rodada, servidor, sockPlayers);
					//}
					break;
				}

				case 3:{
					//if(strcmp(players[3].ID,myPlayer.ID)==0/* && strcmp(winnerAtMoment,myPlayer.ID)!=0*/){
						printf("MINHA VEZ GALERA 4º jogador a jogar\n");
						indiceCarta = whichCardSend(jogadaDaRodada, rodada, winnerAtMoment, valorWinnerAtMoment,myPlayer.ID,myPartner.ID,cards, contadorRodada);
						/* sendData("MC",cards,pegarMaiorCarta(cards,3),servidor);
						sendData("MC",cards,pegarMaiorCarta(cards,3),sockPlayers[0]);
						sendData("MC",cards,pegarMaiorCarta(cards,3),sockPlayers[1]);
						sendData("MC",cards,pegarMaiorCarta(cards,3),sockPlayers[2]);
						if(cards[indiceCarta].card != 0)*/

						jogaCarta(cards, indiceCarta, rodada, servidor, sockPlayers);
					//}

					/*if(strcmp(message->info,"OK")==0) {
						printf("\nQUEM VENCEU A PRIMEIRA RODADA FOI: %s\n",winnerAtMoment );


					}*/
					break;
				}
				default:{
					printf("CAIU DEFAULTTT\n" );
					break;
				}
			}
		}

		}

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

		// Servidor envia mensagem de fim da rodada
		if(strcmp(message->info,"FR") == 0) {

		}

    //Destruindo mensagem recebida após cada interação
    message = destroyMessage(message);
  }
  return 0;
}
