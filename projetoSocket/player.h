#ifndef PLAYER_H

//Defines necessários para esta biblioteca.
#define PLAYER_H PLAYER_H

//Estruturas referentes ao tipo player.
typedef struct player {
    char ID[10];
    char ip[16];
    char port[10];
} player;

//Header das funções criadas.
void receivePlayers(subs *, player *);
void displayPlayers(player *);
player whoIsMyPartner(player *, player);

//Funções criadas para esta biblioteca.
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
	printf("\n\nPlayers:\n");
	for(i=0;i<4;i++) {
		printf("  Player %s\n    ip: %s\n    port: %s\n\n",players[i].ID,players[i].ip,players[i].port);
	}
}

player whoIsMyPartner (player *players, player myPlayer) {
  int i;

  for (i=0; i<4; i++) {
    if (strcmp(myPlayer.ID,players[i].ID) == 0) { // Achei quem eu sou
      switch (i) {  // Verifica pela posição no vetor de players quem é meu parceiro
        case 0: return players[2]; break;
        case 1: return players[3]; break;
        case 2: return players[0]; break;
        case 3: return players[1]; break;
      }
    }
  }
}

#endif
