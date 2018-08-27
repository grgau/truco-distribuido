#ifndef SUBS_H

//Defines necessários para esta biblioteca.
#define SUBS_H SUBS_H

//Estrutura referente ao tipo subs.
typedef struct subs {
	char info[15];
	struct subs *prox;
} subs;

//Header das funções criadas.
subs* newSubs(subs *, char *);
void displayMessage(subs *);
subs* destroyMessage(subs *);
subs *structureMessage(subs *, char [], int);

//Funções criadas para esta biblioteca.
//Função pra adicionar substrings a estrutura message
subs* newSubs(subs *message,char *substring) {
	subs *aux = message, *nova = (subs*) malloc(sizeof(subs));

	strcpy(nova->info,substring);
	nova->prox = NULL;

	if(aux!= NULL) {
		while(aux->prox != NULL)
			aux = aux->prox;

		aux->prox = nova;
	}
	else
		message = nova;

	return message;
}

//Função de apoio pra mostrar mensagem já separada
void displayMessage(subs *message) {
	if(message != NULL) {
		printf("\nMensagem recebida:\n");
		while(message != NULL) {
			printf("%s ",message->info);
			message = message->prox;
		}
		printf("\n");
	}
	else
		printf("Mensagem não existe!\n");
}

//Função q destrói mensagem para zerar message depois de receber/enviar
subs* destroyMessage(subs *message) {
	if(message != NULL) {
		message = destroyMessage(message->prox);
		free(message);
	}
	return NULL;
}

//Função que percorre string recebida preenchendo message
subs* structureMessage(subs *message,char recv_data[],int bytes_recv) {
  int pos = 0, i;
  char substring[15];

  while(pos < bytes_recv) {
    i = 0;

    while(recv_data[pos] != ',' && recv_data[pos] != '\0') {
      substring[i] = recv_data[pos];
      pos++;
      i++;
    }
    pos++;
    substring[i] = '\0';

    message = newSubs(message,substring);
  }

  return message;
}

#endif
