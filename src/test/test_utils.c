#include <stdio.h>
#include <unistd.h>
#include <pwd.h>

#include "../utils.h"

int test_last_login_time(void)
{
    struct passwd *pw;
    time_t last_login;

    pw = getpwuid(getuid()); // Get the current user
    if (pw == NULL)
    {
        perror("getpwuid");
        return 1;
    }

    last_login = last_login_time(pw->pw_name);
    if (last_login == 0)
    {
        printf("No previous login found for user %s\n", pw->pw_name);
    }
    else
    {
        printf("Last login time for user %s: %s", pw->pw_name, ctime(&last_login));
    }
    return 0;
}

int test_parse_duration(const char *duration, uint64_t expected_seconds)
{
    uint64_t seconds;
    if (parse_duration(duration, &seconds) != 0)
    {
        printf("Failed to parse duration %s\n", duration);
        return 1;
    }

    if (seconds != expected_seconds)
    {
        printf("Expected %s to be parsed as %lu seconds, but got %lu seconds\n", duration, expected_seconds, seconds);
        return 1;
    }
    return 0;
}

int main(void)
{
    int failed = 0;

    failed += test_last_login_time();

    failed += test_parse_duration("350", 350);
    failed += test_parse_duration("1s", 1);
    failed += test_parse_duration("350s", 350);
    failed += test_parse_duration("1m", 60);
    failed += test_parse_duration("18m", 1080);
    failed += test_parse_duration("1h", 60 * 60);
    failed += test_parse_duration("1d", 60 * 60 * 24);
    failed += test_parse_duration("1d2h3s", 60 * 60 * 24 + 60 * 60 * 2 + 3);
    failed += test_parse_duration("0d0h0m0s", 0);
    failed += test_parse_duration("dhms", 0);

    if (failed == 0)
    {
        printf("test_utils: All tests passed\n");
    }
    else
    {
        printf("test_utils: %d tests failed\n", failed);
    }

    return failed;
}