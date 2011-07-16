#ifndef LISP_H
#define LISP_H

/* type of a node */
typedef enum node_type_e
{
  NIL,				/* nil */
  NUM,				/* number */
  STR,				/* string */
  SYM,				/* symbol */
  LIST				/* list */
} node_type;

/* node of the ast */
typedef struct node_s
{
  node_type	type;		/* type of the node */
  void 		*elem;		/* content of the node (depends of the type) */
} node;

typedef struct node_list_s
{
  struct node_list_s	*next;	/* next element of the list */
  struct node_s		*elem;	/* elem of the list */
} node_list;

typedef struct node_num_s
{
  char			*val;	/* value of the number */
} node_num;

typedef struct node_sym_s
{
  char			*name;	/* name of the symbol */
} node_sym;

#define MAX_INPUT_SIZE 	65536	/* maximum size of the input file */

/* file being interpreted */
typedef struct file_s
{
  const char *	path;		/* path to the input file */
  char *	buf;		/* mmap of the input file */
  int 		fd;		/* file descriptor of the input file */
  int 		pos;		/* current offset in the input */
  int 		line;		/* current line in the input */
} file;

extern file *cur_f;

/* these are so commonly accessed that it's a pain to type them each time */
#define F_POS	(cur_f->pos)
#define F_CHAR	(cur_f->buf[cur_f->pos])
#define F_LINE	(cur_f->line)
#define F_PATH	(cur_f->path)

node	*parse_list(void);
int 	skip_blanks(void);
int	eval_list(node *list);

#endif /* LISP_H */
