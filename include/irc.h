#ifndef IRC_H
#define IRC_H

char * hostname, * nickname;
int port;
char listener_alive;

void set_hostname(char * host);
void set_nickname(char * nick);

#endif
