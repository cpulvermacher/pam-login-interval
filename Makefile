CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic $(shell pkg-config --cflags pam)
OBJS = utils.o
LIBS = $(shell pkg-config --libs pam) $(OBJS)
INSTALLDIR = /lib/security/


all: pam_login_interval.so

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c -o $@ $<

pam_login_interval.so: pam_login_interval.c utils.o
	$(CC) $(CFLAGS) -shared -o $@ pam_login_interval.c $(LIBS)

clean:
	rm -f pam_login_interval.so test_utils test_pam_login_interval $(OBJS)

install: pam_login_interval.so
	mkdir -p $(INSTALLDIR)
	cp pam_login_interval.so $(INSTALLDIR)

test: test_utils test_pam_login_interval
	./test_utils
	./test_pam_login_interval
	@echo "All tests OK."

test_utils: test_utils.c utils.o
	$(CC) $(CFLAGS) -o $@ $@.c $(LIBS)

test_pam_login_interval: test_pam_login_interval.c pam_login_interval.c utils.o
	$(CC) $(CFLAGS) -o $@ $@.c $(LIBS)