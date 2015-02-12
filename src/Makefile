CFLAGS = -Wall -Wextra -Wno-unused -O2
LDFLAGS = -latf-c

TUSH_HOME=$$(pwd)/../tush

all: cstow

cstow: cstow.o util.o path.o
	$(CC) $(CFLAGS) -o cstow cstow.o util.o path.o

test: cstow
	@PATH=$$PATH:$(TUSH_HOME)/bin:$$(pwd) tush-check TESTS.md

clean:
	rm -f cstow *.o *~

install: cstow
	@test x$(INSTALL_DIR) != x || ( echo "INSTALL_DIR not defined!. You should be calling this target from the toplevel makefile." && false )
	test ! -d $(INSTALL_DIR) && mkdir $(INSTALL_DIR) || true
	install -Sc cstow $(INSTALL_DIR)/cstow
