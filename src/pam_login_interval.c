#include <time.h>
#include <string.h>
#include <stdint.h>

#define PAM_SM_ACCOUNT
#include <security/pam_modules.h>

#include "utils.h"

int pam_log(pam_handle_t *pamh, const char *message)
{
    struct pam_message msg;
    struct pam_response *resp;
    const struct pam_message *msgp;
    struct pam_conv *conv;
    int retval;

    msg.msg_style = PAM_TEXT_INFO;
    msg.msg = message;
    msgp = &msg;

    retval = pam_get_item(pamh, PAM_CONV, (const void **)&conv);
    if (retval != PAM_SUCCESS)
    {
        return 1;
    }

    retval = conv->conv(1, &msgp, &resp, conv->appdata_ptr);
    if (retval != PAM_SUCCESS)
    {
        return 1;
    }

    return 0;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    char *target_user = NULL;
    uint64_t min_seconds_between_logins;
    const char *username = NULL;
    time_t now;
    time_t last_login;
    uint64_t seconds_since_last_login;

    if (parse_args(argc, argv, &target_user, &min_seconds_between_logins) != 0)
    {
        return PAM_AUTH_ERR;
    }

    /* Get the username */
    if (pam_get_user(pamh, &username, NULL) != PAM_SUCCESS)
    {
        return PAM_USER_UNKNOWN;
    }

    if (target_user != NULL && strcmp(username, target_user) != 0)
    {
        // not target user, allow login
        return PAM_SUCCESS;
    }

    now = time(NULL);
    last_login = last_login_time(username);
    if (last_login == 0)
    {
        // not logged in before, so don't limit login
        return PAM_SUCCESS;
    }

    seconds_since_last_login = difftime(now, last_login);

    if (seconds_since_last_login < min_seconds_between_logins)
    {
        if (!(flags & PAM_SILENT))
        {
            char message[100];
            print_login_denied_msg(message, sizeof(message), min_seconds_between_logins - seconds_since_last_login);
            pam_log(pamh, message);
        }
        return PAM_AUTH_ERR;
    }
    else
    {
        return PAM_SUCCESS;
    }
}
