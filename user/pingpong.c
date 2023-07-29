#include "kernel/types.h"
#include "user/user.h"

#define BUFFSIZE 128

void perror_exit(char* err_msg) {
  fprintf(2, "%s\n", err_msg);
  exit(-1);
}

int
main(int argc, char *argv[])
{
  int toson_fd[2];
  int toparent_fd[2];

  int ret1 = pipe(toson_fd);
  int ret2 = pipe(toparent_fd);
  if (ret1 == -1 || ret2 == -1) {
    perror_exit("pipe error");
  }
  
  int pid = fork();
  if (pid == -1) { // 
    perror_exit("fork error");
  } else if (pid == 0) { // child process
    // read from the pipe1
    char buf[BUFFSIZE];
    int rbytes = read(toson_fd[0], buf, sizeof(buf));
    if (rbytes == -1) {
      perror_exit("read error");
    }
    buf[rbytes] = '\0';
    
    // print the msg from parent
    fprintf(1, "%d: received %s\n", getpid(), buf);

    // write response to parent (to pipe2)
    char resp[4] = "pong";
    int ret = write(toparent_fd[1], resp, sizeof(resp));
    if (ret == -1) {
      perror_exit("write error");
    }
  } else { // parent process
    // write to son
    char msg[4] = "ping";
    int ret = write(toson_fd[1], msg, sizeof(msg));
    if (ret == -1) {
      perror_exit("write error");
    }

    // read from son
    char buf[BUFFSIZE];
    int rbytes = read(toparent_fd[0], buf, sizeof(buf));
    if (rbytes == -1) {
      perror_exit("read");
    }
    buf[rbytes] = '\0';

    // print the resp from son
    fprintf(1, "%d: received %s\n", getpid(), buf);
  }

  exit(0);
}