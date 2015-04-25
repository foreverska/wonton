include = include/
bin = bin/wonton
obj = bin/irc.o
lib = -lpthread

debug: irc
	gcc -g src/main.c $(obj) -I $(include) $(lib) -o $(bin)

all: irc
	gcc src/main.c $(obj) -I $(include) $(lib) -o $(bin)
	
irc:
	gcc -g src/irc.c -I $(include) $(lib) -c -o bin/irc.o
