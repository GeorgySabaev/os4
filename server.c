#include "protocol.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXPENDING 5

char string_buffer[2048];
char buffer_for_everything[12931];

void DieWithError(char *errorMessage);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage:  %s <Server Port> <N>\n", argv[0]);
    exit(1);
  }
  unsigned short servPort = atoi(argv[1]);// server port
  int N = atoi(argv[2]);                  // number of awaited letters

  int clientCount = 0;
  struct sockaddr_in *clientAddresses = (struct sockaddr_in *) malloc(N * sizeof(struct sockaddr_in));
  struct send_letter **clientLetters = (struct send_letter **) malloc(N * sizeof(struct send_letter **));

  // creating a socket
  int servSock;
  if ((servSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    DieWithError("socket() failed");

  // building the address structure
  struct sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(servPort);

  // binding the socket
  if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
    DieWithError("bind() failed");
  printf("\nServer IP address = %s. Waiting for letters...\n\n", inet_ntoa(servAddr.sin_addr));

  while (clientCount < N) {

    struct sockaddr_in sockAddr; /* Source address of echo */
    int sockAddrLen = sizeof(sockAddr);
    int type;
    error_code code;
    int recv_size = recvfrom(servSock, buffer_for_everything, sizeof(buffer_for_everything), 0, (struct sockaddr *) &sockAddr, &sockAddrLen);
    uint32_t commandType = *((uint32_t *) buffer_for_everything);
    printf("Got data: %d\n", recv_size);
    switch (commandType) {
      case SEND_LETTER:

        clientLetters[clientCount] = (struct send_letter *) malloc(recv_size);

        memcpy(clientLetters[clientCount], (struct send_letter *) buffer_for_everything, recv_size);

        clientAddresses[clientCount] = sockAddr;

        printf("-----begin-connect-----\nReceiving letter %d/%d: %s\nName:", clientCount + 1, N, inet_ntoa(sockAddr.sin_addr));

        printf("\nWealth: %d\n------end-connect------\n\n", clientLetters[clientCount]->wealth);

        ++clientCount;
        break;
      default:
        break;
    }
  }
  // максимально приближенный к реальности алгоритм выбора
  int wealthiest = 0;
  for (int i = 1; i < N; ++i) {
    if (clientLetters[wealthiest]->wealth < clientLetters[i]->wealth) {
      wealthiest = i;
    }
  }
  printf("Partner (%.250s) chosen successfully.\n\n", clientLetters[wealthiest]->name);
  printf("Sending replies...\n");
  for (int i = 0; i < N; ++i) {
    char *reply;
    if (i == wealthiest) {
      reply = "OF COURSE!";
    } else {
      reply = "NOT TODAY.";
    }
    if (sendto(servSock, reply, REPLY_SIZE, 0, (struct sockaddr *) &clientAddresses[i], sizeof(struct sockaddr_in)) != REPLY_SIZE)
      DieWithError("send() failed");
    printf("\"%s\" sent to %.250s (%d/%d).\n", reply, clientLetters[i]->name, i + 1, N);
  }

  // cleanup
  for (int i = 0; i < N; ++i) {
    free(clientLetters[i]);
  }
  close(servSock);
  free(clientAddresses);
  free(clientLetters);
}