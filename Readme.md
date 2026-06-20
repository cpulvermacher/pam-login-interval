# pam-login-interval

A PAM module that enforces a minimum time between logins.

## Prerequisites

gcc, make, PAM development libraries, and SQLite3 development libraries are required.
For Debian/Ubuntu-based systems:

```
sudo apt install gcc make libpam0g-dev libsqlite3-dev
```

## Installation

Build and install the module via:

```
make
sudo make install
```

Add the following to your PAM config, e.g. to /etc/pam.d/lightdm to use this module with LightDM logins:

```
   account required pam_login_interval.so target_user=your_user_name min_interval=30m
```

change the `target_user` and `min_interval` arguments as required.

If there is a successful authentication for the configured user before the configured `min_interval` time has passed, the login will be rejected.
If supported by the login UI / display manager, it will show a message like "Login denied (need to wait ... before next login)".

## Module Parameters

You should specify the following parameters in the PAM configuration:

### `target_user`

The user to limit logins for. For any other users, this module will not interfere with logins. If not specified, the `min_interval` setting will be applied to all users!
Only a single user name can be specified. To limit multiple users (but not everyone), add the pam_login_interval.so module to your config file multiple times.

### `min_interval` (required)

The minimum duration since the user's last login. This can be a number in seconds, or a duration in the form `1d2h3m4s` or `30m`. If a login is attempted before this time has elapsed, the login will be rejected.

## Login Time Tracking

This module automatically detects and uses the appropriate login tracking mechanism:

1. **lastlog2** (preferred): If `/var/lib/lastlog/lastlog2.db` exists, the module reads from this SQLite database updated by `pam_lastlog2.so`.

2. **wtmp** (fallback): If lastlog2 is not available, the module falls back to reading from the traditional wtmp file (`/var/log/wtmp`).

Note: If your distribution does not come with lastlog2 configured, you may need to install e.g. `libpam-lastlog2` and configure it in `/etc/pam.d/common-session` via:

```
session optional pam_lastlog2.so
```

## References

- The Linux-PAM Module Writers' Guide - https://fossies.org/linux/Linux-PAM-docs/doc/mwg/Linux-PAM_MWG.pdf
- pam-google-auth - https://github.com/google/google-authenticator-libpam/blob/master/src/pam_google_authenticator.c
- How to C (in 2016) - https://matt.sh/howto-c
