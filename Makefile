CC=gcc
STD=gnu99
WARNFLAGS=-Wall -Wextra -pedantic
DEBUGFLAGS=-g
LIBS=-lm
EXECUTABLE=bot

CFLAGS=-std=${STD} ${WARNFLAGS} ${DEBUGFLAGS}
LDFLAGS= ${DEBUGFLAGS} ${LIBS}

all: utils.o bot.o 
	${CC} ${LDFLAGS} -o ${EXECUTABLE} utils.o bot.o

bot.o: bot.c bot.h 
	${CC} ${CFLAGS} -c bot.c bot.h 

utils.o: utils.c utils.h
	${CC} ${CFLAGS} -c utils.c utils.h

clean: 
	rm *.o *.gch
