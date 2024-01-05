#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stddef.h>
#include <security/pam_appl.h>

#include "pam_limit_logins.c"

int main(void)
{
    struct passwd *pw;
    pam_handle_t *pamh;
    int flags = 0;
    int argc = 0;
    const char **argv = NULL;
    int authenticate_result;

    pw = getpwuid(getuid()); // Get the current user
    if (pw == NULL)
    {
        perror("getpwuid");
        return 1;
    }

    pam_start("test", pw->pw_name, NULL, &pamh);
    authenticate_result = pam_sm_authenticate(&pamh, flags, argc, argv);

    if (authenticate_result == PAM_SUCCESS)
    {
        printf("Authentication successful\n");
    }
    else
    {
        printf("Authentication denied, status code %d\n", authenticate_result);
    }
    return 0;
}