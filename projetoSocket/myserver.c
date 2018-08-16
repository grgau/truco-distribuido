#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


char winnercard[5];
int winner, tie=0;

struct players{
  char id[20];
  char IP[12];
  int port;
  int cd1;
  int cd2;
  int cd3; };

struct dequecard {
  char carta[4]; } cartas_jogadas[13];

struct dequecard converte (int sort)
{char card[4];
 struct dequecard resp;

  switch (sort / 10)
  { case 0 : card[0] = 'O'; break;
    case 1 : card[0] = 'E'; break;
    case 2 : card[0] = 'C'; break;
    case 3 : card[0] = 'P'; break;
  }
  switch (sort % 10)
  { case 0 : card[1] = '1'; card[2] = '0'; card[3] = '\0'; break;
    case 1 : card[1] = '1'; card[2] = '\0'; break;
    case 2 : card[1] = '2'; card[2] = '\0'; break;
    case 3 : card[1] = '3'; card[2] = '\0'; break;
    case 4 : card[1] = '4'; card[2] = '\0'; break;
    case 5 : card[1] = '5'; card[2] = '\0'; break;
    case 6 : card[1] = '6'; card[2] = '\0'; break;
    case 7 : card[1] = '7'; card[2] = '\0'; break;
    case 8 : card[1] = '8'; card[2] = '\0'; break;
    case 9 : card[1] = '9'; card[2] = '\0'; break;
  }
  strcpy(resp.carta, card);
  return(resp);
}

int converteback (char *sort)
{int card;

  switch (sort[0])
  { case 'O' : card = 0; break;
    case 'E' : card = 10; break;
    case 'C' : card = 20; break;
    case 'P' : card = 30; break;
  }
  switch (sort[1])
  { case '1' : if (sort[2] != '0')
                   card = card+1;
               break;
    case '2' : card = card + 2; break;
    case '3' : card = card + 3; break;
    case '4' : card = card + 4; break;
    case '5' : card = card + 5; break;
    case '6' : card = card + 6; break;
    case '7' : card = card + 7; break;
    case '8' : card = card + 8; break;
    case '9' : card = card + 9; break;
  }
  return(card);
}

long etime ()
{long ms;
 struct timespec spec;

  clock_gettime(CLOCK_REALTIME, &spec);
  ms = round(spec.tv_nsec / 1.0e3);
  ms = ms%1000;
  return (ms);
}

int winning(char card[5], int who)
{int vira, carta, atual;

   if (cartas_jogadas[12].carta[2] == '\0')
       vira = (int)(cartas_jogadas[12].carta[1] - '0');
     else
       vira = 10;
   if (card[2] == '\0')
       carta = (int)(card[1] - '0');
     else
       carta = 10;
   if (winnercard[2] == '\0')
       atual = (int)(winnercard[1] - '0');
     else
       atual = 10;
   if (carta - vira%10 == 1)
       { if (atual - vira%10 == 1)
             { switch (card[0])
               { case 'O' : break;
                 case 'E' : if (winnercard[0] != 'O')
                                break;
                 case 'C' : if (winnercard[0] == 'P')
                                break;
                 case 'P' : strcpy(winnercard, card);
                            winner = who;
                                break;
               }
             }
           else
             { strcpy(winnercard, card);
               winner = who;
             }
         tie = 0;
       }
     else
       { if ((atual == carta) && (abs(winner - who)%2 == 1))
              tie = 1;
            else
              {
//printf("atual=%d (%s) carta=%d (%s)\n",atual,winnercard,carta,card);
                switch (carta)
                { case 3: if ((atual - vira)%10 != 1)
                              { winner = who;
                                strcpy(winnercard, card);
                                tie = 0;
                              }
                          break;
                  case 2: if (((atual - vira)%10 != 1) && (atual != 3))
                              { winner = who;
                                strcpy(winnercard, card);
                                tie = 0;
                              }
                          break;
                  case 1: if (((atual - vira)%10 != 1) && (atual != 3) && (atual != 2))
                              { winner = who;
                                strcpy(winnercard, card);
                                tie = 0;
                              }
                          break;
                  default: if (((atual - vira)%10 != 1) && ((atual > 3) && (atual < carta)))
                               { winner = who;
                                 strcpy(winnercard, card);
                                 tie = 0;
                               }
                           break;
                }
              }
       }
  return(winner);
}


int main(int argc, char *argv[])
{int sock, connected[4], bytes_recv, true = 1, mao=0, alfa, winnerround;
 int i, j, k, porta=0, digit=0, meudeque[40], jogada[40];
 char send_data[1024] , recv_data[1024], blank[2], text[50], textpos[4][200];
 struct sockaddr_in server_addr,  *client_addr[4];
 int sin_size;
 struct players player[4];
 char qual[5];
 int curcard, m=0, who;
 int par1, par2, pontos;
 int trucoaceito=0, oddies = 0, evens = 0, primeira;

 srand((int)etime());

// Creates a socket and checks if it was successful
 if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
       perror("Socket");
       exit(1);
    }

// Sets options for the socket created before
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &true,sizeof(int)) == -1) {
       perror("Setsockopt");
       exit(1);
    }

// Initializes parameter values for the socket. Notice the port address (5000)
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(14000);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(server_addr.sin_zero),8);

// Binds the socket structure to the actual socket
  if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
       perror("Unable to bind");
       exit(1);
   }

// Defines the input queue size for this socket as 5
  if (listen(sock, 5) == -1) {
       perror("Listen");
       exit(1);
   }

  printf("\nTCPServer Waiting for clients on port 14000\n\n");
  fflush(stdout);
  for(i=0;i<40;i++) { meudeque[i] = 0;     jogada[i] = 0; }

// Runs forever
  for (i=0; i<4; i++)
  { sin_size = sizeof(struct sockaddr_in);
    connected[i] = accept(sock, (struct sockaddr *)&client_addr[i], &sin_size);
    { bytes_recv = recv(connected[i], recv_data, 1024, 0);
      recv_data[bytes_recv] = '\0';
      j = 0; k = j;
      while (recv_data[j] != ',')   { text[k] = recv_data[j]; k++; j++; }
      text[k] = '\0'; j++; k = 0; strcpy(player[i].id, text);
      while (recv_data[j] != ',')   { text[k] = recv_data[j]; k++; j++; }
      text[k] = '\0'; j++; k = 0; strcpy(player[i].IP, text); porta = 0;
      while (recv_data[j] != '\0')
      { digit = (int)(recv_data[j] - '0');
        porta = porta*10 + digit;
        j++;
      }
      player[i].port = porta;
      fflush(stdout);
    }
  }
  strcpy(send_data,"MESA,");
  { for (i=0; i<4; i++)
    {char portatxt[20];
      strcat(send_data,player[i].id);
      strcat(send_data,",");
      strcat(send_data,player[i].IP);
      strcat(send_data,",");
      {int rest, temp, j, k;
       char uka[20];
         j=0; temp=player[i].port;
         while (temp > 0)
         { uka[j] = '0' + temp%10;
           temp = temp/10;
           j++;
         }
         k = 0; j--;
         while (j >= 0)
         { portatxt[k] = uka[j];
           k++; j--;
         }
         portatxt[k] = '\0';
      }
      strcat(send_data,portatxt);
      strcat(send_data,",");
    }
  }
  send_data[strlen(send_data)-1] = '\0';
  for (i=0; i<4; i++)
  { send(connected[i],send_data,strlen(send_data), 0);
    fflush(stdout);
    sleep(1);
  }
  sleep(1);

par1=par2=0;
while (par1<12 && par2<12)
{
puts("\n STARTING NEW ROUND (with new cards) \n\n");
  for (i=0; i<40; i++)
  { meudeque[i] = 0;
    jogada[i] = 0;
  }
//  distribui cartas
  for (i=0; i<4; i++)
  { strcpy(send_data,"CARTA");
    for (j=0;j<3; j++)
    { int result;
      result = rand()%40;
      while (meudeque[result] != 0)
         result = rand()%40;
      meudeque[result]=-1;
      strcat(send_data,",");
      cartas_jogadas[j+i*3] = converte(result);
      switch (j)
      { case 0 : player[i].cd1 = result; break;
        case 1 : player[i].cd2 = result; break;
        case 2 : player[i].cd3 = result; break;
      }
      jogada[result] = 1;
      strcat(send_data,cartas_jogadas[j+i*3].carta);
    }
    send(connected[i],send_data,strlen(send_data), 0);
    fflush(stdout);
  }
  sleep(1);

//  distribui a vira e indica primeiro jogador
  {int result;
    result = rand()%40;
    while (meudeque[result] != 0)
       result = rand()%40;
    meudeque[result]=-1;
    cartas_jogadas[12] = converte(result);
    strcpy(send_data,"VIRA,");
    strcat(send_data,cartas_jogadas[12].carta);
    { winner = mao;
      if ((int)(cartas_jogadas[12].carta[1] - '0') != 3)
          strcpy(winnercard,"O4");
        else
          strcpy(winnercard,"O5");
    }
printf("vira is %s \n",cartas_jogadas[12].carta);
    strcat(send_data,",");
    strcat(send_data,player[mao].id);
    for (i=0; i<4; i++)
       send(connected[i],send_data,strlen(send_data), 0);
    fflush(stdout);
  }

//* recebe e valida jogadas
  alfa=mao;
  for (k=0; k<3; k++)
  {
    who = alfa;
//printf("\nROUND %d -- starting with %s \n",k+1,player[who].id);
    for (int kk=0; kk<4; kk++)
    { bytes_recv = recv(connected[who], recv_data, 1024, 0);
      recv_data[bytes_recv] = '\0';
printf("Round %d: received card %s from player %s \n",k+1,recv_data, player[who].id);
      if (recv_data[0] == 'M' || recv_data[0] == 'E') // não é truco
          { qual[0] = recv_data[3]; qual[1] = recv_data[4];
            if (recv_data[5] != '\0')
                { qual[2] = recv_data[5]; qual[3] = '\0'; }
              else
                qual[2] = '\0';
/// mounted the card
            curcard = converteback(qual);
            if (jogada[curcard] == 1 && (player[who].cd1 == curcard || player[who].cd2 == curcard || player[who].cd3 == curcard))
                { jogada[curcard] = 0;
                  strcpy(send_data,"OK,");
                  if (recv_data[0] == 'M')
                      { strcat(send_data,qual);
                        winnerround = winning(qual, who);
                      }
                    else
                      strcat(send_data,"NULL");
                  for (i=0; i<4; i++)
                      { send(connected[i],send_data,strlen(send_data), 0);
                        fflush(stdout);
                        sleep(1);
                      }
                }
              else // card wasn't validated
                { strcpy(send_data,"Someone is cheating!!");
                  for (i=0; i<4; i++)
                      send(connected[i],send_data,strlen(send_data), 0);
                  exit(0);
                }
          }
        else // é truco ou resposta de truco
          {char answer[1024];
            for (i=0; i<4; i++)
            { if (who != i)
                  { send(connected[i],recv_data,strlen(recv_data), 0);
                    fflush(stdout);
                    sleep(1);
                  }
            }
            bytes_recv = recv(connected[(who+1)%4], answer, 1024, 0);
            answer[bytes_recv] = '\0';
            bytes_recv = recv(connected[(who+3)%4], recv_data, 1024, 0);
            recv_data[bytes_recv] = '\0';
            if ((recv_data[0] == 'F') || (answer[0] == 'F'))
                { k = 3; kk = 4;
                  strcpy(send_data,"FORA,NULL");
                  for (i=0; i<4; i++)
                  { send(connected[i],send_data,strlen(send_data), 0);
                    fflush(stdout);
                    sleep(1);
                  }
                  if (who % 2)
                      oddies = oddies + 3;
                    else
                      evens = evens + 3;
                  winnerround = who;
                }
              else
                { trucoaceito = 1;
                  strcpy(send_data,"DESCE,NULL");
                  send(connected[who],send_data,strlen(send_data), 0);
                  fflush(stdout);
                  sleep(1);
                  strcpy(recv_data,"");
                  bytes_recv = recv(connected[who], recv_data, 1024, 0);
                  recv_data[bytes_recv] = '\0';
                  qual[0] = recv_data[3]; qual[1] = recv_data[4];
                  if (recv_data[5] != '\0')
                      { qual[2] = recv_data[5]; qual[3] = '\0'; }
                    else
                      qual[2] = '\0';
// mounted the card
                  curcard = converteback(qual);
                  if (jogada[curcard] == 1 && (player[who].cd1 == curcard || player[who].cd2 == curcard || player[who].cd3 == curcard))
                      { jogada[curcard] = 0;
                        strcpy(send_data,"OK,");
                        if (recv_data[0] == 'M')
                            strcat(send_data,qual);
                          else
                            strcat(send_data,"NULL");
printf("Round %d: received card %s from player %s \n",k+1,send_data, player[who].id);
                        for (i=0; i<4; i++)
                        { send(connected[i],send_data,strlen(send_data), 0);
                          fflush(stdout);
                          sleep(1);
                        }
                        winnerround = winning(qual, who);
                      }
                    else
                      { strcpy(send_data,"Someone is cheating!!");
                        for (i=0; i<4; i++)
                        send(connected[i],send_data,strlen(send_data), 0);
                        exit(0);
                      }
                } // end of desce's answer
           } // end of truco's else (
      who = (who+1)%4;
    } // end of a round (4 cards) (kk for)
    if (tie == 0)
        { if (winnerround % 2)
              oddies++;
            else
              evens++;
          if (k == 0)
              primeira = evens*2 + oddies;
        }
      else
        { if (k == 1)
              { if (primeira == 1)
                    oddies = oddies + 3;
                  else
                    evens = evens + 3;
              }
            else
              { if (k == 2)
                    if (oddies == evens)
                        if (primeira == 1)
                            oddies = oddies + 3;
                          else
                            evens = evens + 3;
              }
        }

    if (k < 2)
    { alfa = winnerround;
      strcpy(send_data,"MAO,");
      strcat(send_data,player[alfa].id);
      { winner = alfa;
        if ((int)(cartas_jogadas[12].carta[1] - '0') != 3)
            strcpy(winnercard,"O4");
          else
            strcpy(winnercard,"O5");
      }

      for (i=0; i<4; i++)
         send(connected[i],send_data,strlen(send_data), 0);
      fflush(stdout);
      sleep(1);
    }
  } // end of 3 rounds (point will be allocated)

//puts("will count points");
  if (trucoaceito)
      pontos = 3;
    else
      pontos = 1;
  if (oddies < evens)
      par2 = par2 + pontos;
    else
      par1 = par1 + pontos;
  trucoaceito = 0; oddies = 0; evens = 0, primeira = 0;
  printf("Current score is (%s-%s): %d and (%s-%s): %d \n",player[0].id,player[2].id,par2,player[1].id,player[3].id,par1);
// muda para a proxima mao...
  mao = (mao+1)%4;
}
  close(sock);
  return 0;
}
