// Sender.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

struct a_msg {
  long int msg_type;
  char data[BUFSIZ];
};
int main(void) {
  int running = 1, msgID; struct a_msg a_msg;
  char buffer[BUFSIZ];

  msgID = msgget((key_t)1234, 0666 | IPC_CREAT);
  if (msgID == -1) {
    fprintf(stderr, "msgget failed\n");
    exit(EXIT_FAILURE);
  }
  while (running) {
    printf("Enter data: ");
    fgets(buffer, BUFSIZ, stdin);
    a_msg.msg_type = 1;
    strcpy(a_msg.data, buffer);
    if (msgsnd(msgID, (void *)&a_msg, BUFSIZ, 0) == -1) {
      fprintf(stderr, "msgsnd failed\n");
      exit(EXIT_FAILURE);
    }
    if (strncmp(buffer, "end", 3) == 0)
      running = 0;
  }
  exit(EXIT_SUCCESS);
}