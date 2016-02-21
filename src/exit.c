#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "command.h"
#include "irc.h"
#include "exit.h"

void bot_exit(void *args)
{
    struct arg *input_args = (struct arg *) args;
    if(!strcmp("foreverska", input_args->username))
    {
        exit(1);
    }
}
