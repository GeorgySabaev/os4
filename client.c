#include "protocol.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// люблю вкус C по утрам
void DieWithError(char *errorMessage);

char buffer_for_everything[12931];

int main(int argc, char *argv[]) {
  if (argc != 5) {
    fprintf(stderr,
            "Usage: %s <Server IP> <Server Port> <Name> <Wealth>\n",
            argv[0]);
    exit(1);
  }
  int sock;                               // socket descriptor
  char *servIP = argv[1];                 // server ip (as string)
  unsigned short servPort = atoi(argv[2]);// server port
  char *name = argv[3];                   // name of the letter sender
  int wealth = atoi(argv[4]);             // wealth of the letter sender

  // creating a socket
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    DieWithError("socket() failed");

  // building the address structure
  struct sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = inet_addr(servIP);
  servAddr.sin_port = htons(servPort);

  int nameLen = strlen(name) + 1;

  int message_len = sizeof(struct send_letter) + nameLen;

  struct send_letter *message_struct = (struct send_letter *) (buffer_for_everything);
  message_struct->type = SEND_LETTER;
  message_struct->wealth = wealth;
  message_struct->nameLen = nameLen;
  memcpy(message_struct->name, name, nameLen);

  int sendLen = sendto(sock, buffer_for_everything, message_len, 0, (struct sockaddr *) &servAddr, sizeof(servAddr));
  if (sendLen != message_len)
    DieWithError("send() sent a different number of bytes than expected");

  printf("\nName: %s\nWealth: %d\n\n", name, wealth);

  printf("Letter sent!\n\nWaiting for reply...\n");

  struct sockaddr_in sockAddr; /* Source address of echo */
  int sockAddrLen = sizeof(sockAddr);
  int type;
  error_code code;
  int recv_size = recvfrom(sock, buffer_for_everything, sizeof(buffer_for_everything), 0, (struct sockaddr *) &sockAddr, &sockAddrLen);
  printf("%s\n", buffer_for_everything);
  close(sock);
  return 0;
}