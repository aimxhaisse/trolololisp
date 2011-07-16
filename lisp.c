/*
 * trololisp
 * yet another experimental lisp-like interpreter
 *
 * by s. rannou <mxs@sbrk.org>
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <ctype.h>
#include <sys/mman.h>

#include "lisp.h"

static void	usage(const char *);

file		*cur_f = NULL;	/* current file being evaluated */

static void
exec_file(const char * filepath)
{
  file	*new;
  file	*prev;
  node	*in;
  node	*out;

  if (NULL == (new = (file *) malloc(sizeof(*new))))
    err(1, "unable to allocated memory for file %s", filepath);
  memset(new, 0, sizeof(*new));
  new->path = filepath;
  if ((new->fd = open(new->path, O_RDONLY)) < 0)
    err(1, "unable to open file %s", new->path);
  if (NULL == (new->buf = (char *) mmap(0, MAX_INPUT_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, new->fd, 0)))
    err(1, "unable to mmap file %s", new->path);
  prev = cur_f;
  cur_f = new;

  do {
    skip_blanks();
    in = parse_list();
    out = eval_list(in);
    node_free(in);
    node_free(out);
  } while (in && out && '\0' != F_CHAR);

  cur_f = prev;  
  if (munmap(new->buf, MAX_INPUT_SIZE))
    warn("unable to unmap file %s", new->path);
  if (close(new->fd))
    warn("unable to close file %s", new->path);
  free(new);
}

int
main(int argc, char **argv)
{
    if (argc != 2)
      usage(argv[0]);
    exec_file(argv[1]);

    return 0;
}

static void
usage(const char * name)
{
  fprintf(stderr, "usage: %s input\n", name);
  fflush(stderr);
  exit(-1);
}
