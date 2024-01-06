CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic $(shell pkg-config --cflags pam)
OBJS = utils.o
LIBS = $(shell pkg-config --libs pam) $(OBJS)
INSTALLDIR = /lib/security/


all: pam_limit_logins.so

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c -o $@ $<

pam_limit_logins.so: pam_limit_logins.c utils.o
	$(CC) $(CFLAGS) -shared -o $@ pam_limit_logins.c $(LIBS)

clean:
	rm -f pam_limit_logins.so test_last_login_time test_pam_limit_logins $(OBJS)

install: pam_limit_logins.so
	mkdir -p $(INSTALLDIR)
	cp pam_limit_logins.so $(INSTALLDIR)

test: test_last_login_time test_pam_limit_logins
	./test_last_login_time
	./test_pam_limit_logins
	@echo "All tests OK."

test_last_login_time: test_last_login_time.c utils.o
	$(CC) $(CFLAGS) -o $@ $@.c $(LIBS)

test_pam_limit_logins: test_pam_limit_logins.c pam_limit_logins.c utils.o
	$(CC) $(CFLAGS) -o $@ $@.c $(LIBS)