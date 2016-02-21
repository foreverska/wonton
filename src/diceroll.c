#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <time.h>

#include "command.h"
#include "irc.h"
#include "diceroll.h"

void snarky_answer(char *username)
{
    char *message;
    int msglen = strlen(channel) + strlen(username) + 78;
    message= malloc(msglen);

    sprintf(message, "PRIVMSG %s :%s. Look, if you're not going to behave I'm not going to participate\n", channel, username);
    irc_send(message);
    free(message);
}

void diceroll(void *args)
{
    char *message;
    struct arg *input_args = (struct arg *) args;
    int sides = 0;
    if(input_args->command[4] == '\0')
    {
        sides = 6;
    }
    else
    {
        sscanf(&input_args->command[5], "%d", &sides);
        if(sides < 3)
        {
            snarky_answer(input_args->username);
            return;
        }
    }
    
    //roll the dice
    srand(time(NULL));
    int dice = rand()%sides + 1;
    char *dicestr = malloc(32);
    sprintf(dicestr, "%d", dice);

    int msglen = strlen(channel) + strlen(input_args->username) +strlen(dicestr) + 24;
    message= malloc(msglen);

    sprintf(message, "PRIVMSG %s :%s you got a: %s\n", channel, input_args->username, dicestr);
    irc_send(message);

    free(message);
    free(dicestr);
}
