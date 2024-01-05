#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <lastlog.h>
#include <utmpx.h>
#include <security/pam_modules.h>

time_t last_login_time(char *target_user);

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    char *username = NULL;
    time_t now;
    time_t last_login;
    long seconds_since_last_login;

    /* Get the username */
    if (pam_get_user(pamh, &username, NULL) != PAM_SUCCESS)
    {
        return PAM_USER_UNKNOWN;
    }

    now = time(NULL);
    last_login = last_login_time(username);
    if (last_login == 0)
    {
        printf("no last login found\n");
        // not logged in before, so don't limit login
        return PAM_SUCCESS;
    }

    seconds_since_last_login = difftime(now, last_login);

    /* Check if the difference is less than X */
    if (seconds_since_last_login < 24 * 60 * 60 * 20)
    {
        printf("too soon\n");
        return PAM_AUTH_ERR;
    }
    else
    {
        printf("normal success\n");
        return PAM_SUCCESS;
    }
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    return PAM_SUCCESS;
}

time_t last_login_time(char *target_user)
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