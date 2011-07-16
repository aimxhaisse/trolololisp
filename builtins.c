#include <string.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "lisp.h"

static node	*sym_builtin_add(node_list *params);
static node	*sym_builtin_sub(node_list *params);
static node	*sym_builtin_div(node_list *params);
static node	*sym_builtin_mod(node_list *params);
static node	*sym_builtin_mul(node_list *params);
static node	*sym_builtin_print(node_list *params);
static node	*sym_builtin_list(node_list *params);
static node	*sym_builtin_help(node_list *params);

sym_builtins builtins[] = {
  { "+",	&sym_builtin_add,	"performs an addition with all parameters" },
  { "add",	&sym_builtin_add,	"performs an addition with all parameters" },
  { "-",	&sym_builtin_sub,	"performs a substraction with all parameters" },
  { "sub",	&sym_builtin_sub,	"performs a substraction with all parameters" },
  { "/",	&sym_builtin_div,	"performs a division with all parameters" },
  { "div",	&sym_builtin_div,	"performs a division with all parameters" },
  { "%",	&sym_builtin_mod,	"performs a modulo with all parameters" },
  { "mod",	&sym_builtin_mod,	"performs a modulo with all parameters" },
  { "*",	&sym_builtin_mul,	"performs a multiplication with all parameters" },
  { "mul",	&sym_builtin_mul,	"performs a multiplication with all parameters" },
  { "puts",	&sym_builtin_print,	"prints each parameter on a new line" },
  { "list",	&sym_builtin_list,	"returns a list containing all parameters"},
  { "help",	&sym_builtin_help,	"displays help about a builtin"},
  { NULL, NULL, NULL}
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
sym_builtin_mul(node_list *params)
{
  node_list	*current;
  int		result = 1;

  for (current = params; NULL != current; current = current->next)
    {
      if (current->elem->type != NUM && current->elem->type != NIL)
	builtin_error("bad parameter for builtin multiplication");
      if (current->elem->type == NUM)
	result *= atoi(((node_num *) current->elem->elem)->val);
    }

  return node_new_num(result);
}

static node *
sym_builtin_div(node_list *params)
{
  node_list	*current;
  int		init = 0;
  int		result = 0;
  int		num;

  for (current = params; NULL != current; current = current->next)
    {
      if (current->elem->type != NUM && current->elem->type != NIL)
	builtin_error("bad parameter for builtin division");
      if (current->elem->type == NUM)
	{
	  if (init == 0)
	    {
	      init = 1;
	      result = atoi(((node_num *) current->elem->elem)->val);
	    }
	  else
	    {
	      num = atoi(((node_num *) current->elem->elem)->val);
	      if (num == 0)
		builtin_error("division by 0");
	      result /= num;
	      init = 2;
	    }
	}
      else
	builtin_error("division by 0");
    }
  if (2 != init)
    builtin_error("builtin division expects at least two parameters");

  return node_new_num(result);
}

static node *
sym_builtin_mod(node_list *params)
{
  node_list	*current;
  int		init = 0;
  int		result = 0;
  int		num;

  for (current = params; NULL != current; current = current->next)
    {
      if (current->elem->type != NUM && current->elem->type != NIL)
	builtin_error("bad parameter for builtin modulo");
      if (current->elem->type == NUM)
	{
	  if (init == 0)
	    {
	      init = 1;
	      result = atoi(((node_num *) current->elem->elem)->val);
	    }
	  else
	    {
	      num = atoi(((node_num *) current->elem->elem)->val);
	      if (num == 0)
		builtin_error("modulo by 0");
	      result /= num;
	      init = 2;
	    }
	}
      else
	builtin_error("modulo by 0");
    }
  if (2 != init)
    builtin_error("builtin modulo expects at least two parameters");

  return node_new_num(result);
}

static node *
sym_builtin_sub(node_list *params)
{
  node_list	*current;
  int		result = 0;

  for (current = params; NULL != current; current = current->next)
    {
      if (current->elem->type != NUM && current->elem->type != NIL)
	builtin_error("bad parameter for builtin substraction");
      if (current->elem->type == NUM)
	result -= atoi(((node_num *) current->elem->elem)->val);
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

static node *
sym_builtin_list(node_list *params)
{
  node		*result;
  node_list	*current;
  node_list	*tmp;
  node_list	*new;

  if (NULL == (result = malloc(sizeof(*result))))
    err(1, "unable to allocate memory for list");
  memset(result, 0, sizeof(*result));
  result->type = LIST;
  for (current = params; NULL != current; current = current->next)
    {
      if (NULL == (new = malloc(sizeof(*new))))
	err(1, "unable to allocate memory for list");
      memset(new, 0, sizeof(*new));
      new->elem = node_dup(current->elem);
      if (NULL == result->elem)
	result->elem = new;
      else
	{
	  for (tmp = result->elem; NULL != tmp->next; tmp = tmp->next)
	    ;
	  tmp->next = new;
	}
    }

  return result;
}

static node
*sym_builtin_help(node_list *params)
{
  node_sym	*sym;
  int		i;
  int		found;

  if (NULL == params->elem || params->elem->type != SYM)
    builtin_error("bad parameter for builtin help");    
  sym = params->elem->elem;
  for (i = 0, found = 0; NULL != builtins[i].name; ++i)
    {
      if (0 == strcmp(builtins[i].name, sym->name))
	{
	  printf("%s\n", builtins[i].help);
	  found++;
	  break;
	}
    }
  if (!found)
    printf("no help found for this builtin\n");

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
