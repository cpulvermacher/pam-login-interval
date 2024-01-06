# pam-limit-logins

A PAM module to require a minimum time between logins

### Prerequisites

gcc, make, and PAM development libraries are required.
Debian/Ubuntu-based:
```
sudo apt install gcc make libpam0g-dev
```

## Getting Started

Build and install the plugin via:
```
make
sudo make install
```

Add the following to your PAM config, ideally a new file in /etc/pam.d/:
```
   auth required pam_limit_logins.so user=your.user.name min_interval=30m
```
change the `user` and `min_interval` arguments as required.

## Module Parameters

You should specify the following parameters in the PAM configuration:
- `user`: The user to limit logins for. For any other users, this module will not interfere with logins. If not specified, the `min_interval` setting will be applied to all users!
- `min_interval`: (required) The minimum time since the user's last login. This can be a number in seconds, or a duration in the form `1d2h3m4s` or `30m`. If a login is attempted before this time has elapsed, the login will be rejected.
