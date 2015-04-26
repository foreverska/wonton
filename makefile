include = include/
bin = bin/wonton
obj = bin/irc.o bin/ping.o
lib = -lpthread
options = -Wall -std=c99

debug: irc ping
	gcc -g src/main.c $(obj) -I $(include) $(lib) $(options) -o $(bin)

all: irc ping
	gcc src/main.c $(obj) -I $(include) $(lib) $(options) -o $(bin)
	
irc:
	gcc -g src/irc.c -I $(include) $(lib) -c $(options) -o bin/irc.o
	
ping:
	gcc -g src/ping.c -I $(include) $(lib) -c $(options) -o bin/ping.o
