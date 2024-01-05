#include <string.h>
#include <utmpx.h>

#include "utils.h"

time_t last_login_time(const char *target_user)
{
    struct utmpx *ut;
    char user[__UT_NAMESIZE + 1];
    time_t last_login = 0;

    setutxent(); // rewind to the beginning of the wtmp file
    while ((ut = getutxent()) != NULL)
    {
        strncpy(user, ut->ut_user, __UT_NAMESIZE);
        user[__UT_NAMESIZE] = '\0'; // Ensure null-termination
        if (ut->ut_type == USER_PROCESS && strcmp(user, target_user) == 0)
        {
            last_login = ut->ut_tv.tv_sec;
            break;
        }
    }
    endutxent(); // close the wtmp file

    return last_login;
}