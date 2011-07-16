#include <string.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>

#include "lisp.h"

int
eval_list(node *list)
{
  if (NULL == list)
    return -1;

  printf("eval!\n");

  return 0;
}
