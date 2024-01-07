CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic $(shell pkg-config --cflags pam)
OBJS = utils.o
LIBS = $(shell pkg-config --libs pam)
INSTALLDIR = /lib/security/


all: pam_login_interval.so

clean:
	rm -f pam_login_interval.so test_utils test_pam_login_interval $(OBJS)

install: pam_login_interval.so
	mkdir -p $(INSTALLDIR)
	cp pam_login_interval.so $(INSTALLDIR)

test: test_utils test_pam_login_interval
	./test_utils
	./test_pam_login_interval
	@echo "All tests OK."


utils.o: src/utils.c src/utils.h
	$(CC) $(CFLAGS) -c -o $@ $<

pam_login_interval.so: src/pam_login_interval.c utils.o
	$(CC) $(CFLAGS) -shared -o $@ src/pam_login_interval.c $(LIBS) $(OBJS)

test_utils: src/test/test_utils.c utils.o
	$(CC) $(CFLAGS) -o $@ src/test/test_utils.c $(LIBS) $(OBJS)

test_pam_login_interval: src/test/test_pam_login_interval.c pam_login_interval.so
	$(CC) $(CFLAGS) -o $@ src/test/test_pam_login_interval.c $(LIBS) ./pam_login_interval.so