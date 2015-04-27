#ifndef IRC_H
#define IRC_H

char * hostname, * nickname, * channel;
int port;
char listener_alive;

int connect_server();
int irc_send(char * packet);
int substr(char * string, char **substr, char stop_char);

void set_hostname(char * host);
void set_nickname(char * nick);
void set_channel(char * chan);


#endif
