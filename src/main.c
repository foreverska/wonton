#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  //sleep

#include "irc.h"

#define VERSION "0.0.1"

void parameters(int argc, char **argv)
{
	//add nickname required parameter
	for(int i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i],"-h"))
		{
			printf("\nWonton IRC bot, Copyright (c) 2015 Adam Parker, MIT Liscense\n"
						"Wonton usage: wonton "
						"-s [server name] "
						"-p [port] "
						"-n [nick name] "
						"-c [channel]\n\t"
						"-h displays this message\n\t"
						"-v displays version information\n\n");
			exit(0);
		}
		else if(!strcmp(argv[i],"-v"))
		{
			printf("\nWonton Version: %s\n\n", VERSION);
			exit(0);
		}
		else if(!strcmp(argv[i],"-s"))
		{
			if(i+1 <= argc)
			{
				set_hostname(argv[++i]);
			}
			else
			{
				printf("Not enough parameters.  Exiting\n");
				exit(-1);
			}
		}
		else if(!strcmp(argv[i],"-n"))
		{
			if(i+1 <= argc)
			{
				set_nickname(argv[++i]);
			}
			else
			{
				printf("Not enough parameters.  Exiting\n");
				exit(-1);
			}
		}
		else if(!strcmp(argv[i],"-c"))
		{
			if(i+1 <= argc)
			{
				set_channel(argv[++i]);
			}
			else
			{
				printf("Not enough parameters.  Exiting\n");
				exit(-1);
			}
		}
		else if(!strcmp(argv[i],"-p"))
		{
			if(i+1 <= argc)
			{
				port = atoi(argv[++i]);
				if(port == 0)
				{
					printf("Invalid port setting.  Exiting\n");
					exit(-1);
				}
			}
			else
			{
				printf("Not enough parameters.  Exiting\n");
				exit(-1);
			}
		}
	}
}

int main(int argc, char **argv)
{
	port = 6666;
	set_hostname("irc.freenode.net");
	set_nickname("foreverbot");
	set_channel("#forevertest");
	
	parameters(argc, argv);
	
	connect_server();
	
	sleep(1);
	while(listener_alive){}
	
	return 0;
}
