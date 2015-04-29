#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "irc.h"
#include "privmsg.h"

void *privmsg(void *args)
{
	char *room = NULL, *msg = NULL, *temp;
	char * parameters = ((char **) args)[0];
	char * prefix = ((char **) args)[1];
	int room_len;
	
	room_len = substr(parameters, &room, ' ');
	temp = parameters + room_len + 2;
	substr(temp, &msg, '\r');
	
	if(!room || !msg)
	{
		printf("Malformed PRIVMSG.  Throwing away.\n");
		
		if(room)
			free(room);
		if(msg)
			free(msg);
		
		return NULL;
	}
	
	if(!strcmp(room, channel))
	{
		char *username;
		
		substr(prefix+1, &username, '!');
		printf("%s: %s\n", username, msg);
		
		free(username);
	}
	
	free(room);
	free(msg);
	
	return NULL;
}

void start_privmsgThread(char * parameters, int parameters_len, char *prefix, int prefix_len)
{
	pthread_t thread;
	
	if(!parameters)
	{
		printf("No parameters in PRIVMSG.  Thowing away.\n");
		return;
	}
	
	char ** pass = malloc(sizeof(char*) * 2);
	pass[0] = malloc(parameters_len+1); 
	pass[1] = malloc(prefix_len+1);
	if(pass && pass[0] && pass[1])
	{
		strcpy(pass[0], parameters);
		strcpy(pass[1], prefix);
	
		pthread_create(&thread, NULL, privmsg, (void *) pass);
	}
	else
	{
		printf("Error in malloc in PRIVMSG code.  PRIVMSG not processed.\n");
	}
}
