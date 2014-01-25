CC=gcc
STD=gnu99
WARNFLAGS=-Wall -Wextra -pedantic
DEBUGFLAGS=-g
LIBS=-lm
EXECUTABLE=bot

CFLAGS=-std=${STD} ${WARNFLAGS} ${DEBUGFLAGS} 
LDFLAGS= ${DEBUGFLAGS} ${LIBS}

all: bot.o
	${CC} ${DEBUGFLAGS} -o ${EXECUTABLE} bot.o

bot.o: bot.c bot.h
	${CC} ${CFLAGS} -c bot.c bot.h

clean: 
	rm *.o *.gch
