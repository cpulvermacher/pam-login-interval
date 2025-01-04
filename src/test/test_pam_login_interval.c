#define _XOPEN_SOURCE 700

#include <pwd.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <security/pam_appl.h>
#include <security/pam_modules.h>

char *last_pam_message = NULL;
static int test_conv(int num_msg, const struct pam_message **msg,
                     struct pam_response **resp, void *appdata_ptr)
{
    (void)num_msg;
    last_pam_message = strdup(msg[0]->msg);
    (void)resp;
    (void)appdata_ptr;

    return PAM_CONV_ERR;
}

struct pam_conv conv = {test_conv, NULL};

int test_login(int argc, const char **argv, const char *current_user)
{
    pam_handle_t *pamh = NULL;
    int flags = 0;

    int status = pam_start("test", current_user, &conv, &pamh);
    if (status != PAM_SUCCESS)
    {
        fprintf(stderr, "Error in pam_start: %s\n", pam_strerror(pamh, status));
        return 1;
    }

    status = pam_sm_acct_mgmt(pamh, flags, argc, argv);
    if (status == PAM_SUCCESS)
    {
        printf("Authentication successful\n");
    }
    else
    {
        printf("Authentication denied: %s. Message: %s\n", pam_strerror(pamh, status), last_pam_message);
    }
    pam_end(pamh, status);
    return status;
}

int main(void)
{
    struct passwd *pw = getpwuid(getuid()); // Get the current user
    if (pw == NULL)
    {
        perror("getpwuid");
        return 1;
    }

    // log in as the current user, with zero interval
    const char *argv1[] = {"min_interval=0"};
    if (test_login(1, argv1, pw->pw_name) != PAM_SUCCESS)
    {
        printf("%s:%d: Expected PAM_SUCCESS\n", __FILE__, __LINE__);
        return 1;
    }

    // log in as the current user, but require last login before 100 years
    const char *argv2[] = {"min_interval=36500d"};
    if (test_login(1, argv2, pw->pw_name) != PAM_AUTH_ERR)
    {
        printf("%s:%d: Expected PAM_AUTH_ERR\n", __FILE__, __LINE__);
        return 1;
    }

    char target_user_arg[100];
    snprintf(target_user_arg, sizeof(target_user_arg), "target_user=%s", pw->pw_name);

    // log in as the current user, with zero interval (target_user set)
    const char *argv3[] = {target_user_arg, "min_interval=0"};
    if (test_login(2, argv3, pw->pw_name) != PAM_SUCCESS)
    {
        printf("%s:%d: Expected PAM_SUCCESS\n", __FILE__, __LINE__);
        return 1;
    }

    // log in as the current user, but require last login before 100 years (target_user set)
    const char *argv4[] = {target_user_arg, "min_interval=36500d"};
    if (test_login(2, argv4, pw->pw_name) != PAM_AUTH_ERR)
    {
        printf("%s:%d: Expected PAM_AUTH_ERR\n", __FILE__, __LINE__);
        return 1;
    }

    // log in as other user, with zero interval (target_user set)
    const char *argv5[] = {"target_user=A", "min_interval=0"};
    if (test_login(2, argv5, "B") != PAM_SUCCESS)
    {
        printf("%s:%d: Expected PAM_SUCCESS\n", __FILE__, __LINE__);
        return 1;
    }

    // log in as other user, but require last login before 100 years (target_user set)
    const char *argv6[] = {"target_user=A", "min_interval=36500d"};
    if (test_login(2, argv6, "B") != PAM_SUCCESS)
    {
        printf("%s:%d: Expected PAM_SUCCESS\n", __FILE__, __LINE__);
        return 1;
    }

    return 0;
}