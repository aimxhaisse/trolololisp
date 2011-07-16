SRCS 	= lisp.c
OBJS 	= $(SRCS:.c=.o)
CFLAGS	= -W -Wall -pedantic -ansi -I. -D_GNU_SOURCE
TARGET	= lisp

all:	$(OBJS)
	gcc $(OBJS) -o $(TARGET)

clean:
	rm -f $(OBJS)

fclean:	clean
	rm -f $(TARGET)

re:	fclean all
