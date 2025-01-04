#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <utmp.h>
#include <utmpx.h>
#include <stdlib.h>
#include <paths.h>
#include <inttypes.h>

#include "utils.h"

time_t last_login_time(const char *target_user)
{
    struct utmpx *ut;
    char user[__UT_NAMESIZE + 1];
    time_t last_login = 0;

    // change from utmp (current state) to wtmp (historical state)
    if (utmpname(_PATH_WTMP) < 0)
    {
        return -1;
    }

    setutxent(); // rewind to the beginning of the wtmp file
    while ((ut = getutxent()) != NULL)
    {
        strncpy(user, ut->ut_user, __UT_NAMESIZE);
        user[__UT_NAMESIZE] = '\0'; // Ensure null-termination
        if (ut->ut_type == USER_PROCESS && strcmp(user, target_user) == 0 && ut->ut_tv.tv_sec > last_login)
        {
            last_login = ut->ut_tv.tv_sec;
        }
    }
    endutxent(); // close the wtmp file

    return last_login;
}

int parse_args(int argc, const char **argv, const char **target_user, uint64_t *min_seconds_between_logins)
{
    for (int i = 0; i < argc; i++)
    {
        if (strncmp(argv[i], "target_user=", 12) == 0)
        {
            *target_user = argv[i] + 12;
        }
        else if (strncmp(argv[i], "min_interval=", 13) == 0)
        {
            if (parse_duration(argv[i] + 13, min_seconds_between_logins) != 0)
            {
                return 1;
            }
        }
    }

    return 0;
}

int parse_duration(const char *duration, uint64_t *seconds)
{
    uint64_t number = 0;
    *seconds = 0;

    while (*duration != '\0')
    {
        if (*duration == 's')
        {
            *seconds += number;
            number = 0;
        }
        else if (*duration == 'm')
        {
            *seconds += number * 60;
            number = 0;
        }
        else if (*duration == 'h')
        {
            *seconds += number * 60 * 60;
            number = 0;
        }
        else if (*duration == 'd')
        {
            *seconds += number * 60 * 60 * 24;
            number = 0;
        }
        else if (*duration >= '0' && *duration <= '9')
        {
            number = number * 10 + (*duration - '0');
        }
        else
        {
            return 1;
        }
        duration++;
    }

    // any remaining `number` without unit is in seconds
    *seconds += number;

    return 0;
}

int print_duration(char *buffer, size_t size, uint64_t seconds)
{
    uint64_t days = seconds / (60 * 60 * 24);
    if (days > 0)
    {
        return snprintf(buffer, size, "%" PRIu64 " days", days);
    }

    uint64_t hours = seconds / (60 * 60);
    if (hours > 0)
    {
        return snprintf(buffer, size, "%" PRIu64 " hours", hours);
    }

    uint64_t minutes = seconds / 60;
    return snprintf(buffer, size, "%" PRIu64 " minutes", minutes);
}

int print_login_denied_msg(char *buffer, size_t size, uint64_t seconds_remaining)
{
    char duration[25];
    int duration_ret = print_duration(duration, sizeof(duration), seconds_remaining);
    if (duration_ret > 0)
    {
        int ret = snprintf(buffer, size, "Login denied (need to wait %s before next login)\n", duration);
        if (ret > 0)
        {
            return 0;
        }
    }

    // Fallback to a generic message
    snprintf(buffer, size, "Login denied (too soon after last login)\n");
    return 1;
}