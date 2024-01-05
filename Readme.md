# pam-limit-logins

A PAM plugin to require a minimum time between logins

## Getting Started

Build and install the plugin via:
```
make
sudo make install
```

Add the following to your PAM config, ideally a new file in /etc/pam.d/:
```
   auth required pam_limit_logins.so user=your.user.name min_interval=3600
```
change the `user` and `min_interval` arguments as required.


### Prerequisites

gcc, make, and PAM development libraries are required.
Debian/Ubuntu-based:
```
sudo apt install gcc make libpam0g-dev
```
