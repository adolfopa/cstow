CFLAGS = -ansi -pedantic -Wall -Werror -Wextra -O2

cstow: cstow.o
	cc $(CFLAGS) -o cstow cstow.o
cstow.o:

.PHONY: clean test install

test:
	atf-run | atf-report

clean:
	rm -f cstow cstow.o *~

install: cstow
	test ! -d $(INSTALL_DIR) && mkdir $(INSTALL_DIR) || true
	install -Sc cstow $(INSTALL_DIR)/cstow