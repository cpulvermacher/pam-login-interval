#include <stdio.h>
#include <string.h>
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

int test_print_duration(uint64_t seconds, const char *expected_duration)
{
    char duration[20];
    int ret = print_duration(duration, sizeof(duration), seconds);
    if (ret <= 0)
    {
        printf("Unexpected return value from print_duration: %d\n", ret);
        return 1;
    }

    if (strcmp(duration, expected_duration) != 0)
    {
        printf("Expected %lu seconds to be formatted as '%s', but got '%s'\n", seconds, expected_duration, duration);
        return 1;
    }
    return 0;
}

int test_print_duration_returns_length_ignoring_limit_null_buffer(void)
{
    int ret = print_duration(NULL, 0, 123);
    if (ret != 9) // "2 minutes" is 9 characters
    {
        printf("Unexpected return value from print_duration(NULL, 0, 123) %d\n", ret);
        return 1;
    }

    return 0;
}

int test_print_duration_returns_length_ignoring_limit(void)
{
    char duration[20];
    int ret = print_duration(duration, 0, 123);
    if (ret != 9) // "2 minutes" is 9 characters
    {
        printf("Unexpected return value from print_duration(..., 0, 123) %d\n", ret);
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

    failed += test_print_duration(0, "0 minutes");
    failed += test_print_duration(59, "0 minutes");
    failed += test_print_duration(60, "1 minutes");
    failed += test_print_duration(61, "1 minutes");
    failed += test_print_duration(60 * 59 + 1, "59 minutes");
    failed += test_print_duration(60 * 60, "1 hours");
    failed += test_print_duration(60 * 60 * 24, "1 days");
    failed += test_print_duration(60 * 60 * 24 * 500 + 1000, "500 days");
    failed += test_print_duration_returns_length_ignoring_limit();
    failed += test_print_duration_returns_length_ignoring_limit_null_buffer();

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