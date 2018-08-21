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

int pegarMaiorCarta(card *cards){
	card *cartas = cards;
	int maiorValor = 0, indice=0,valor=0;
	for (int i=0;i<3;i++){
		valor = valorCarta(&cartas[i]);
		if(valor>maiorValor){
			maiorValor = valor;
			indice=i;
		}
	}
	return indice;

}
