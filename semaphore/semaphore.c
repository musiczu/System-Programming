#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int set_semvalue(int);
static void del_semvalue(void);
static int semaphore_p(int);
static int semaphore_v(int);
static int sem_id;
static int sem1 = 0, sem2 = 1;
static void f1(pid_t);
static void f2(pid_t);

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};

int main(int argc, char *argv[]) {
  sem_id = semget((key_t)100, 2, 0666 | IPC_CREAT);

  if (!set_semvalue(sem1) || !set_semvalue(sem2)) {
    fprintf(stderr, "Failed to initialize semaphore\n");
    exit(EXIT_FAILURE);
  }
  pid_t pid = fork();

  if (pid) {
    f1(pid);
  } else {
    f2(pid);
  }
  if (pid) {
    wait(NULL);
    del_semvalue();
  }
  exit(EXIT_SUCCESS);
}

void f1(pid_t pid) {
  int pause_time;
  semaphore_p(sem1);
  pause_time = rand() % 2;
  sleep(pause_time);
  if (pid)
    printf("Call f2 from f1\n");
  f2(pid);

  if (!semaphore_v(sem1))
    exit(EXIT_FAILURE);

  printf("Done");
}
void f2(pid_t pid) {
  int pause_time;
  semaphore_p(sem2);
  pause_time = rand() % 2;
  sleep(pause_time);
  if (pid == 0)
    printf("Call f1 from f2\n");
  f1(pid);
  if (!semaphore_v(sem2))
    exit(EXIT_FAILURE);

  pause_time = rand() % 2;
  sleep(pause_time);
  printf("Done");
}
static int set_semvalue(int num) {
  union semun sem_union;
  sem_union.val = 1;
  if (semctl(sem_id, num, SETVAL, sem_union) == -1)
    return (0);
  return (1);
}

static void del_semvalue(void) {
  union semun sem_union;

  if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
    fprintf(stderr, "Failed to delete semaphore\n");
}

static int semaphore_p(int num) {
  struct sembuf sem_b;

  sem_b.sem_num = num;
  sem_b.sem_op = -1; /* P() */
  sem_b.sem_flg = SEM_UNDO;
  if (semop(sem_id, &sem_b, 1) == -1) {
    fprintf(stderr, "semaphore_p failed\n");
    return (0);
  }
  return (1);
}

static int semaphore_v(int num) {
  struct sembuf sem_b;

  sem_b.sem_num = num;
  sem_b.sem_op = 1; /* V() */
  sem_b.sem_flg = SEM_UNDO;
  if (semop(sem_id, &sem_b, 1) == -1) {
    fprintf(stderr, "semaphore_v failed\n");
    return (0);
  }
  return (1);
}