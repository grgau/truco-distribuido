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
