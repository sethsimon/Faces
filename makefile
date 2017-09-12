CC = gcc
CFLAGS = -Wall -Wextra -g -std=gnu99

OBJ = obj/main.o obj/scomplex.o obj/command.o obj/showface.o\
      obj/simplex.o

faces : $(OBJ)
	$(CC) $(CFLAGS) -o faces $(OBJ)

obj/main.o : main.c obj/scomplex.o obj/command.o
	$(CC) $(CFLAGS) -c -o obj/main.o main.c

obj/scomplex.o : scomplex.c scomplex.h obj/simplex.o
	$(CC) $(CFLAGS) -c -o obj/scomplex.o scomplex.c

obj/command.o : command.c command.h obj/showface.o
	$(CC) $(CFLAGS) -c -o obj/command.o command.c

obj/showface.o : showface.h showface.c obj/scomplex.o
	$(CC) $(CFLAGS) -c -o obj/showface.o showface.c

obj/simplex.o : simplex.c simplex.h
	$(CC) $(CFLAGS) -c -o obj/simplex.o simplex.c

runtime : runtime.c
	$(CC) $(CFLAGS) -o runtime runtime.c
