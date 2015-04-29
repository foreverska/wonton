#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "irc.h"
#include "numeric.h"

struct thread_args{
	char *parameters;
	int parameters_len, code;
};

void code_endMOTD()
{
	int packet_len = strlen(channel)+8;
	char * packet = malloc(packet_len);
	
	snprintf(packet, packet_len, "JOIN %s\r\n", channel);
	if(irc_send(packet))
	{
		return;
	}
}

void code_chanRedir(char * parameters)
{
	
}

void *numeric(void * args)
{
	struct thread_args *pass = (struct thread_args *)args;
	
	switch(pass->code)
	{
		case 376:
			code_endMOTD();
		case 470:
			code_chanRedir(pass->parameters);
	}
	
	free(pass->parameters);
	free(pass);

	return NULL;
}

void start_numericThread(char * parameters, int parameters_len, int code)
{
	pthread_t thread;
	
	if(!parameters)
	{
		printf("No parameters in numeric.  Thowing away.\n");
		return;
	}
	
	struct thread_args * pass = malloc(sizeof(struct thread_args));
	pass->parameters = malloc(parameters_len+1); 
	if(pass && pass->parameters)
	{
		strcpy(pass->parameters, parameters);
		pass->parameters_len = parameters_len;
		pass->code = code;
	
		pthread_create(&thread, NULL, numeric, (void *) pass);
	}
	else
	{
		printf("Error in malloc in numeric code.  Numeric not processed.\n");
	}
}
