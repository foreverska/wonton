#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#include "command.h"
#include "irc.h"
#include "diceroll.h"
#include "exit.h"

int num_commands = 2;
char *command_list[] = {"dice", "exit"};
uint32_t *command_fingerprints;
void (*command_functions[]) (void *arg) = {diceroll, bot_exit};

uint32_t fingerprint_command(char *command)
{
    uint32_t retval;
    uint8_t *fingerprint = (uint8_t *) &retval;
    int command_len = strlen(command);
    int spacing = command_len /4;

    for(int x=0, y=0; x < command_len; x += spacing, y++)
    {
        fingerprint[y] = command[x];
    }
}

void fingerprint_commands()
{
    command_fingerprints = malloc(num_commands * 4);
    for(int i = 0; i < num_commands; i++)
    {
        command_fingerprints[i] = fingerprint_command(command_list[i]);
    }
}

void command_decode(char *username, char *command)
{
	char *first_word;
    uint32_t fingerprint;

	substr(command, &first_word, ' ');
    printf("%s\n", first_word);
    fingerprint = fingerprint_command(first_word);

	for(int i = 0; i < num_commands; i++)
    {
        if(fingerprint == command_fingerprints[i] && 
           !strcmp(command_list[i], first_word))
        {
            struct arg args = {username, command};
            (*command_functions[i]) (&args);
        }
    }

	free(first_word);
}
