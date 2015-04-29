include = include/
bin = bin/wonton
obj = bin/irc.o bin/ping.o bin/privmsg.o bin/numeric.o
lib = -lpthread
debug =
options = -Wall -std=c99

debug: debug += -g
debug: all

all: irc ping privmsg numeric
	gcc $(debug) src/main.c $(obj) -I $(include) $(lib) $(options) -o $(bin)
	
irc:
	gcc $(debug) src/irc.c -I $(include) $(lib) -c $(options) -o bin/irc.o
	
ping:
	gcc $(debug) src/ping.c -I $(include) $(lib) -c $(options) -o bin/ping.o
	
privmsg:
	gcc $(debug) src/privmsg.c -I $(include) $(lib) -c $(options) -o bin/privmsg.o
	
numeric:
	gcc $(debug) src/numeric.c -I $(include) $(lib) -c $(options) -o bin/numeric.o
