#include <time.h>
#include <stdio.h>
#include <security/pam_modules.h>

#include "utils.h"

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    const char *username = NULL;
    time_t now;
    time_t last_login;
    long min_seconds_between_logins = 60 * 60 * 24; // TODO make configurable
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

    if (seconds_since_last_login < min_seconds_between_logins)
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

// TODO check if I need these
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
