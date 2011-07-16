#include <string.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "lisp.h"

static node	*sym_builtin_add(node_list *params);
static node	*sym_builtin_print(node_list *params);

sym_builtins builtins[] = {
  { "+",	&sym_builtin_add },
  { "add",	&sym_builtin_add },
  { "puts",	&sym_builtin_print },
  { NULL,	NULL}
};

static void
builtin_error(const char *msg)
{
  fprintf(stderr, "builtin error: %s\n", msg);
  exit(1);
}

static node *
sym_builtin_add(node_list *params)
{
  node_list	*current;
  int		result = 0;

  for (current = params; NULL != current; current = current->next)
    {
      if (current->elem->type != NUM && current->elem->type != NIL)
	builtin_error("bad parameter for builtin addition");
      if (current->elem->type == NUM)
	result += atoi(((node_num *) current->elem->elem)->val);
    }

  return node_new_num(result);
}


static node *
sym_builtin_print(node_list *params)
{
  node_list	*current;

  for (current = params; NULL != current; current = current->next)
    {
      switch (current->elem->type)
	{
	case NIL:
	  printf("nil\n");
	  break;
	case NUM:
	  printf("%s\n", ((node_num *) current->elem->elem)->val);
	  break;
	case SYM:
	  printf("%s\n", ((node_sym *) current->elem->elem)->name);
	  break;
	case LIST:
	  printf("list\n");
	  break;
	}
    }

  return node_new_nil();
}

node *
sym_builtin_exec(node_sym *sym, node_list *params)
{
  int		i;

  for (i = 0; NULL != builtins[i].name; ++i)    
    if (0 == strcmp(sym->name, builtins[i].name))
      return builtins[i].builtin(params);

  return NULL;
}
