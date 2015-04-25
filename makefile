include = include/
bin = bin/wonton
obj = bin/irc.o
lib = -lpthread
options = -Wall -std=c99

debug: irc
	gcc -g src/main.c $(obj) -I $(include) $(lib) $(options) -o $(bin)

all: irc
	gcc src/main.c $(obj) -I $(include) $(lib) $(options) -o $(bin)
	
irc:
	gcc -g src/irc.c -I $(include) $(lib) -c $(options) -o bin/irc.o
