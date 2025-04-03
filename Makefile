CC = gcc
PAM_CFLAGS = $(shell pkg-config --cflags pam || echo "-I/usr/include/security")
# via https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html
CFLAGS0 = -std=c11 -O2 -Wall -Wextra -Wpedantic
CFLAGS1 = -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough
CFLAGS2 = -Werror=format-security
CFLAGS3 = -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3
# needs GCC 13+
#CFLAGS4 = -fstrict-flex-arrays=3
CFLAGS5 = -fstack-clash-protection -fstack-protector-strong
# prevents dlopen(): -z,nodlopen -Wl
CFLAGS6 = -Wl,-z,noexecstack
CFLAGS7 = -Wl,-z,relro -Wl,-z,now
CFLAGS8 = -Wl,--as-needed -Wl,--no-copy-dt-needed-entries
# for production
CFLAGS9 = -fno-delete-null-pointer-checks -fno-strict-overflow -fno-strict-aliasing -ftrivial-auto-var-init=zero
CFLAGS = ${CFLAGS0} ${CFLAGS1} ${CFLAGS2} ${CFLAGS3} ${CFLAGS4} ${CFLAGS5} ${CFLAGS6} ${CFLAGS7} ${CFLAGS8} ${CFLAGS9} ${PAM_CFLAGS}
SOFLAGS = -fPIC -shared

LIBS = $(shell pkg-config --libs pam || echo "-lpam")
BUILDDIR = build
INSTALLDIR = /lib/security/
OBJS = $(BUILDDIR)/utils.o


all: pam_login_interval.so

clean:
	rm -f pam_login_interval.so $(BUILDDIR)/*

install: pam_login_interval.so
	mkdir -p $(INSTALLDIR)
	cp pam_login_interval.so $(INSTALLDIR)

test: $(BUILDDIR)/test_utils $(BUILDDIR)/test_pam_login_interval pam_login_interval.so
	./$(BUILDDIR)/test_utils
	./$(BUILDDIR)/test_pam_login_interval
	@echo "All tests OK."


$(BUILDDIR):
	mkdir -p $(BUILDDIR)

pam_login_interval.so: src/pam_login_interval.c $(BUILDDIR)/utils.o
	$(CC) $(CFLAGS) ${SOFLAGS} -o $@ src/pam_login_interval.c $(LIBS) $(OBJS)

$(BUILDDIR)/utils.o: src/utils.c src/utils.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/test_utils: src/test/test_utils.c $(BUILDDIR)/utils.o
	$(CC) $(CFLAGS) -o $@ src/test/test_utils.c $(LIBS) $(OBJS)

$(BUILDDIR)/test_pam_login_interval: src/test/test_pam_login_interval.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ src/test/test_pam_login_interval.c $(LIBS) $(OBJS)
