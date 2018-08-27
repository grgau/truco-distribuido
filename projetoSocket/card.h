#ifndef CARDS_H
#define CARDS_H CARDS_H


//Estruturas referentes ao tipo card.
typedef struct card {
	int card;
	char suit;
	int potencia;
} card;

typedef struct placar {
	int meuTime;
	int timeInimigo;
} placar;


//Header das funções criadas.
void pedirTruco(int);
void aceitaTruco(int);
void displayCard(card);
int cardNumber(char *);
int pegarDoMeio(card *);
void naoAceitaTruco(int);
void displayCards(card *);
int pegarMaiorCrta(card *);
int pegarMenorCarta(card *);
int getPotencia(card, card);
void cardString(char *, int);
card receiveCard(subs *, card);
void displayVira(card, char *);
void organizeCards(card *, card);
void receiveCards(subs *, card *);
card receiveVira(subs *, card, char *);
void jogaCarta(card *, int, int, int, int *);
void escondeCarta(card *, int, int, int, int *);
int findJogada(char *, player, player, player *);


//Includes necessários.
#include <stdio.h>
#include "data.h"
#include "player.h"


//Declaração das funções.
//Passa o numero da carta para string.
void cardString(char *card, int number) {
	sprintf(card,"%d",number);
}

//Transforma o string card em int.
int cardNumber(char *card) {
	char number[3];
	int i=1;

	memset(number, 0, sizeof number);
	while(card[i] != '\0') {
		number[i-1] = card[i];
		i++;
	}
	number[i] = '\0';
	return atoi(number);
}

//Recebe as cartas, fazendo o tratamento das strings para o tipo cards.
void receiveCards(subs *message,card *cards) {
	int i;
	for (i=0;i<3;i++) {
		cards[i].suit = message->info[0];
		cards[i].card = cardNumber(message->info);
		cards[i].potencia = -1;	// Inicia potencia da carta com -1, será modificada após receber o vira
		message = message->prox;
	}
}

//Retorna a "potencia" da carta "carta" baseado no valor da carta "vira".
int getPotencia(card carta, card vira){
	int potencia, potVira, potManilha;

	potencia = (carta.card + 6) %10; 						//Assim o 4 tem valor de 0, 5 tem valor de 1, etc, seguindo pela ordem de "potencia".
	potManilha = (vira.card + 7) % 10;
	if(potencia == (potManilha)){								//Verifica se a carta atual é manilha
		potencia = 9;
		switch((carta.suit)){
			case 'P':
				potencia++;
			case 'C':
				potencia++;
			case 'E':
				potencia++;
			case 'O':
				potencia++;
		}
	}
	else
	if(potManilha <= potencia){												//Se já passou da "potencia" da manilha, as cartas para frente possuem um grau a menos de potencia.
		potencia--;
	}
	return potencia;
}

//Organiza as cartas da mão em ordem crescente de potencia.
void organizeCards(card *cards, card vira){
	card aux;
	int i, j, potCartaAtual, potAux, indice;

	// Ordena as cartas
	for(i = 0; i<3; i++) {
		potAux = getPotencia(cards[i], vira);
		indice = i;
		for(j = 0; j < 3; j++){
			cards[j].potencia = getPotencia(cards[j], vira);	// Atribui as potencias pra cada carta
		}
		for(j = i; j < 3; j++){
			potCartaAtual = getPotencia(cards[j], vira);
			if(potCartaAtual > potAux){
				potAux = potCartaAtual;
				indice = j;
			}
		}

		aux.card = cards[i].card;
		aux.suit = cards[i].suit;
		aux.potencia = cards[i].potencia;
		cards[i].card = cards[indice].card;
		cards[i].suit = cards[indice].suit;
		cards[i].potencia = cards[indice].potencia;
		cards[indice].card = aux.card;
		cards[indice].suit = aux.suit;
		cards[indice].potencia = aux.potencia;
	}
}

//Faz o tratamento da carta VIRA para o tipo card e também salva qual jogador será o primeiro pelo ID.
card receiveVira(subs *message, card vira, char *IDStart) {
	vira.suit = message->info[0];
	vira.card = cardNumber(message->info);
	strcpy(IDStart,message->prox->info);
	return vira;
}

//Faz o tratamento de uma carta para o tipo card.
card receiveCard(subs *message, card carta){
	carta.suit = message->info[0];
	carta.card = cardNumber(message->info);
	return carta;
}

//Printa informações sobre a carta.
void displayCards(card *cards) {
	int i;
	printf("\nCards:\n");
	for(i=0;i<3;i++) {
		printf("	Card: %d\n	Suit: %c\n	Potencia: %d\n\n", cards[i].card, cards[i].suit, cards[i].potencia);
	}
}

//Printa informações sobre a VIRA.
void displayVira(card vira, char *IDStart) {
	printf("\nVira:\n  Card: %d\n  Suit: %c\n  IDStart: %s\n\n", vira.card, vira.suit, IDStart);
}

//Printa informações sobre a carta.
void displayCard(card carta) {
	printf("\nCarta:\n  Card: %d\n  Suit: %c\n\n", carta.card, carta.suit);
}

// Retorna primeira carta da mão ordenada, isto é, a maior
int pegarMaiorCarta(card *cards) {
	int indice;

	if(cards[0].card != 0) {
		printf("\npegarMaiorCarta: maior\n");
		indice = 0;
		return indice;
	}
	else if(cards[1].card != 0) {
		printf("\npegarMaiorCarta: medio\n");
		indice = 1;
		return indice;
	}
	else {
		printf("\npegarMaiorCarta: menor\n");
		indice = 2;
		return indice;
	}
}

// Retorna menor carta, tratando caso de cartas descartadas
int pegarMenorCarta(card *cards) {
	int indice;

	if(cards[2].card != 0) {	// Se ainda tiver a ultima carta da mao, retorna ela
		printf("\npegarMenorCarta: menor\n");
		indice = 2;
		return indice;
	}
	else if (cards[1].card != 0) { // Se ja tiver descartado a ultima carta, retorna a do meio mesmo
		printf("\npegarMenorCarta: meio\n");
		indice = 1;
		return indice;
	}
	else {	// Se ja tiver descartado duas, retorna a maior
		printf("\npegarMenorCarta: maior\n");
		indice = 0;
		return indice;
	}
}

// Retorna a carta média, tratando caso de cartas descartadas
int pegarDoMeio(card *cards) {
	int indice;

	if(cards[1].card != 0) {	// Se ainda tem carta do meio manda do meio
		printf("\npegarDoMeio: meio\n");
		indice = 1;
		return indice;
	}
	else if(cards[0].card != 0) {	// Se não tem carta do meio, manda maior mesmo
		printf("\npegarDoMeio: maior\n");
		indice = 0;
		return indice;
	}
	else {	// Se não tem nem do meio nem maior manda menor mesmo
		printf("\npegarDoMeio: menor\n");
		indice = 2;
		return indice;
	}
}

//Faz a decisão de qual carta jogar.
int whichCardSend(int taTrucado, int jogadaDaRodada, int rodada, char *WinnerAtMoment,int valorWinnerAtMoment,char *eu, char *parceiro, card *cartas, placar contadorRodada){
	int minhaMaior, minhaMenor, cartaMeio;

	//primeira rodada ( Deve-se tentar sempre ganhar a primeira)
	if(rodada == 1){
		minhaMaior = pegarMaiorCarta(cartas);
		minhaMenor = pegarMenorCarta(cartas);
		cartaMeio = pegarDoMeio(cartas);

		//parceiro esta ganhando ou eu comeco
		if(strcmp(parceiro,WinnerAtMoment)==0 || jogadaDaRodada==0){
			//jogar a mais fraca
			return minhaMenor;
		}
		else{
			//adversario esta ganhando no momento
			//caso seja o primeiro a jogar da dupla
			if(jogadaDaRodada==1){
				//se a do meio ganhar: tente
				if(cartas[cartaMeio].potencia > valorWinnerAtMoment){
					return cartaMeio;
				}
				//descarta a menor
				return minhaMenor;
			}

			if(jogadaDaRodada==3){
				//segundo da dupla a jogar e o parceiro nao esta ganhando
				if(cartas[minhaMenor].potencia > valorWinnerAtMoment){
					return minhaMenor;
				}
				if(cartas[cartaMeio].potencia > valorWinnerAtMoment){
					return cartaMeio;
				}
				if(cartas[minhaMaior].potencia > valorWinnerAtMoment){
					return minhaMaior;
				}
				return minhaMenor;
			}
		}
	}
	else if(rodada==2){
		//rodada do meio
		minhaMaior = pegarMaiorCarta(cartas);
		minhaMenor = pegarMenorCarta(cartas);
		//nosso time está ganhando na pontuação.
		if(contadorRodada.meuTime==1){
			//só pode ser na jogadaDaRodada = 3
			if(cartas[minhaMenor].potencia > valorWinnerAtMoment){
				return minhaMenor;
			}
			if(cartas[minhaMaior].potencia > valorWinnerAtMoment){
				return minhaMaior;
			}
			return minhaMenor;
		}
		return minhaMaior;
	}
	if(rodada=3){
		//ultima rodada
		minhaMaior = pegarMaiorCarta(cartas);
		return minhaMaior;
	}
}

//Verifica a necessidade (ou não) de pedir truco.
int devoPedirTruco(card *cards,card vira,int rodada, placar contadorRodada){
	int probabilidade = 0;

	card *cartas = cards;
	organizeCards(cartas, vira);


	if(rodada==1){
		//precisa ter no minimo 2 cartas fortes
		probabilidade += getPotencia(cartas[0],vira)-7;
		probabilidade += getPotencia(cartas[1],vira)-7;
	}

	else if(rodada==2){
		//se estiver ganhando
		if(contadorRodada.meuTime==1){
			//precisa ter 1 carta forte
			probabilidade += getPotencia(cartas[0],vira)-7;

		}
		if(contadorRodada.meuTime==0){
			//precisa ter 2 cartas fortes
			probabilidade += getPotencia(cartas[0],vira)-8;
			probabilidade += getPotencia(cartas[1],vira)-8;
		}
	}

	else if(rodada==3){
			//jogo está empatado
			//precisa ter 1 carta forte
			probabilidade += getPotencia(cartas[0],vira)-8;
	}


	//probabilidade < 0 ( cartas fracas)
	//probabilidade > 0 ( cartas aceitaveis)

	//adicionando aleatoriedade (roubo)
	probabilidade -= rand()%3;
	if(probabilidade>0) return 1;
	return 0;
}


//Função que joga a carta na posição pos da mão, levando em consideração o numero de jogadas já feitas.
void jogaCarta(card *cards, int indiceCartaJogada, int rodada, int servidor, int *sockPlayers){
	int i;

	//Envia a carta escolhida para os demais jogadores e para o servidor.
	sendData("MC", cards, indiceCartaJogada, servidor);
	sendData("MC", cards, indiceCartaJogada, sockPlayers[0]);
	sendData("MC", cards, indiceCartaJogada, sockPlayers[1]);
	sendData("MC", cards, indiceCartaJogada, sockPlayers[2]);

	cards[indiceCartaJogada].suit = '-';
	cards[indiceCartaJogada].card = 0;
	cards[indiceCartaJogada].potencia = -1;
}

//Função que manda NULL pros players e a carta pro servidor.
void escondeCarta(card *cards, int indiceCartaJogada, int rodada, int servidor, int *sockPlayers){
	int i;

	//Envia a carta escolhida para os demais jogadores e para o servidor.
	sendData("EC", cards, indiceCartaJogada, servidor);
	sendData("EC", NULL, indiceCartaJogada, sockPlayers[0]);
	sendData("EC", NULL, indiceCartaJogada, sockPlayers[1]);
	sendData("EC", NULL, indiceCartaJogada, sockPlayers[2]);

	cards[indiceCartaJogada].suit = '-';
	cards[indiceCartaJogada].card = 0;
	cards[indiceCartaJogada].potencia = -1;
}

//Pede truco.
void pedirTruco(int servidor){
	sendData("TRUCO",NULL,0,servidor);
}

//Aceita truco.
void aceitaTruco(int servidor){
	sendData("DESCE",NULL,0,servidor);
}

//Não aceita o truco.
void naoAceitaTruco(int servidor){
	sendData("FORA",NULL,0,servidor);
}

//Retorna qual jogada estamos da rodada. 
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
		}
	}
}

#endif
