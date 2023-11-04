#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define FIFO_NAME "my_fifo"
#define FIFO_NAME2 "my_fifo2"

int fd1;
int fd2;
int fifo1, fifo2;
int checkinput;

int main(int argc, char *argv[]) {

  /* 1) pid_t fork()
  ------------------------
  - สำหรับสร้าง process ใหม่ เรียกว่า child process โดยทำงานแบบคู่ขนานกับ parent process
  - ในโปรแกรมนี้ กำหนดให้
    (1.1) parent process รับ input จาก standard input ไป write ลงไฟล์ "my_fifo"
  (user 1) และ "my_fifo2" (จาก user 2) (1.2) child process read ไฟล์จากข้อ (1)
  แล้วแสดงผลผ่าน standard output
  - ฟังก์ชั่นนี้ไม่มี arguement แต่หาก fork สำเร็จ จะ return ค่า id ของ process โดย
     - return Child's PID หากเป็น parent
     - return 0 หากเป็น child
  ซึ่งเราสามารถนำค่าที่ return ได้ไปเข้าเงื่อนไข เพื่อให้ process
  แต่ละอันทำงานอย่างที่ต้องการตามข้อ (1) และ (2)
  -------------------------

  */

  /* 2) int open(const char *pathname, int flags)
  ------------------------
  - ก่อนที่เราจะ read/write ไฟล์ได้นั้น เราต้อง call open() ก่อน โดยมี arguement ดังนี้
      - *pathname : ชื่อไฟล์ที่เราสนใจ (ระบุ directory ที่ถูกต้อง)
      - flags : mode ของการเปิดไฟล์ ในโปรแกรมนี่เราใช้ "O_WRONLY" (อนุญาตให้ write
  ซึ่งใช้สำหรับ (1.1)) และ "O_RDONLY" (อนุญาตให้ write ซึ่งใช้สำหรับ (1.2))
  - โดยหาก open สำเร็จ, ฟังก์ชั่น จะ return file descriptor (fd) ของไฟล์นั้น
  โดยเราสามารถนำค่าดังกล่าวไปเป็น arguement ของฟังก์ชั่น read() และ write() เพื่อให้ไฟล์
  "my_fifo" และ "my_fifo2" เหมาะกับการทำงานมากขึ้น

  -------------------------

  */

  /* 3) int strncmp ( const char * str1, const char * str2, size_t num );
  ------------------------
  - ใช้เปรียบเทียบ string (ในภาษา C) โดยในโปรแกรมนี้เราใช้เพื่อเปรียบเทียบ input จาก user
  กับ "end chat" (คำสั่งในการออกจากโปรแกรม)
  - มี arguement คือ
        - *str1 และ *str2 เป็น string 2 ตัวที่เราต้องการเปรียบเทียบ
        - num: จำนวนตัวอักษรที่ให้เปรียบเทียบ
  - โดยฟังก์ชั่นจะ return 0 หาก input ตรงกับคำว่า "end chat" จริงๆ โดยเราสามารถนำค่าที่
  return ได้ไปเป็นเงื่อนไขใน while loop ต่อไป

  -------------------------

  */
  /* 4) int mkfifo(const char *pathname, mode_t mode);
  ------------------------
  - ใข้สร้างไฟล์ชนิดพิเศษ FIFO (คล้าย file ทั่วไปแต่เหมาะสำหรับการ read/write file ระหว่าง
  processes)
  - โดยมี arguement คือ
      - *pathname: ชื่อไฟล์ที่ต้องการให้เป็น FIFO
      - mode: สำหรับกำหนด permission ของ file
  - โดยหากสร้างสำเร็จ จะ return 0 และ return -1 หาก fail
  -------------------------

  */

  if (access(FIFO_NAME, F_OK) == -1) {
    fifo1 = mkfifo(FIFO_NAME, 0777);
    if (fifo1) {
      fprintf(stderr, "Could not create fo %s\n", FIFO_NAME);
      exit(EXIT_FAILURE);
    }
  }
  if (access(FIFO_NAME2, F_OK) == -1) {
    fifo2 = mkfifo(FIFO_NAME2, 0777);
    if (fifo2) {
      fprintf(stderr, "Could not create fo %s\n", FIFO_NAME2);
      exit(EXIT_FAILURE);
    }
  }

  pid_t pid = fork();
  if (pid == -1) {
    printf("failed");
    exit(EXIT_FAILURE);
  }

  char buf1[512] ;
  char buf2[512] ;

  // send
  if (pid > 0) { /* Parent */
    if (strcmp(*(argv + 1), "1") == 0) {
      write(1, "\ruser 1 : ", 10);
      fd1 = open(FIFO_NAME, O_WRONLY);
      while (strncmp(buf1, "end chat", 8) != 0) {
        write(1, "\ruser 1 : ", 10);
        int nread = read(0, buf1, sizeof(buf1));
        checkinput = 1;
        if ((strncmp(buf1, "end chat", 8) == 0)) {
          write(fd1, buf1, nread);
          break;
        }
        if (nread == 1)
          continue;
        write(fd1, buf1, nread);
      }

    } else if (strcmp(*(argv + 1), "2") == 0) {
      write(1, "\ruser 2 : ", 10);
      fd2 = open(FIFO_NAME2, O_WRONLY);
      while (strncmp(buf2, "end chat", 8) != 0) {
        write(1, "\ruser 2 : ", 10);
        int nread = read(0, buf2, sizeof(buf2));
        checkinput = 2;
        if ((strncmp(buf2, "end chat", 8) == 0)) {
          write(fd2, buf2, nread);
          break;
        }
        if (nread == 1)
          continue;
        write(fd2, buf2, nread);
      }
    }
    // receive
  } else { /* child */

    if (strcmp(*(argv + 1), "1") == 0) {
      fd2 = open(FIFO_NAME2, O_RDONLY);
      while (strncmp(buf2, "end chat", 8) != 0) {

        int numread = read(fd2, buf2, sizeof(buf2));

        if (strncmp(buf2, "end chat", 8) == 0 || (access(FIFO_NAME, F_OK) != 0))
          break;
        write(1, "\ruser 2 : ", 10);
        write(1, buf2, numread);

        write(1, "\ruser 1 : ", 10);
      }

    } else if (strcmp(*(argv + 1), "2") == 0) {
      fd1 = open(FIFO_NAME, O_RDONLY);
      while (strncmp(buf1, "end chat", 8) != 0) {

        int numread = read(fd1, buf1, sizeof(buf1));

        if ((strncmp(buf1, "end chat", 8) == 0) ||
            (access(FIFO_NAME2, F_OK) != 0))
          break;
        write(1, "\ruser 1 : ", 10);
        write(1, buf1, numread);
        write(1, "\ruser 2 : ", 10);
      }
    }
  }

  if (fd1 != -1)
    (void)close(fd1);
  if (fd2 != -1)
    (void)close(fd2);

  if (strcmp(*(argv + 1), "2") == 0) {
    unlink(FIFO_NAME);
  }
  if (strcmp(*(argv + 1), "1") == 0) {
    unlink(FIFO_NAME2);
  }

  exit(EXIT_SUCCESS);

  return 0;
}