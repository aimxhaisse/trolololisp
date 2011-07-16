#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <ctype.h>

#include "lisp.h"

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

static node	*parse_elem(void);
static node	*parse_num(void);
static node	*parse_symbol(void);

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

static void
syntax_error(const char *msg)
{
  fprintf(stderr, "syntax error in file %s at line %d:%d, %s\n",
	  F_PATH, F_LINE + 1, F_POS + 1, msg);
  exit(1);
}

static node *
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

static node *
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

static node *
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
  node_list	*new_elem;
  node_list	*current;

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
	  while (elem)
	    {
	      if (NULL == (new_elem = malloc(sizeof(*new_elem))))
		err(1, "unable to allocate memory for new list");
	      memset(new_elem, 0, sizeof(*new_elem));
	      new_elem->elem = elem;

	      if (NULL == new->elem)
		new->elem = new_elem;
	      else
		{
		  for (current = new->elem;
		       NULL != current->next;
		       current = current->next)
		    ;
		  current->next = new_elem;
		}

	      skip_blanks();
	      elem = parse_elem();
	    }
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
