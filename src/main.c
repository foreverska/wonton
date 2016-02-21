#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  //sleep

#include "irc.h"
#include "command.h"

#define VERSION "0.0.1"

void arg_parser(int argc, char **argv)
{
    int c;

    while ((c = getopt (argc, argv, "hvs:n:c:p:")) != -1)
    switch (c)
    {
		case 'v':
			printf("\nWonton Version: %s\n\n", VERSION);
			exit(0);
        case 's':
			free(hostname);
			set_hostname(optarg);
            break;
        case 'n':
            free(nickname);
			set_nickname(optarg);
            break;
        case 'c':
            free(channel);
			set_channel(optarg);
            break;
		case 'p':
			port = atoi(optarg);
			if(port == 0)
			{
				printf("Invalid port setting.  Exiting\n");
				exit(-1);
			}
		case 'h':
			printf("\nWonton IRC bot, Copyright (c) 2015 Adam Parker, MIT Liscense\n"
						"Wonton usage: wonton\n\t"
						"-s [server name]\n\t"
						"-p [port]\n\t"
						"-n [nick name]\n\t"
						"-c [channel]\n\t"
						"-h displays this message\n\t"
						"-v displays version information\n\n");
			exit(0);
        case '?':
            exit(-1);
    }
}

int main(int argc, char **argv)
{
	port = 6667;
	set_hostname("irc.twitch.tv");
	set_nickname("goodsoupbot");
	set_channel("#foreverska");

	fingerprint_commands();
	
	arg_parser(argc, argv);
	
	connect_server();
	
	sleep(1);
	while(listener_alive){sleep(1);}
	
	return 0;
}
