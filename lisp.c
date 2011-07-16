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
#include <sys/mman.h>
#include <ctype.h>

#include "lisp.h"

void	usage(const char *);
int 	skip_blanks(void);

node	*parse_list(void);
node	*parse_elem(void);
node	*parse_num(void);
node	*parse_symbol(void);

/*
 * BNF
 * all members are separated by blanks, with:
 * b := [' ' | '\n' | '\t' | b ]?
 * 
 * list 	:= b ( b elem* b ) b
 * elem		:= num | symbol | list | nil
 * num 		:= 0..9+
 * string	:= "[^"]*"
 * symbol	:= a..zA..Z
 */

file		*cur_f = NULL;	/* current file being evaluated */

void
syntax_error(const char *msg)
{
  fprintf(stderr, "syntax error in file %s at line %d:%d, %s\n",
	  F_PATH, F_LINE + 1, F_POS + 1, msg);
  exit(1);
}

int
skip_blanks(void)
{
  int		result = 0;

  while (F_POS < MAX_INPUT_SIZE && F_CHAR)
    {
      switch (F_CHAR)
	{
	case '\n':
	  F_LINE++;
	case ' ':
	case '\t':
	case '\r':
	  break;
	default:
	  return result;
	}
      F_POS++;
      result++;
    }

  return result;
}

node *
parse_symbol(void)
{
  int		count;
  node		*new;
  node_sym	*sym;

  for (count = 0; strchr("abcdefghijklmnopqrstuvwxyz+", F_CHAR); ++count)
    F_POS++;
  if (count > 0)
    {
      if (NULL == (new = malloc(sizeof(*new))))
	err(1, "can't allocate memory for symbol");
      memset(new, 0, sizeof(*new));
      new->type = SYM;
      if (NULL == (sym = malloc(sizeof(*sym))))
	err(1, "can't allocate memory for symbol");
      memset(sym, 0, sizeof(*sym));
      if (NULL == (sym->name = strndup(&F_CHAR - count, count)))
	err(1, "can't allocate memory for symbol");
      new->elem = sym;
      return new;
    }
  return NULL;
}

node *
parse_num(void)
{
  int		count;
  node		*new;
  node_num	*num;

  for (count = 0; isdigit(F_CHAR); ++count)
    F_POS++;
  if (count > 0)
    {
      if (NULL == (new = malloc(sizeof(*new))))
	err(1, "can't allocate memory for number");
      memset(new, 0, sizeof(*new));
      new->type = NUM;
      if (NULL == (num = malloc(sizeof(*num))))
	err(1, "can't allocate memory for number");
      memset(num, 0, sizeof(*num));
      if (NULL == (num->val = strndup(&F_CHAR - count, count)))
	err(1, "can't allocate memory for number");
      new->elem = num;
      return new;
    }
  return NULL;
}

node *
parse_elem(void)
{
  node	*elem;

  if (NULL != (elem = parse_num()))
    return elem;
  if (NULL != (elem = parse_list()))
    return elem;
  if (NULL != (elem = parse_symbol()))
    return elem;

  return NULL;
}

node *
parse_list(void)
{
  node		*new;
  node		*elem;
  node_list	*list;

  skip_blanks();
  if (F_CHAR == '(')
    {
      F_POS++;
      skip_blanks();
      if (NULL == (new = malloc(sizeof(*new))))
	err(1, "unable to allocate memory for new list");
      memset(new, 0, sizeof(*new));
      elem = parse_elem();
      if (elem)
	{
	  new->type = LIST;
	  do {
	    if (NULL == (list = malloc(sizeof(*list))))
	      err(1, "unable to allocate memory for new list");
	    memset(list, 0, sizeof(*list));
	    list->next = new->elem;
	    list->elem = elem;
	    new->elem = list;
	    skip_blanks();
	  } while (NULL != (elem = parse_elem()));
	}
      else
	new->type = NIL;
      if (F_CHAR != ')')
	syntax_error("no closing ) found");
      F_POS++;
      return new;
    }

  return NULL;
}

void
eval_file(const char * filepath)
{
  file	*new;
  file	*prev;

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
  } while (parse_list());

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
    eval_file(argv[1]);
    return 0;
}

void
usage(const char * name)
{
  fprintf(stderr, "usage: %s input\n", name);
  fflush(stderr);
  exit(-1);
}
