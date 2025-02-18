# pam-login-interval

[![Status](https://flat.badgen.net/github/checks/cpulvermacher/pam-login-interval)](https://github.com/cpulvermacher/pam-login-interval/actions/workflows/ci.yml)

A PAM module that enforces a minimum time between logins.

## Prerequisites

gcc, make, and PAM development libraries are required.
For Debian/Ubuntu-based systems:
```
sudo apt install gcc make libpam0g-dev
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

### `min_interval` (required)
The minimum duration since the user's last login. This can be a number in seconds, or a duration in the form `1d2h3m4s` or `30m`. If a login is attempted before this time has elapsed, the login will be rejected.

## References
- The Linux-PAM Module Writers' Guide -  https://fossies.org/linux/Linux-PAM-docs/doc/mwg/Linux-PAM_MWG.pdf
- pam-google-auth - https://github.com/google/google-authenticator-libpam/blob/master/src/pam_google_authenticator.c
- How to C (in 2016) - https://matt.sh/howto-c
