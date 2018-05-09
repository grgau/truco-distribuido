typedef struct player {
    char ID[10];
    char ip[15];
    char port[10];
} player;

//Função que recebe mensagem separada e armazena players
void receivePlayers(subs *message, player *players) {
	int i;
	for(i=0;i<4;i++) {
		strcpy(players[i].ID,	message->info);
		strcpy(players[i].ip,	(message->prox)->info);
		strcpy(players[i].port,((message->prox)->prox)->info);

		message = ((message->prox)->prox)->prox;
	}
} 

//Função de apoio pra testar recebimento dos players
void displayPlayers(player *players) {
	int i;
	printf("\nPlayers:\n");
	for(i=0;i<4;i++) {
		printf("  Player %s\n    ip: %s\n    port: %s\n\n",players[i].ID,players[i].ip,players[i].port);
	}
}