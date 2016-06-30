CC=gcc
STD=gnu99
WARNFLAGS=-Wall -Wextra -pedantic
DEBUGFLAGS=-g
LIBS=-lm
EXECUTABLE=bot

CFLAGS=-std=${STD} ${WARNFLAGS} ${DEBUGFLAGS}
LDFLAGS= ${DEBUGFLAGS} ${LIBS}

all: utils.o command.o irc.o io.o bot.o 
	${CC} ${LDFLAGS} -o ${EXECUTABLE} utils.o command.o irc.o io.o bot.o

bot.o: bot.c bot.h 
	${CC} ${CFLAGS} -c bot.c bot.h 

io.o: io.c io.h
	${CC} ${CFLAGS} -c io.c io.h
	
irc.o: irc.c irc.h
	${CC} ${CFLAGS} -c irc.c irc.h
	
command.o: command.c command.h
	${CC} ${CFLAGS} -c command.c command.h
	
utils.o: utils.c utils.h
	${CC} ${CFLAGS} -c utils.c utils.h
	

clean: 
	rm *.o *.gch
