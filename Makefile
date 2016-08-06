CC=gcc
STD=gnu99
WARNFLAGS=-Wall -Wextra -pedantic
DEBUGFLAGS=-g
LIBS=-lm
BIN=./bin
INCLUDE=./include
TARGET=bot

CFLAGS=-std=${STD} ${WARNFLAGS} ${DEBUGFLAGS}
LDFLAGS= ${DEBUGFLAGS} ${LIBS}

OBJECTS= utils.o command.o irc.o io.o bot.o

all: dirs ${OBJECTS}
	${CC} ${LDFLAGS} -o ${BIN}/${TARGET} ${OBJECTS}

bot.o: bot.c ${INCLUDE}/bot.h 
	${CC} ${CFLAGS} -c bot.c 

io.o: io.c ${INCLUDE}/io.h
	${CC} ${CFLAGS} -c io.c 
	
irc.o: irc.c ${INCLUDE}/irc.h
	${CC} ${CFLAGS} -c irc.c 
	
command.o: command.c ${INCLUDE}/command.h
	${CC} ${CFLAGS} -c command.c 
	
utils.o: utils.c ${INCLUDE}/utils.h
	${CC} ${CFLAGS} -c utils.c 

.PHONY: clean dirs

clean: 
	rm -vf *.o ${INCLUDE}/*.gch ${BIN}/*

dirs: 
	mkdir -p ${BIN}