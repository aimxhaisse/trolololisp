#include <string.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>

#include "lisp.h"

static void
eval_error(const char *msg)
{
  fprintf(stderr, "eval error: %s\n", msg);
  exit(1);
}

node *
eval_node(node *n)
{
  if (NULL == n)
    return NULL;
  switch (n->type)
    {
    case SYM:
    case NIL:
    case NUM:
      return node_dup(n);

    case LIST:
      return eval_list(n);

    default:
      eval_error("unknown type of node");
    }
  return NULL;
}

node *
eval_list(node *n)
{
  node		*result;
  node_list	*list;
  node_list	*current;
  node_list	*tmp;
  node_list	*interpreted = NULL;
  node_list	*new_interpreted;

  if (NULL == n)
    return NULL;
  if (n->type != LIST)
    eval_error("list expected while evaluating");
  list = n->elem;
  if (NULL == list)
    eval_error("nil list detected");

  for (current = list; NULL != current; current = current->next)
    {
      if (NULL == (new_interpreted = malloc(sizeof(*new_interpreted))))
	err(1, "unable to allocate memory for param");
      memset(new_interpreted, 0, sizeof(*new_interpreted));
      new_interpreted->elem = eval_node(current->elem);
      if (interpreted == NULL)
	interpreted = new_interpreted;
      else
	{
	  for (tmp = interpreted; NULL != tmp->next; tmp = tmp->next)
	    ;
	  tmp->next = new_interpreted;
	}
    }

  if (NULL != interpreted)
    {
      if (interpreted->elem->type == SYM)
	{
	  result = sym_builtin_exec(interpreted->elem->elem, interpreted->next);
	  if (NULL == result)
	    eval_error("undefined reference to a symbol");
	  node_list_free(interpreted);
	  return result;
	}
    }

  return node_new_list(interpreted);
}
