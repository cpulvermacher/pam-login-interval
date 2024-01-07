CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic $(shell pkg-config --cflags pam)
LIBS = $(shell pkg-config --libs pam)
BUILDDIR = build
INSTALLDIR = /lib/security/
OBJS = $(BUILDDIR)/utils.o


all: pam_login_interval.so

clean:
	rm -f pam_login_interval.so $(BUILDDIR)/*

install: pam_login_interval.so
	mkdir -p $(INSTALLDIR)
	cp pam_login_interval.so $(INSTALLDIR)

test: $(BUILDDIR)/test_utils $(BUILDDIR)/test_pam_login_interval
	./$(BUILDDIR)/test_utils
	./$(BUILDDIR)/test_pam_login_interval
	@echo "All tests OK."


$(BUILDDIR):
	mkdir -p $(BUILDDIR)

pam_login_interval.so: src/pam_login_interval.c $(BUILDDIR)/utils.o
	$(CC) $(CFLAGS) -shared -o $@ src/pam_login_interval.c $(LIBS) $(OBJS)

$(BUILDDIR)/utils.o: src/utils.c src/utils.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/test_utils: src/test/test_utils.c $(BUILDDIR)/utils.o
	$(CC) $(CFLAGS) -o $@ src/test/test_utils.c $(LIBS) $(OBJS)

$(BUILDDIR)/test_pam_login_interval: src/test/test_pam_login_interval.c pam_login_interval.so | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ src/test/test_pam_login_interval.c $(LIBS) ./pam_login_interval.so