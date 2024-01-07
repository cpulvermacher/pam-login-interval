#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <security/pam_appl.h>

#include "pam_login_interval.c"

static int test_conv(int num_msg, const struct pam_message **msg,
                     struct pam_response **resp, void *appdata_ptr)
{
    (void)num_msg;
    (void)msg;
    (void)resp;
    (void)appdata_ptr;

    return PAM_CONV_ERR;
}

struct pam_conv conv = {test_conv, NULL};

int main(void)
{
    struct passwd *pw;
    pam_handle_t *pamh = NULL;
    int status;
    int flags = 0;
    int argc = 2;
    const char *argv[] = {"min_interval=100000"};

    pw = getpwuid(getuid()); // Get the current user
    if (pw == NULL)
    {
        perror("getpwuid");
        return 1;
    }

    status = pam_start("test", pw->pw_name, &conv, &pamh);
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
        printf("Authentication denied: %s\n", pam_strerror(pamh, status));
    }
    return 0;
}