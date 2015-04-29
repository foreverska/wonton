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
#include "privmsg.h"
#include "numeric.h"

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

int substr(char * string, char **substr, char stop_char)
{
	int count = 0, ret;
	
	while(string[count] != stop_char)
	{
		if(string[count] == '\0')
				break;
		count++;
	}
	
	ret = count;	
	*substr = calloc(count+1,1);
	if(!*substr)
		return -1;
	
	(*substr)[count--] = '\0';
	while(count > -1)
	{
		(*substr)[count] = string[count];
		count--;
	}
		
	return ret;
}

int irc_message(char * message)
{
	char *prefix = NULL, *command = NULL, *parameters = NULL, *temp;
	int prefix_len, command_len, parameters_len;
	
	//Strip prefix, command and put the rest in a string
	prefix_len = substr(message, &prefix, ' ');
	temp = message + prefix_len;
	if(temp != '\0')
	{
		temp++;
		command_len = substr(temp, &command, ' ');
	}
	temp =  temp + command_len ;
	if(temp != '\0')
	{
		temp++;
		parameters_len = substr(temp, &parameters, '\r');
	}
	
	if(!prefix)
	{
		printf("Missing prefix in packet.  Skipping message.\n");
		return -1;
	}
	
	if(!command)
	{
		printf("Missing command in packet.  Skipping message\n");
		free(prefix);
		return -1;
	}
	
	if(!strcmp(prefix, "PING"))
	{
		start_pingThread(command, command_len);
	}
	else if(!strcmp(command, "PRIVMSG"))
	{
		start_privmsgThread(parameters, parameters_len, prefix, prefix_len);
	}
	else if(atoi(command))
	{
		start_numericThread(parameters, parameters_len, atoi(command));
	}
	
	free(prefix);
	free(command);
		
	return 0;
}

void buf_append(char *source, char *dest, int offset, int size)
{
	for(int i = offset, c = 0; c<size; i++, c++)
	{
		dest[i] = source[c];
	}
}

void *listener(void * args)
{
	char * packet = malloc(65536);
	char * buffer;
	if(!packet)
	{
		printf("Listener Buffer not allocated.  Exiting.\n");
		return NULL;
	}
	
	listener_alive = 1;
	
	while(listener_alive)
	{
		int buffer_size = 0;
		buffer = NULL;
		int ret, cycles = 0;
		do
		{
			ret = recv(sock, packet, 65535, 0);
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
			buffer = realloc(buffer, ret+buffer_size);
			buf_append(packet, buffer, buffer_size, ret);
			buffer_size += ret;
			cycles++;
		}
		while(buffer[buffer_size-1] != '\n' && buffer[buffer_size-2] != '\r');
		
		char *current = buffer;
		while(*current)
		{
			char *pass;
			
			current += substr(current, &pass, '\r')+2;
			if(!pass)
			{
				printf("Issue with malloc in packet splitter.  Stopping Splitter\n");
				break;
			}
			
			irc_message(pass);
			
			free(pass);
		}
		
		if(buffer)
			free(buffer);
	}

	free(packet);
	
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
