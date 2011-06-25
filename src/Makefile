CFLAGS = -Wall -Wextra -Wno-unused -O2
LDFLAGS = -latf-c

all: cstow

cstow: cstow.o util.o path.o
	$(CC) $(CFLAGS) -o cstow cstow.o util.o path.o

test: relative_path_test
	atf-run | atf-report

relative_path_test: relative_path_test.c path.o util.o
	$(CC) $(CFLAGS) relative_path_test.c -o relative_path_test path.o util.o $(LDFLAGS)

clean:
	rm -f cstow *.o *~ relative_path_test

install: cstow
	@test x$(INSTALL_DIR) != x || ( echo "INSTALL_DIR not defined!. You should be calling this target from the toplevel makefile." && false )
	test ! -d $(INSTALL_DIR) && mkdir $(INSTALL_DIR) || true
	install -Sc cstow $(INSTALL_DIR)/cstow
