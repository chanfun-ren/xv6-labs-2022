#include "kernel/types.h"
#include "user/user.h"

#define NULL 0

void perror_exit(char* err_msg) {
  fprintf(2, "%s\n", err_msg);
  exit(-1);
}

void child_processing(int left_pipe[2]) {
  // every process do three things：
  // 0. read from left-side pipe, and try to print a prime.
  // 1. do filtering.
  // 2. create a new right-side pipe, do fork, pass the filtered data to right-side pipe.
  // notes: The new child processes forked will recursively do the above tasks.

  close(left_pipe[1]);
  int prime;
  int rbytes = read(left_pipe[0], &prime, sizeof(prime));
  if (rbytes == -1) {
    close(left_pipe[0]);
    perror_exit("read error");
  } else if (rbytes == 0) {
    // No more data reaches here
    close(left_pipe[0]);
    exit(0);
  } else {
    fprintf(1, "prime %d\n", prime);
  }

  int right_pipe[2];
  int ret = pipe(right_pipe);
  if (ret == -1) {
    perror_exit("pipe error");
  }

  ret = fork();
  if (ret == -1) {
    perror_exit("fork error");
  } else if (ret > 0) { // parent/current process
    close(right_pipe[0]);

    // do filtering, write data into the right-side pipe
    int num;
    while ((rbytes = read(left_pipe[0], &num, sizeof(num))) != 0) {
      if (rbytes == -1) {
        perror_exit("read error");
      }
      if (num % prime != 0) {
        write(right_pipe[1], &num, sizeof(num));
      }
    }
    // if rbytes == 0, no more data reaches. the job of this process is done
    close(left_pipe[0]);
    close(right_pipe[1]);
    
    wait(NULL);
    exit(0);
  } else if (ret == 0) { // child process
    child_processing(right_pipe);
  }
} 

int main(int argc, char* argv[])
{
  int pipe_fds[2];
  int ret = pipe(pipe_fds);
  if (ret == -1) {
    perror_exit("pipe error");
  }

  // create child process
  int pid = fork();
  if (pid == -1) {
    perror_exit("fork error");
  } else if (pid == 0) {  // child process
    // read from pipe, do filtering and pass the data to next stage
    child_processing(pipe_fds);
  } else {  // parent process
    close(pipe_fds[0]);
    
    const int MAX = 35;
    for (uint32 i = 2; i <= MAX; ++ i) {
      write(pipe_fds[1], &i, sizeof(i));
    }
    close(pipe_fds[1]);

    wait(NULL);
  }

  exit(0);
}
