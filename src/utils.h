#pragma once

#include <time.h>
#include <stdint.h>

/**
 * Retrieves the last login time for the specified user.
 *
 * @param target_user The username for which to retrieve the last login time.
 * @return The latest login time, or -1 on error.
 */
time_t last_login_time(const char *target_user);

/**
 * Parses the following module arguments:
 * - target_user=<username>
 * - min_interval=<duration> (e.g. 1d2h30m2s)
 *
 * @param argc The number of arguments
 * @param argv The array of arguments
 * @param target_user Pointer to store the target user. Points to data inside `argv` and should not be free()'d.
 * @param min_seconds_between_logins Pointer to store the minimum seconds between logins.
 * @return Returns 0 on success, 1 on failure.
 */
int parse_args(int argc, const char **argv, const char **target_user, uint64_t *min_seconds_between_logins);

/**
 * Parses a duration string and stores the corresponding number of seconds in the provided variable.
 *
 * @param duration String in the format <positive integer><unit>, where unit is one of: s, m, h, d.
 * @param seconds Pointer to a variable where the parsed number of seconds will be stored.
 * @return 0 if the duration was successfully parsed, 1 otherwise.
 */
int parse_duration(const char *duration, uint64_t *seconds);

/** prints `seconds` as "n days" / "n hours" / "n minutes" (depending on duration) into `buffer`, up to `size`. */
int print_duration(char *buffer, size_t size, uint64_t seconds);

/** prints a login denied message into `buffer`, up to `size`, with the remaining `seconds_remaining` */
int print_login_denied_msg(char *buffer, size_t size, uint64_t seconds_remaining);
