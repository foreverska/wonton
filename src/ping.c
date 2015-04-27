#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "irc.h"
#include "ping.h"

//This function is not complete.  Technically the server can send PING [SERVER] [SERVER]
//and the bot needs to respond to the second server.  I don't have a great way of testing
//this because it's not a common request.  The following code works in most cases though.
void *ping(void *code)
{
	char * packet = malloc(1501);
		
	if(packet && code)
	{
		snprintf(packet, 1500, "PONG %s\r\n", (char *) code);
	
		irc_send(packet);
	}
	else
	{
		printf("Error with variables in ping thread.  PONG not sent.\n");
	}
	
	if(packet)
		free(packet);
	if(code)
		free(code);
	
	return NULL;
}

void start_pingThread(char * command, int command_len)
{
	pthread_t thread;
	
	char * pass = malloc(command_len+1);  //the thread will free this when it's done with it
	if(pass)
	{
		strcpy(pass, command);
	
		pthread_create(&thread, NULL, ping, (void *) pass);
	}
	else
	{
		printf("Error in malloc in PING code.  PONG not sent.\n");
	}
}
