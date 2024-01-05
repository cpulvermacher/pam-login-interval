CC = gcc
CFLAGS = -Wall  $(shell pkg-config --cflags pam)

all: pam_limit_logins.so

pam_limit_logins.so: pam_limit_logins.c
	$(CC) $(CFLAGS) -shared -o $@ $^ $(shell pkg-config --libs pam)

clean:
	rm -f pam_limit_logins.so

install: pam_limit_logins.so
	cp pam_check_lastlogin.so /lib/security/

test: test_last_login_time test_pam_limit_logins
	./test_last_login_time
	./test_pam_limit_logins
	@echo "All tests OK."

test_last_login_time: test_last_login_time.c pam_limit_logins.c
	$(CC) $(CFLAGS) -o $@ $@.c $(shell pkg-config --libs pam)

test_pam_limit_logins: test_pam_limit_logins.c pam_limit_logins.c
	$(CC) $(CFLAGS) -Wno-nonnull -o $@ $@.c $(shell pkg-config --libs pam)