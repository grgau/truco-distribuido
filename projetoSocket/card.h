
typedef struct card {
	int card;
	char suit;
} card;

void cardString(char *card,int number) {
	sprintf(card,"%d",number);
}

int cardNumber(char *card) {
	char number[3];
	int i=1;

	while(card[i] != '\0') {
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
		message = message->prox;
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
		printf("  Card: %d\n  Suit: %c\n\n",cards[i].card,cards[i].suit);
	}
}

void displayVira(card vira, char *IDStart) {
	printf("\nVira:\n  Card: %d\n  Suit: %c\n  IDStart: %s\n\n", vira.card, vira.suit, IDStart);
}

void displayCard(card carta) {
	printf("\nCarta:\n  Card: %d\n  Suit: %c\n\n", carta.card, carta.suit);
}



int valorCarta(card *carta){
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

}

int pegarMaiorCarta(card *cards, int quantCartas){
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

}

int pegarMenorCarta(card *cards, int quantCartas){
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

}

int pegarDoMeio(int maior, int menor){
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
}

int whichCardSend(int jogadaDaRodada,int rodada, char *WinnerAtMoment,int valorWinnerAtMoment,char *eu, char *parceiro, card *cartas,int placar){
	int minhaMaior = 0;
	int minhaMenor = 0;


	//primeira rodada ( Deve-se tentar sempre ganhar a primeira)
	if(rodada ==0){
		minhaMaior = pegarMaiorCarta(cartas,3);
		minhaMenor = pegarMenorCarta(cartas,3);
		int cartaMeio = pegarDoMeio(minhaMaior,minhaMenor);
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
				if(valorCarta(&cartas[cartaMeio])>valorWinnerAtMoment){
					return cartaMeio;
				}
				//descarta a menor
				return minhaMenor;
			}

			if(jogadaDaRodada==3){
				//segundo da dupla a jogar e o parceiro nao esta ganhando
				if(valorCarta(&cartas[minhaMenor])>valorWinnerAtMoment){
					return minhaMenor;
				}
				if(valorCarta(&cartas[cartaMeio])>valorWinnerAtMoment){
					return cartaMeio;
				}
				if(valorCarta(&cartas[minhaMaior])>valorWinnerAtMoment){
					return minhaMaior;
				}
				return minhaMenor;
			}
		}
	}

//rodada do meio
	if(rodada==1){
		minhaMaior = pegarMaiorCarta(cartas,2);
		minhaMenor = pegarMenorCarta(cartas,2);
		//tamo ganhando
		if(placar==1){
			//só pode ser na jogadaDaRodada = 3
			if(valorCarta(&cartas[minhaMenor])>valorWinnerAtMoment){
				return minhaMenor;
			}
			if(valorCarta(&cartas[minhaMaior])>valorWinnerAtMoment){
				return minhaMaior;
			}
			return minhaMenor;
		}
		return minhaMaior;
	}

//ultima rodada
	if(rodada=2){
		minhaMaior = pegarMaiorCarta(cartas,1);
		return minhaMaior;
	}
}
