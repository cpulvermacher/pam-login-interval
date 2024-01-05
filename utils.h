#ifndef UTILS_H
#define UTILS_H

#include <time.h>

time_t last_login_time(const char *target_user);

int parse_args(int argc, const char **argv, char **target_user, long *min_seconds_between_logins);

#endif