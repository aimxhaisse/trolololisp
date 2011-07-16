#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>

#include "lisp.h"

static void
node_error(const char *msg)
{
  fprintf(stderr, "node error: %s\n", msg);
  exit(1);
}

void
node_free(node *n)
{
  node_list	*next;
  node_list	*list;

  if (NULL == n)
    return;

  switch (n->type)
    {
    case NIL:
      free(n);
      break;

    case NUM:
      if (NULL != ((node_num *) n->elem)->val)
	free(((node_num *) n->elem)->val);
      free(n);
      break;

    case SYM:
      if (NULL != ((node_sym *) n->elem)->name)
	free(((node_sym *) n->elem)->name);
      free(n);
      break;

    case LIST:
      list = (node_list *) n->elem;
      while (list != NULL)
	{
	  next = list->next;
	  node_free(list->elem);
	  free(list);
	  list = next;
	}
      free(n);
      break;
    }
}

node *
node_dup(node *n)
{
  node		*result;
  node_list	*list;
  node_list	*dup;

  if (NULL == n)
    return NULL;

  if (NULL == (result = malloc(sizeof(*result))))
    err(1, "unable to allocate memory for node");
  memset(result, 0, sizeof(*result));

  switch (n->type)
    {
    case NIL:
      result->type = NIL;
      break;

    case NUM:
      result->type = NUM;
      if (NULL == (result->elem = malloc(sizeof(node_num))))
	err(1, "unable to allocate memory for number");
      ((node_num *) result->elem)->val = strdup(((node_num *) n->elem)->val);
      if (NULL == ((node_num *) result->elem)->val)
	err(1, "unable to allocate memory for number");
      break;

    case SYM:
      result->type = SYM;
      if (NULL == (result->elem = malloc(sizeof(node_sym))))
	err(1, "unable to allocate memory for symbol");
      ((node_sym *) result->elem)->name = strdup(((node_sym *) n->elem)->name);
      if (NULL == ((node_sym *) result->elem)->name)
	err(1, "unable to allocate memory for symbol");
      break;

    case LIST:
      result->type = LIST;
      if (NULL == (result->elem = malloc(sizeof(node_list))))
	err(1, "unable to allocated memory for list");
      for (list = n->elem; NULL != list; list = list->next)
	{
	  if (NULL == (dup = malloc(sizeof(*dup))))
	    err(1, "unable to allocate memory for list");
	  memset(dup, 0, sizeof(*dup));
	  dup->next = result->elem;
	  dup->elem = node_dup(list->elem);
	  result->elem = dup;
	}
      break;

    default:
      node_error("unknown type of node found while duplicating");
    }

  return result;
}

node *
node_new_num(int num)
{
  char		value[12];
  node		*result;
  node_num	*elem;

  if (NULL == (result = malloc(sizeof(*result))))
    err(1, "unable to allocate memory for num");
  memset(result, 0, sizeof(*result));

  result->type = NUM;
  if (NULL == (elem = malloc(sizeof(*elem))))
    err(1, "unable to allocate memory for num");    

  snprintf(value, sizeof(value), "%d", num);
  if (NULL == (elem->val = strdup(value)))
    err(1, "unable to allocate memory for num");
  result->elem = elem;

  return result;
}

node *
node_new_nil(void)
{
  node		*result;

  if (NULL == (result = malloc(sizeof(*result))))
    err(1, "unable to allocate memory for nil");
  memset(result, 0, sizeof(*result));
  result->type = NIL;

  return result;
}

node *
node_new_list(node_list *l)
{
  node		*result;

  if (NULL == (result = malloc(sizeof(*result))))
    err(1, "unable to allocate memory for nil");
  memset(result, 0, sizeof(*result));
  result->type = LIST;
  result->elem = l;

  return result;
}

void
node_list_free(node_list *l)
{
  node_list	*next;
  node_list	*current;

  current = l;
  while (NULL != current)
    {
      next = current->next;
      node_free(current->elem);
      free(current);
      current = next;
    }
}

void
node_dump(node *n)
{
  node_list	*current;

  if (NULL == n)
    return;
  switch (n->type)
    {
    case NIL:
      printf("node nil\n");
      break;
    case NUM:
      printf("node num (%s)\n", ((node_num *) n->elem)->val);
      break;
    case SYM:
      printf("node sym (%s)\n", ((node_sym *) n->elem)->name);
      break;
    case LIST:
      printf("node list\n");
      for (current = n->elem; NULL != current; current = current->next)
	node_dump(current->elem);
      break;
    }
}
