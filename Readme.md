# pam-login-interval

A PAM module that enforces a minimum time between logins.

Status: *Experimental.* Needs more testing, may or may not work.

### Prerequisites

gcc, make, and PAM development libraries are required.
Debian/Ubuntu-based:
```
sudo apt install gcc make libpam0g-dev
```

## Getting Started

Build and install the module via:
```
make
sudo make install
```

Add the following to your PAM config, e.g. to /etc/pam.d/lightdm to use this module with LightDM logins:
```
   account required pam_login_interval.so target_user=your.user.name min_interval=30m
```
change the `target_user` and `min_interval` arguments as required.

## Module Parameters

You should specify the following parameters in the PAM configuration:
- `target_user`: The user to limit logins for. For any other users, this module will not interfere with logins. If not specified, the `min_interval` setting will be applied to all users!
- `min_interval`: (required) The minimum time since the user's last login. This can be a number in seconds, or a duration in the form `1d2h3m4s` or `30m`. If a login is attempted before this time has elapsed, the login will be rejected.
