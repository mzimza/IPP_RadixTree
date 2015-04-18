# Author: Maja Zalewska nr336088
CC = gcc
CFLAGS = -Wall

NAME = dictionary
NAMED = dictionary.dbg
OBJECTS = trie.o parse.o dictionary.o vector.o

.PHONY: all clean debug

all: dictionary

trie.o: trie.h trie.c
	$(CC) $(CFLAGS) -c trie.c

parse.o: parse.h parse.c
	$(CC) $(CFLAGS) -c parse.c

dictionary.o: dictionary.c
	$(CC) $(CFLAGS) -c dictionary.c

vector.o: vector.h vector.c
	$(CC) $(CFLAGS) -c vector.c

dictionary: $(OBJECTS) 
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS)
	
#tworzy też plik dictionary.dbg
debug: clean
debug: CFLAGS += -g
debug: NAME = $(NAMED)
debug: all

clean:
ifneq ("$(wildcard $(NAME))","")
	rm -f dictionary
endif
ifneq ("$(wildcard $(NAMED))","")
	rm -f dictionary.dbg
endif
	rm -f $(OBJECTS)