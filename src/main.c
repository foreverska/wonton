#include <stdio.h>
#include <stdlib.h>

#include "irc.h"

int main(int argv, char **argc)
{
	int result;
	
	port = 6666;
	set_hostname("irc.freenode.net");
	set_nickname("foreverbot");
	
	if(result = connect_server())
	{
		printf("Connect error: %d\nClosing.", result);
		exit(-1);
	}
	
	sleep(1);
	while(listener_alive){}
	
	return 0;
}
