#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <inttypes.h>

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
        printf("Expected %s to be parsed as %" PRIu64 " seconds, but got %" PRIu64 " seconds\n", duration, expected_seconds, seconds);
        return 1;
    }
    return 0;
}

int test_parse_duration_expect_error(const char *duration)
{
    uint64_t seconds;
    if (parse_duration(duration, &seconds) == 0)
    {
        printf("Expected parse_duration to return an error for %s\n", duration);
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
        printf("Expected %" PRIu64 " seconds to be formatted as '%s', but got '%s'\n", seconds, expected_duration, duration);
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

int test_print_login_denied_msg(uint64_t seconds_remaining, const char *expected_msg)
{
    char buffer[100];
    int ret = print_login_denied_msg(buffer, sizeof(buffer), seconds_remaining);
    if (ret != 0)
    {
        printf("Unexpected return value from print_login_denied_msg: %d\n", ret);
        return 1;
    }

    if (strcmp(buffer, expected_msg) != 0)
    {
        printf("Expected message to be '%s', but got '%s'\n", expected_msg, buffer);
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
    failed += test_parse_duration("213503982334601d", 18446744073709526400u);
    failed += test_parse_duration("18446744073709551615", UINT64_MAX);
    failed += test_parse_duration("18446744073709551615s", UINT64_MAX);
    failed += test_parse_duration("dhms", 0);
    failed += test_parse_duration("", 0);
    failed += test_parse_duration_expect_error("-1s");
    failed += test_parse_duration_expect_error("-55");
    failed += test_parse_duration_expect_error("abc");
    failed += test_parse_duration_expect_error(u8"ユニコード");
    failed += test_parse_duration_expect_error(" ");

    failed += test_print_duration(0, "0 minutes");
    failed += test_print_duration(59, "0 minutes");
    failed += test_print_duration(60, "1 minutes");
    failed += test_print_duration(61, "1 minutes");
    failed += test_print_duration(60 * 59 + 1, "59 minutes");
    failed += test_print_duration(60 * 60, "1 hours");
    failed += test_print_duration(60 * 60 * 24, "1 days");
    failed += test_print_duration(60 * 60 * 24 * 500 + 1000, "500 days");
    failed += test_print_duration(UINT64_MAX, "213503982334601 day"); // longer than buffer, but should not overflow
    failed += test_print_duration(-1, "213503982334601 day");         // longer than buffer, but should not overflow
    failed += test_print_duration_returns_length_ignoring_limit();
    failed += test_print_duration_returns_length_ignoring_limit_null_buffer();

    failed += test_print_login_denied_msg(0, "Login denied (need to wait 0 minutes before next login)\n");
    failed += test_print_login_denied_msg(3602, "Login denied (need to wait 1 hours before next login)\n");
    failed += test_print_login_denied_msg(60 * 60 * 24 * 7, "Login denied (need to wait 7 days before next login)\n");
    failed += test_print_login_denied_msg(UINT64_MAX, "Login denied (need to wait 213503982334601 days before next login)\n");

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