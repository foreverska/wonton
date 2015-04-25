#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h> //sleep

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include "irc.h"

int sock;
struct in_addr server_addr;
struct sockaddr_in server_sock;
struct hostent *server_info;

pthread_t listener_t;

void set_hostname(char * host)
{
	if(hostname)
		free(hostname);
	hostname = malloc(strlen(host));
	strcpy(hostname, host);
}

void set_nickname(char * nick)
{
	if(nickname)
		free(nickname);
	nickname = malloc(strlen(nick));
	strcpy(nickname, nick);
}

void set_channel(char * chan)
{
	if(channel)
		free(channel);
	channel = malloc(strlen(chan));
	strcpy(channel, chan);
}

void resolve_host()
{
	server_info = gethostbyname(hostname);
	if(server_info == NULL)
	{
		printf("Host could not be resolved.  Check your internet connection and host info then try again\n");
		exit(-1);
	}
	server_addr.s_addr = *((unsigned long *) server_info->h_addr_list[0]);
}

void open_socket()
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		printf("Failed to open socket\n");
		exit(-1);
	}
}

int irc_send(char * packet)
{
	int ret;
	
	ret = send(sock, packet, strlen(packet), 0);
	if(ret == -1)
	{
		printf("send failed: %d", errno);
		return -1;
	}
	
	return 0;
}

int irc_message(char * message)
{
	char *server = NULL, *target = NULL, *code = NULL, *optional = NULL, *msg = NULL;
	
	sscanf(message, "%ms %ms %m[^\n]s", &server, &code, &optional);
	if(!strcmp(server, "PING"))
	{
		char * packet = malloc(1501);
		
		snprintf(packet, 1500, "PONG %s\r\n", code);
		
		irc_send(packet);
		
		free(packet);
	}
	if(!strcmp(code, "PRIVMSG"))
	{
		char *room = NULL, *message = NULL;
		
		sscanf(optional, "%ms :%m[^\n]s", &room, &message);
		
		if(!strcmp(room, channel))
		{
			char *username;
			
			sscanf(server, ":%m[^!]s", &username);
			printf("%s: %s\n", username, message);
			
			free(username);
		}
		
		free(room);
		free(message);
	}
	else if(atoi(code))
	{
		//miscellaneous codes, none of which I'm concerned with yet
	}
	
	if(server)
		free(server);
	if(code)
		free(code);
	if(target)
		free(target);
	if(optional)
		free(optional);
	if(msg)
		free(msg);
		
	return 0;
}

void *listener(void * args)
{
	char * buffer = malloc(1501);
	
	listener_alive = 1;
	
	while(listener_alive)
	{
		int ret = recv(sock, buffer, 1501, 0);
		if(ret == -1)
		{
			printf("Issue with recv: %d.  Closing listener thread.", errno);
			listener_alive = 0;
			break;
		}
		else if(ret == 0)
		{
			printf("Server has performed an orderly shutdown.  Closing listener thread.\n");
			listener_alive = 0;
			break;
		}
		
		irc_message(buffer);
	}

	free(buffer);
	
	return NULL;
}


void irc_ident()
{
	char * packet = malloc(1501);
	
	sprintf(packet, "NICK %s\r\n", nickname);
	if(irc_send(packet))
	{
		return;
	}
	
	sprintf(packet, "USER %s 0 * :%s\r\n", nickname, "pishaw");
	if(irc_send(packet))
	{
		return;
	}
	
	sleep(1);
	
	sprintf(packet, "JOIN %s\r\n", channel);
	if(irc_send(packet))
	{
		return;
	}
}

int connect_server()
{
	open_socket();
	
	memset(&server_addr, 0, sizeof(server_addr));
	server_sock.sin_family = AF_INET;
	server_sock.sin_port = htons(port);
	resolve_host();
	server_sock.sin_addr.s_addr = server_addr.s_addr;
	
	connect(sock, (struct sockaddr *) &server_sock, sizeof(server_sock));
	if(errno)
	{
		return errno;
	}
	
	pthread_create(&listener_t, NULL, listener, (void *) &sock);
	irc_ident();
	
	return 0;
}
