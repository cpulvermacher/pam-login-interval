#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "pam_limit_logins.c"  // Include the source file that contains the last_login_time function

int main()
{
    struct passwd *pw;
    time_t last_login;

    pw = getpwuid(getuid());  // Get the current user
    if (pw == NULL) {
        perror("getpwuid");
        return 1;
    }

    last_login = last_login_time(pw->pw_name);  // Get the last login time of the current user
    if (last_login == 0) {
        printf("No previous login found for user %s\n", pw->pw_name);
    } else {
        printf("Last login time for user %s: %s", pw->pw_name, ctime(&last_login));
    }

    return 0;
}