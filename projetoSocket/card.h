typedef struct card {
	int card;
	char suit;
} card;

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

void receiveCards(subs *message,card *cards, int qtdd) {
	int i;
	for (i=0;i<qtdd;i++) {
		cards[i].suit = message->info[0];
		cards[i].card = cardNumber(message->info);
		message = message->prox;
	}
}

void displayCards(card *cards) {
	int i;
	printf("\nCards:\n");
	for(i=0;i<3;i++) {
		printf("  Card: %d\n  Suit: %c\n\n",cards[i].card,cards[i].suit);
	}
}

void displayVira(card *vira) {
	printf("\nVira:\n  Card: %d\n  Suit: %c\n\n",vira[i].card,vira[i].suit);
}