#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc <= 1) {
    fprintf(2, "usage: sleep `time`...\n");
  }
  
  int tick_num = atoi(argv[1]);
  sleep(tick_num);
  
  exit(0);
}