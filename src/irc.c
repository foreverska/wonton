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
#include "ping.h"

int sock;
struct in_addr server_addr;
struct sockaddr_in server_sock;
struct hostent *server_info;

pthread_t listener_t;

void set_hostname(char * host)
{
	hostname = malloc(strlen(host));
	strcpy(hostname, host);
}

void set_nickname(char * nick)
{
	nickname = malloc(strlen(nick));
	strcpy(nickname, nick);
}

void set_channel(char * chan)
{
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
	char *prefix = NULL, *command = NULL, *parameters = NULL;
	
	sscanf(message, "%ms %ms%m[^\n]s", &prefix, &command, &parameters);
	
	if(!prefix)
	{
		printf("Missing prefix in packet.  Skipping message.\n");
		return -1;
	}
	
	if(!command)
	{
		printf("Missing command in packet.  Skipping message\n");
		printf("%s\n",message);
		free(prefix);
		return -1;
	}
	
	if(!strcmp(prefix, "PING"))
	{
		pthread_t thread;
		
		char * pass = malloc(sizeof(command));  //the thread will free this when it's done with it
		if(pass)
		{
			strcpy(pass, command);
		
			pthread_create(&thread, NULL, ping, (void *) pass);
		}
		else
		{
			printf("Error in malloc in PING code.  PONG not sent.\n");
		}
		
		free(prefix);
		free(command);
		return 0;
	}
	
	if(!strcmp(command, "PRIVMSG"))
	{
		char *room = NULL, *msg = NULL;
		
		sscanf(parameters, "%ms :%m[^\n]s", &room, &msg);
		
		if(!room || !msg)
		{
			printf("Malformed PRIVMSG.  Throwing away.\n");
			
			free(prefix);
			free(command);
			if(room)
				free(room);
			if(msg)
				free(msg);
			
			return -1;
		}
		
		if(!strcmp(room, channel))
		{
			char *username;
			
			sscanf(prefix, ":%m[^!]s", &username);
			printf("%s: %s\n", username, msg);
			
			free(username);
		}
		
		free(room);
		free(msg);
	}
	else if(atoi(command))
	{
		//miscellaneous codes, none of which I'm concerned with yet
		if(atoi(command) == 376)
		{
			char * packet = malloc(1501);
			
			snprintf(packet, 1500, "JOIN %s\r\n", channel);
			if(irc_send(packet))
			{
				return -1;
			}
		}
	}
	
	free(prefix);
	free(command);
		
	return 0;
}

void *listener(void * args)
{
	int n;
	unsigned int m = sizeof(n);
	getsockopt(sock,SOL_SOCKET,SO_RCVBUF,(void *)&n, &m);
	
	char * buffer = malloc(n+1);
	if(!buffer)
	{
		printf("Listener Buffer (%d bytes) not allocated.  Exiting.\n", n);
		return NULL;
	}
	
	listener_alive = 1;
	
	while(listener_alive)
	{
		int ret = recv(sock, buffer, n+1, 0);
		if(ret == -1)
		{
			printf("Issue with recv: %d.  Closing listener thread.\n", errno);
			listener_alive = 0;
			break;
		}
		else if(ret == 0)
		{
			printf("Server has performed an orderly shutdown.  Closing listener thread.\n");
			listener_alive = 0;
			break;
		}
		
		buffer[ret] = '\0';
		char *current = buffer;
		while(*current)
		{
			char *pass;
			
			sscanf(current,"%m[^\r]s", &pass);
			if(!pass)
				break;
				
			current += strlen(pass)+2;
			
			irc_message(pass);
			
			free(pass);
		}
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
