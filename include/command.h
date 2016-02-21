#ifndef __COMMAND_H__
#define __COMMAND_H__

void command_decode(char *username, char *command);
void fingerprint_commands();

struct arg
{
    char *username, *command;
};

#endif
