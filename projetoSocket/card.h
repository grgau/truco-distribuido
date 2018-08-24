typedef struct card {
	int card;
	char suit;
	int potencia;
} card;

typedef struct placar {
	int meuTime;
	int timeInimigo;
} placar;

void cardString(char *card, int number) {
	sprintf(card,"%d",number);
}

int cardNumber(char *card) {
	char number[3];

	memset(number, 0, sizeof number);	// Limpa vetor number (corrige bug de aparecer um caracter a mais nas cartas após receber um 10)

	int i=1;

	while(card[i] != '\0') {
		//printf("cardNumber[%d]: %c\n", i, card[i]);			//Coloquei isso aqui pra debugar caso o bug do 80 aparecer dnv.
		number[i-1] = card[i];
		i++;
	}
	number[i] = '\0';


	return atoi(number);
}

void receiveCards(subs *message,card *cards) {
	int i;
	for (i=0;i<3;i++) {
		cards[i].suit = message->info[0];
		cards[i].card = cardNumber(message->info);
		cards[i].potencia = -1;	// Inicia potencia da carta com 0, será modificada após receber o vira
		message = message->prox;
	}
}

//Retorna a "potencia" da carta "carta" baseado no valor da carta "vira".
int getPotencia(card carta, card vira){
	int potencia, potVira, potManilha;

	// potVira = (vira.card + 6) % 10;
	potencia = (carta.card + 6) %10; 						//Assim o 4 tem valor de 0, 5 tem valor de 1, etc, seguindo pela ordem de "potencia".
	potManilha = (vira.card + 7) % 10;
	if(potencia == (potManilha)){								//Verifica se a carta atual é manilha{
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
			// printf("Entrou aqui!");
			potencia--;
		}
		return potencia;
	}

void organizeCards(card *cards, card vira){
	card aux;
	int i, j, potCartaAtual, potAux, indice;

	// potAux = getPotencia(cards[0], vira);

	// Ordena as cartas
	for(i = 0; i<3; i++) {
		potAux = getPotencia(cards[i], vira);
		indice = i;
		for(j = 0; j < 3; j++){												//Esse for serve só pra printar a ordem de potencia atual das cartas na ordenação pra eu ver se tava certo. Pode comentar/tirar se precisar.
			cards[j].potencia = getPotencia(cards[j], vira);	// Atribui as potencias pra cada carta
			//printf("Carta %c%d - Potencia : %d\n", cards[j].suit, cards[j].card, cards[j].potencia);
		}
		for(j = i; j < 3; j++){
			potCartaAtual = getPotencia(cards[j], vira);
			if(potCartaAtual > potAux){
				potAux = potCartaAtual;
				indice = j;
			}
		}

		//printf("O indice que será trocado é: %d\n", indice);
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


card receiveVira(subs *message, card vira, char *IDStart) {
	vira.suit = message->info[0];
	vira.card = cardNumber(message->info);
	strcpy(IDStart,message->prox->info);
	return vira;
}

card receiveCard(subs *message, card carta){
	carta.suit = message->info[0];
	carta.card = cardNumber(message->info);
	return carta;
}

void displayCards(card *cards) {
	int i;
	printf("\nCards:\n");
	for(i=0;i<3;i++) {
		printf("	Card: %d\n	Suit: %c\n	Potencia: %d\n\n", cards[i].card, cards[i].suit, cards[i].potencia);
	}
}

void displayVira(card vira, char *IDStart) {
	printf("\nVira:\n  Card: %d\n  Suit: %c\n  IDStart: %s\n\n", vira.card, vira.suit, IDStart);
}

void displayCard(card carta) {
	printf("\nCarta:\n  Card: %d\n  Suit: %c\n\n", carta.card, carta.suit);
}



/*int valorCarta(card *carta){
 int valor =0;
   if(strcmp(&carta->suit,"P")==0){
    valor=30;
   }
   else if(strcmp(&carta->suit,"C")==0){
   valor=20;
   }
   else if(strcmp(&carta->suit,"E")==0){
   valor=10;
   }
   else if(strcmp(&carta->suit,"O")==0){
   valor=0;
   }
   valor = valor + carta->card;
 return valor;

}*/

/*int pegarMaiorCarta(card *cards, int quantCartas){
	card *cartas = cards;
	int maiorValor = 0, indice=0,valor=0;
	for (int i=0;i<quantCartas;i++){
		valor = valorCarta(&cartas[i]);
		if(valor>maiorValor){
			maiorValor = valor;
			indice=i;
		}
	}
	return indice;
}*/

int pegarMaiorCarta(card *cards) {	// Retorna primeira carta da mão ordenada, isto é, a maior
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

int pegarMenorCarta(card *cards) {	// Retorna menor carta, tratando caso de cartas descartadas
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

/*int pegarMenorCarta(card *cards, int quantCartas){
	card *cartas = cards;
	int menorValor = 0, indice=0,valor=0;
	for (int i=0;i<quantCartas;i++){
		valor = valorCarta(&cartas[i]);
		if(valor<menorValor){
			menorValor = valor;
			indice=i;
		}
	}
	return indice;
}*/

/*int pegarDoMeio(int maior, int menor){
	if(maior==0){
		if(menor==1){
			return 2;
		}
		if(menor==2){
			return 1;
		}
	}
	if(maior==1){
		if(menor==0){
			return 2;
		}
		if(menor==2){
			return 1;
		}
	}
	if(maior==2){
		if(menor==0){
			return 1;
		}
		if(menor==1){
			return 0;
		}
	}
}*/

int whichCardSend(int jogadaDaRodada, int rodada, char *WinnerAtMoment,int valorWinnerAtMoment,char *eu, char *parceiro, card *cartas, placar contadorRodada){
	int minhaMaior, minhaMenor, cartaMeio;

	//primeira rodada ( Deve-se tentar sempre ganhar a primeira)
	if(rodada == 1){
		minhaMaior = pegarMaiorCarta(cartas/*,3*/);
		minhaMenor = pegarMenorCarta(cartas/*,3*/);
		cartaMeio = pegarDoMeio(/*minhaMaior,minhaMenor*/cartas);

		//parceiro esta ganhando
		if(strcmp(parceiro,WinnerAtMoment)==0){
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

//rodada do meio
	if(rodada==2){
		minhaMaior = pegarMaiorCarta(cartas/*,2*/);
		minhaMenor = pegarMenorCarta(cartas/*,2*/);
		//tamo ganhando
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

//ultima rodada
	if(rodada=3){
		minhaMaior = pegarMaiorCarta(cartas/*,1*/);
		return minhaMaior;
	}
}
