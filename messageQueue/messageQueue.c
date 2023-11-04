#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define FILE_LENGTH 2 * (BUFSIZ + 12)

static void sig_end() {
  wait(NULL);
  exit(EXIT_SUCCESS);
}

struct my_msg {
  long int msg_type;
  char data[BUFSIZ];
};

int main(int argc, char *argv[]) {

  int msgID, child, nbytes;
  struct my_msg a_msg;
  char buffer[BUFSIZ] = "";
  int running = 1;

  if (argc < 2) {
    fprintf(stderr, "--------------------------------\n");
    fprintf(stderr, "  [    INVALID ARGUMENT    ]\n");
    fprintf(stderr, "  [  USAGE :   1 , 2   ]\n");
    fprintf(stderr, "--------------------------------\n");
    exit(EXIT_FAILURE);
  }

  else if (argc > 2) {
    fprintf(stderr, "--------------------------------\n");
    fprintf(stderr, "  [    OVER ARGUMENT   ]\n");
    fprintf(stderr, "  [  USAGE :   1 , 2   ]\n");
    fprintf(stderr, "--------------------------------\n");
    exit(EXIT_FAILURE);
  }

  if (strcmp(argv[1], "1") != 0 && strcmp(argv[1], "2") != 0) {
    fprintf(stderr, "--------------------------------\n");
    fprintf(stderr, "  [     INPUT MISMATCH     ]\n");
    fprintf(stderr, "  [  USAGE :   1 , 2   ]\n");
    fprintf(stderr, "--------------------------------\n");
    exit(EXIT_FAILURE);
  }
  signal(SIGUSR1, sig_end);
  msgID = msgget((key_t)6413223, 0666 | IPC_CREAT); // create message queue
  if (msgID == -1) {
    fprintf(stderr, "msgget failed\n");
    exit(EXIT_FAILURE);
  }

  pid_t pid = fork();
  if (pid == 0) { // child receive
    if (strncmp(argv[1], "1", 1) == 0) {
      while (running) {
        if (msgrcv(msgID, (void *)&a_msg, BUFSIZ, 2, 0) == -1) {
          fprintf(stderr, "msgrcv failed\n");
          exit(EXIT_FAILURE);
        }
        printf("\rReceived Message: %s", a_msg.data);
        printf("Enter data: ");
        if (strncmp(a_msg.data, "end chat", 8) == 0)
          running = 0;
      }
    } else if (strncmp(argv[1], "2", 1) == 0) {
      while (running) {
        if (msgrcv(msgID, (void *)&a_msg, BUFSIZ, 1, 0) == -1) {
          fprintf(stderr, "msgrcv failed\n");
          exit(EXIT_FAILURE);
        }
        printf("\rReceived Message: %s", a_msg.data);
        printf("Enter data: ");
        if (strncmp(a_msg.data, "end chat", 8) == 0)
          running = 0;
      }
    }

  } else if (pid == -1) { // fail
  } else { // parent send
    if (strncmp(argv[1], "1", 1) == 0) {
      while (running) {
        printf("\rEnter data: ");
        fgets(buffer, BUFSIZ, stdin);
        a_msg.msg_type = 1;
        strcpy(a_msg.data, buffer);
        if (msgsnd(msgID, (void *)&a_msg, BUFSIZ, 0) == -1) {
          fprintf(stderr, "msgsnd failed\n");
          exit(EXIT_FAILURE);
        }
        if (strncmp(buffer, "end chat", 8) == 0)
          running = 0;
      }
    } else if (strncmp(argv[1], "2", 1) == 0) {
      while (running) {
        printf("\rEnter data: ");
        fgets(buffer, BUFSIZ, stdin);
        a_msg.msg_type = 2;
        strcpy(a_msg.data, buffer);
        if (msgsnd(msgID, (void *)&a_msg, BUFSIZ, 0) == -1) {
          fprintf(stderr, "msgsnd failed\n");
          exit(EXIT_FAILURE);
        }
        if (strncmp(buffer, "end chat", 8) == 0)
          running = 0;
      }
    }
  }

  if (pid > 0) {
    kill(pid, SIGUSR1);
    wait(NULL);
  }

  else if (pid == 0) {
    kill(getppid(), SIGUSR1);
  }
}