#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h> 

#define SEND_LETTER 1
#define VIEWER_CONNECT 2
#define SEND_MESSAGE 3

typedef unsigned char error_code;

struct send_letter {
  uint32_t type;
  int wealth;
  int nameLen;
  char name[0];
};

struct send_message {
  uint32_t type;
  int messageLen;
  char message[0];
};

#define STATUS_OK 0
#define STATUS_ERROR 1

#define MAX_NAME_LENGTH 250
#define REPLY_SIZE 11