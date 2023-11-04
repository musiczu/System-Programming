#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MEM_SIZE 4096

struct shm_st {
  int written;
  char data[BUFSIZ]; // for P1 --> C2
};

static void sig_end() {
  wait(NULL);
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
int running = 1, shmID, child;
  void *sh_mem = NULL;
  char user_input[BUFSIZ] = "";
  struct shm_st *sh_area;

  shmID = shmget((key_t)21930, MEM_SIZE, 0666 | IPC_CREAT);
  sh_mem = shmat(shmID, NULL, 0);
  sh_area = (struct shm_st *)sh_mem;

  char *s = fgets(user_input, BUFSIZ, stdin);
  n = write(2, sh_area->data, strlen(sh_area->data)); // display
  strcpy(sh_area->data, user_input); // copy ค่าจาก user เข้าไปใน data
  if (shmdt(sh_mem) == -1 || shmctl(shmID, IPC_RMID, 0) == -1) {
        fprintf(stderr, "shmdt or shmctl failed \n");
        exit(EXIT_FAILURE);
      }
}