CC = gcc
CFLAGS = -Wall  $(shell pkg-config --cflags pam)
LIBS = $(shell pkg-config --libs pam)
INSTALLDIR = /lib/security/


all: pam_limit_logins.so

pam_limit_logins.so: pam_limit_logins.c
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LIBS)

clean:
	rm -f pam_limit_logins.so

install: pam_limit_logins.so
	mkdir -p $(INSTALLDIR)
	cp pam_check_lastlogin.so $(INSTALLDIR)
test: test_last_login_time test_pam_limit_logins
	./test_last_login_time
	./test_pam_limit_logins
	@echo "All tests OK."

test_last_login_time: test_last_login_time.c pam_limit_logins.c
	$(CC) $(CFLAGS) -o $@ $@.c $(LIBS)

test_pam_limit_logins: test_pam_limit_logins.c pam_limit_logins.c
	$(CC) $(CFLAGS) -o $@ $@.c $(LIBS)