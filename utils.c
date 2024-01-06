#include <string.h>
#include <utmp.h>
#include <utmpx.h>
#include <stdlib.h>
#include <paths.h>

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

// Accepts the following arguments:
// target_user=<username>
// min_interval=<seconds>
int parse_args(int argc, const char **argv, char **target_user, long *min_seconds_between_logins)
{
    for (int i = 0; i < argc; i++)
    {
        if (strncmp(argv[i], "target_user=", 12) == 0)
        {
            *target_user = strdup(argv[i] + 12);
        }
        else if (strncmp(argv[i], "min_interval=", 13) == 0)
        {
            *min_seconds_between_logins = strtol(argv[i] + 13, NULL, 10);
        }
    }

    return 0;
}