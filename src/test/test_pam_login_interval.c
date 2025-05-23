#define _XOPEN_SOURCE 700

#include <pwd.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <security/pam_appl.h>
#include <security/pam_modules.h>

#include "../utils.h"

typedef int (*pam_sm_acct_mgmt_t)(pam_handle_t *, int, int, const char **);

pam_sm_acct_mgmt_t g_pam_sm_acct_mgmt = NULL;

char *last_pam_message = NULL;
static int test_conv(int num_msg, const struct pam_message **msg,
                     struct pam_response **resp, void *appdata_ptr)
{
    (void)num_msg;
    (void)resp;
    (void)appdata_ptr;
    if (num_msg > 0)
    {
        last_pam_message = strdup(msg[0]->msg);
    }

    return PAM_CONV_ERR;
}

struct pam_conv conv = {test_conv, NULL};

int try_login(int argc, const char **argv, const char *user)
{
    pam_handle_t *pamh = NULL;
    int flags = 0;

    int status = pam_start("test", user, &conv, &pamh);
    if (status != PAM_SUCCESS)
    {
        fprintf(stderr, "Error in pam_start: %s\n", pam_strerror(pamh, status));
        return 1;
    }

    status = g_pam_sm_acct_mgmt(pamh, flags, argc, argv);
    if (status == PAM_SUCCESS)
    {
        printf("Authentication successful\n");
    }
    else
    {
        printf("Authentication denied: %s. Message: %s\n", pam_strerror(pamh, status), last_pam_message);
    }

    free(last_pam_message);
    last_pam_message = NULL;
    pam_end(pamh, status);
    return status;
}

int test_pam_login_interval(int argc, const char **argv, const char *login_user, int expected_status)
{
    int status = try_login(argc, argv, login_user);
    if (status != expected_status)
    {
        printf("Expected status %d, got %d\n", expected_status, status);
        for (int i = 0; i < argc; i++)
        {
            printf(" argv[%d] = %s\n", i, argv[i]);
        }
        printf(" Login user: %s\n", login_user);
        return 1;
    }
    return 0;
}

int main(void)
{
    void *handle = dlopen("./pam_login_interval.so", RTLD_NOW);
    if (!handle)
    {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        return 1;
    }

    g_pam_sm_acct_mgmt = (pam_sm_acct_mgmt_t)(uintptr_t)dlsym(handle, "pam_sm_acct_mgmt");
    if (!g_pam_sm_acct_mgmt)
    {
        fprintf(stderr, "dlsym failed: %s\n", dlerror());
        dlclose(handle);
        return 1;
    }

    struct passwd *pw = getpwuid(getuid()); // Get the current user
    if (pw == NULL)
    {
        perror("getpwuid");
        return 1;
    }
    time_t last_login = last_login_time(pw->pw_name);

    int failed = 0;
    // log in as current user (no target_user set)
    failed += test_pam_login_interval(1, (const char *[]){"min_interval=0"}, pw->pw_name, PAM_SUCCESS);
    if (last_login != 0)
    {
        failed += test_pam_login_interval(1, (const char *[]){"min_interval=36500d"}, pw->pw_name, PAM_AUTH_ERR);
    }

    // log in with user '-' (not a valid user, so should never have logged in before)
    failed += test_pam_login_interval(1, (const char *[]){"min_interval=36500d"}, "-", PAM_SUCCESS);

    // login in as current user (target_user set)
    char target_user_arg[100];
    snprintf(target_user_arg, sizeof(target_user_arg), "target_user=%s", pw->pw_name);
    failed += test_pam_login_interval(2, (const char *[]){"min_interval=0", target_user_arg}, pw->pw_name, PAM_SUCCESS);
    if (last_login != 0)
    {
        failed += test_pam_login_interval(2, (const char *[]){"min_interval=36500d", target_user_arg}, pw->pw_name, PAM_AUTH_ERR);
    }

    // log in as user A (target_user set to B)
    failed += test_pam_login_interval(2, (const char *[]){"min_interval=0", "target_user=B"}, "A", PAM_SUCCESS);
    failed += test_pam_login_interval(2, (const char *[]){"min_interval=36500d", "target_user=B"}, "A", PAM_SUCCESS);

    if (last_login == 0)
    {
        printf("No previous login found for user %s, skipped failure test cases!\n", pw->pw_name);
    }

    if (failed > 0)
    {
        printf("%s: %d tests failed\n", __FILE__, failed);
        return 1;
    }

    return 0;
}