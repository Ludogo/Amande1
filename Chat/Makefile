all : serveur chat1 chat2
# le compilateur utilise gcc
CC=gcc
# l�emplacement des fichiers d�en-t�te, ici le repertoire courant .
INCLUDE=.

# option de compilation pour le d�veloppement
CFLAGS= -g  -Wall -std=gnu99
LDLIBS=-pthread 
LDFLAGS=-L.

# option de compilation pour la version final avec optimisation
# CFLAGS= -O  -Wall
# appli : server.o chat2.o chat1.o
# 	$(CC)   -o appli server.o chat2.o chat1.o
lib: libutils.a

libutils.a: utils.o
	$(AR) -rc $@ $^

serveur : server.o
	$(CC)   -o serveur server.o

chat1 : chat1.o
	$(CC)  $(LDLIBS)  -o chat1 chat1.o

chat2 : chat2.o
	$(CC)   -o chat2 chat2.o

server.o : server.c utils.h
	$(CC)  -I$(INCLUDE) $(CFLAGS)  -c server.c
chat2.o : chat2.c
	$(CC)  -I$(INCLUDE) $(CFLAGS)  -c chat2.c
chat1.o : chat1.c
		$(CC)  -I$(INCLUDE) $(CFLAGS)  -c chat1.c
# #  l�option -c indique  il s�agit que d�une compilation
clean:
	$(RM) *.o $(TARGETS)
	$(RM) *.fifo $(TARGETS)
	$(RM) server chat1 chat2 $(TARGETS)

mrproper: clean
	$(RM) *.a

.PHONY: clean  mrproper lib all
