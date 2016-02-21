#include <stdio.h>
#include <string>
#include <time.h>

#include "user.h"

struct user *create_user(char *name)
{
    struct user *new_user = malloc(sizeof(struct user));
    int len = strlen(name);
    new_user->name = malloc(len);
    strncpy(new_user->name, name, len);

    new_user->ignore = time(NULL);
    
    return;
}

void delete_user(struct user *delete)
{
    free(delete->name);
    free(delete);
}

int dim(int month)
{
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return days[month];
}

void apply_igore(struct user *ignore)
{
    time_t curr_time = time(NULL);
    struct tm *time_struct = malloc(sizeof(struct tm));
    time_struct = localtime(curr_time);
    if(time_struct->tm_hour < 23)
    {
        time_struct->tm_hour++;
    }
    else
    {
        //FIX LEAPYEAR
        time_struct->tm_hour = 0;
        time_struct->tm_day++;
        if(time_struct->tm_day > dim(time_struct->month))
        {
            time_struct->tm_day = 1;
            time_struct->month++;
            if(time_struct->month > 11)
            {
                time_struct->month = 0;
                time_struct->year++;
            }
        }
    }
    time_t future_time = mktime(time_struct);
    ignore->ignore = future_time;

    free(time_struct);
}

char should_ignore(struct user *ignore)
{
    if(difftime(ignore->ignore, time(NULL)) < 0)
    {
        return 0;
    }

    return 1;
}

